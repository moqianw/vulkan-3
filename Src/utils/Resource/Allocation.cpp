#include "Allocation.hpp"

namespace RE {
	Allocator& Allocator::setDevice(const vk::Device& device) {
		this->device = device;
		return *this;
	}
	Allocator& Allocator::setPhysicalDevice(const vk::PhysicalDevice& device) {
		this->physicaldevice = device;
		return *this;
	}
	void Allocator::destroy() {
		if (!device) return;
		for (auto& memoryblock : memoryblocks) {
			if (memoryblock.memory) {
				device.freeMemory(memoryblock.memory);
			}
			memoryblock.memory = nullptr;
		}
		memoryblocks.clear();
	}
	std::optional<uint32_t> Allocator::findMemorytypeIndex(
		const uint32_t& typeFilter, vk::MemoryPropertyFlags memorypropertyflags
	) {
		if (!physicaldevice) throw std::runtime_error("not set physicaldevice");
		auto types = physicaldevice.getMemoryProperties();
		for (uint32_t i = 0; i < types.memoryTypeCount; i++) {
			if ((typeFilter & (1 << i)) &&
				(types.memoryTypes[i].propertyFlags & memorypropertyflags) == memorypropertyflags) {
				return i;
			}
		}
		throw std::runtime_error("cannot find MemorytypeIndex");
	}
	MemoryBlock& Allocator::createMemoryBlock(const vk::DeviceSize& size, const uint32_t& memorytypeindex)
	{
		if (!device) throw std::runtime_error("Allocate Memory ERROR: not set device");
		MemoryBlock block;
		vk::MemoryAllocateInfo allocateinfo;
		allocateinfo.setAllocationSize(size)
			.setMemoryTypeIndex(memorytypeindex);
		block.memory = device.allocateMemory(allocateinfo);
		if(!block.memory) throw std::runtime_error("Allocate Memory ERROR: allocate memory block error");
		block.size = size;
		block.freeregions.emplace_back(size, 0);
		memoryblocks.emplace_back(block);
		return memoryblocks.back();
	}
	void Allocator::mergeFreeRegions(MemoryBlock& block) {
		std::sort(block.freeregions.begin(), block.freeregions.end(),
			[](const auto& a, const auto& b) { return a.offset < b.offset; });
		for (size_t i = 0; i + 1 < block.freeregions.size();) {
			auto& cur = block.freeregions[i];
			auto& next = block.freeregions[i + 1];
			if (cur.offset + cur.size == next.offset) {
				cur.size += next.size;
				block.freeregions.erase(block.freeregions.begin() + i + 1);
			}
			else {
				i++;
			}
		}
	}

	UT::Buffer Allocator::createBuffer(const UT::BufferCreateInfo& createinfo)
	{
		if (!device) throw std::runtime_error("create buffer not set device");

		// 创建 VkBuffer
		auto buffer = device.createBuffer(createinfo.buffercreateinfo);
		if (!buffer) throw std::runtime_error("create buffer false");

		UT::Buffer ubuffer;
		ubuffer.buffer = buffer;

		if (createinfo.allocateMemory) {
			auto memReq = device.getBufferMemoryRequirements(buffer);

			auto index = findMemorytypeIndex(memReq.memoryTypeBits, createinfo.memorypropertyflags).value();
			auto alloc = allocate(memReq.size, memReq.alignment, index);

			device.bindBufferMemory(buffer, alloc.memory, alloc.offset);

			ubuffer.memory = alloc.memory;
			ubuffer.offset = alloc.offset;
			ubuffer.size = alloc.size;

			if (createinfo.mapmemory) {
				// 用 VK_WHOLE_SIZE 避免 size 对齐问题
				ubuffer.ptr = device.mapMemory(ubuffer.memory, ubuffer.offset, VK_WHOLE_SIZE);
			}
		}

		return ubuffer;
	}

	UT::Image Allocator::createImage(const UT::ImageCreateInfo& createinfo)
	{
		if (!device) throw std::runtime_error("Image create Error: not set device");

		auto image = device.createImage(createinfo.imagecreateinfo);
		if (!image) throw std::runtime_error("create image false");
		UT::Image uimage;
		uimage.image = image;
		if (createinfo.allocatememory) {
			auto requirement = device.getImageMemoryRequirements(image);
			auto index = findMemorytypeIndex(requirement.memoryTypeBits, createinfo.memorypropertyflags).value();
			auto alloc = allocate(requirement.size, requirement.alignment, index);
			device.bindImageMemory(image, alloc.memory, alloc.offset);
			uimage.memory = alloc.memory;
			uimage.offset = alloc.offset;
			uimage.size = alloc.size;
		}
		if (createinfo.createimageview) {
			auto viewinfo = createinfo.imageviewcreateinfo;
			viewinfo.setImage(uimage.image);
			uimage.imageview = device.createImageView(viewinfo);
		}
		return uimage;
	}

	Allocation Allocator::allocate(
		const vk::DeviceSize& size, const vk::DeviceSize& alignment, const uint32_t& memorytypeindex)
	{
		assert((alignment & (alignment - 1)) == 0 && "Alignment must be power of two");

		// ---- 按 alignment 对齐 size ----
		vk::DeviceSize alignedSize = (size + alignment - 1) & ~(alignment - 1);

		for (auto& block : memoryblocks) {
			for (auto it = block.freeregions.begin(); it != block.freeregions.end(); ++it) {
				vk::DeviceSize alignedOffset = (it->offset + alignment - 1) & ~(alignment - 1);

				if (alignedOffset + alignedSize <= it->offset + it->size) {
					vk::DeviceSize freeEnd = it->offset + it->size;

					// 左边剩余空间
					if (alignedOffset > it->offset) {
						block.freeregions.insert(it, { it->offset, alignedOffset - it->offset });
					}

					// 更新 freeRegion
					it->offset = alignedOffset + alignedSize;
					it->size = freeEnd - it->offset;
					if (it->size == 0) block.freeregions.erase(it);

					return Allocation(block.memory, alignedSize, alignedOffset);
				}
			}
		}

		// ---- 如果没有可用的，就新建一个 block ----
		vk::DeviceSize newBlockSize = getNextBlockSize(size);
		auto& block = createMemoryBlock(newBlockSize, memorytypeindex);

		auto it = block.freeregions.begin();
		vk::DeviceSize alignedOffset = (it->offset + alignment - 1) & ~(alignment - 1);

		vk::DeviceSize freeEnd = it->offset + it->size;
		it->offset = alignedOffset + alignedSize;
		it->size = freeEnd - it->offset;
		if (it->size == 0) block.freeregions.erase(it);

		return Allocation(block.memory, alignedSize, alignedOffset);
	}


	vk::DeviceSize Allocator::getNextBlockSize(const vk::DeviceSize& size) {
		vk::DeviceSize blockSize = std::max(size, nextBlockSize);
		nextBlockSize = std::min(blockSize * 2, maxBlockSize);
		return blockSize;
	}
	void Allocator::free(const Allocation& allocation)
	{
		for (auto& block : memoryblocks) {
			if (allocation.memory == block.memory) {
				block.freeregions.emplace_back(allocation.size,allocation.offset);
				mergeFreeRegions(block);
				return;
			}
		}
	}

	void Allocator::destroyBuffer(const vk::Buffer& buffer)
	{
	}

	void Allocator::destroyImage(const vk::Image& image)
	{
	}

}