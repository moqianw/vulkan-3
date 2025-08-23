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
			if (memoryblock.memory) device.freeMemory(memoryblock.memory);
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
	vk::Buffer Allocator::createBuffer(const UT::BufferCreateInfo& createinfo)
	{
		return vk::Buffer();
	}

	vk::Image Allocator::createImage(const UT::ImageCreateInfo& createinfo)
	{
		return vk::Image();
	}

	Allocation Allocator::allocate(const vk::DeviceSize& size, const vk::DeviceSize& alignment, const uint32_t& memorytypeindex)
	{
		for (auto& block : memoryblocks) {
			for (auto it = block.freeregions.begin(); it != block.freeregions.end(); it++) {
				vk::DeviceSize alignedOffset = (it->offset + alignment - 1) & ~(alignment - 1);
				if (alignedOffset + size <= it->offset + it->size) {

					Allocation alloc(block.memory, size, alignedOffset);
					vk::DeviceSize freeEnd = it->offset + it->size;
					it->offset = alignedOffset + size;
					it->size = freeEnd - it->offset;
					if (it->size == 0) block.freeregions.erase(it);
					return alloc;
				}
			}
		}
		auto& block = createMemoryBlock(std::max(size, static_cast<vk::DeviceSize>(256 * 1024 * 1024)), memorytypeindex);
		auto it = block.freeregions.begin();
		vk::DeviceSize alignedOffset = (it->offset + alignment - 1) & ~(alignment - 1);
		Allocation alloc(block.memory, size, alignedOffset);
		vk::DeviceSize freeEnd = it->offset + it->size;
		it->offset = alignedOffset + size;
		it->size = freeEnd - it->offset;
		if (it->size == 0) block.freeregions.erase(it);
		return alloc;
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