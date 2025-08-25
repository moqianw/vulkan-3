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
				if (memoryblock.mapped) {
					device.unmapMemory(memoryblock.memory);
					memoryblock.mapped = false;
					memoryblock.mappedPtr = nullptr;
				}
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
		block.memoryTypeIndex = memorytypeindex;
		block.mapped = false;
		block.mappedPtr = nullptr;
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
				auto memProps = physicaldevice.getMemoryProperties();
				if ((memProps.memoryTypes[index].propertyFlags & vk::MemoryPropertyFlagBits::eHostVisible) == vk::MemoryPropertyFlagBits::eHostVisible) {
					MemoryBlock* ownerBlock = nullptr;
					for (auto& block : memoryblocks) {
						if (block.memory == alloc.memory) { ownerBlock = &block; break; }
					}
					if (ownerBlock) {
						if (!ownerBlock->mapped) {
							ownerBlock->mappedPtr = device.mapMemory(ownerBlock->memory, 0, VK_WHOLE_SIZE);
							ownerBlock->mapped = true;
						}
						ubuffer.ptr = static_cast<char*>(ownerBlock->mappedPtr) + ubuffer.offset;
					}
				}
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
			if (block.memoryTypeIndex != memorytypeindex) continue; // 仅从相同类型的块里分配
			for (size_t idx = 0; idx < block.freeregions.size(); ++idx) {
				auto &reg = block.freeregions[idx];
				vk::DeviceSize alignedOffset = (reg.offset + alignment - 1) & ~(alignment - 1);
				vk::DeviceSize regionStart = reg.offset;
				vk::DeviceSize regionEnd   = reg.offset + reg.size;

				if (alignedOffset + alignedSize <= regionEnd) {
					vk::DeviceSize freeEnd = regionEnd;
					vk::DeviceSize leftSize  = (alignedOffset > regionStart) ? (alignedOffset - regionStart) : 0;
					vk::DeviceSize rightStart = alignedOffset + alignedSize;
					vk::DeviceSize rightSize  = (rightStart < freeEnd) ? (freeEnd - rightStart) : 0;

					if (leftSize == 0 && rightSize == 0) {
						// 完全占满该区间
						block.freeregions.erase(block.freeregions.begin() + idx);
					} else if (leftSize > 0 && rightSize == 0) {
						// 只剩左边
						reg.size = leftSize;
					} else if (leftSize == 0 && rightSize > 0) {
						// 只剩右边
						reg.offset = rightStart;
						reg.size   = rightSize;
					} else {
						// 左右都剩：当前改为左片段，右片段插入到 idx+1
						reg.size = leftSize;
						RE::MemoryBlock::FreeRegion right{ rightSize, rightStart };
						block.freeregions.insert(block.freeregions.begin() + idx + 1, right);
					}

					return Allocation(block.memory, alignedSize, alignedOffset);
				}
			}
		}

		// ---- 没有可用的，创建新的 block ----
		vk::DeviceSize newBlockSize = getNextBlockSize(size);
		auto& block = createMemoryBlock(newBlockSize, memorytypeindex);

		// 使用首个空闲区间分配
		vk::DeviceSize alignedOffset = (block.freeregions[0].offset + alignment - 1) & ~(alignment - 1);
		vk::DeviceSize freeEnd = block.freeregions[0].offset + block.freeregions[0].size;
		block.freeregions[0].offset = alignedOffset + alignedSize;
		block.freeregions[0].size = freeEnd - block.freeregions[0].offset;
		if (block.freeregions[0].size == 0) block.freeregions.erase(block.freeregions.begin());

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