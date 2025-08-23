#pragma once


#include <vulkan/vulkan.hpp>
#include <memory>
#include <algorithm>
#include "../Buffer.hpp"
#include "../Image.hpp"
namespace RE {
	struct Allocation {
		vk::DeviceMemory memory = nullptr;
		vk::DeviceSize size = 0;
		vk::DeviceSize offset = 0;
		Allocation() = default;
		Allocation(const vk::DeviceMemory& memory,const vk::DeviceSize& size,const vk::DeviceSize& offset):
			memory(memory),size(size),offset(offset){ }
		bool operator==(const Allocation& other) const {
			return memory == other.memory &&
				offset == other.offset &&
				size == other.size;
		}
	};

	struct MemoryBlock {
		vk::DeviceMemory memory = nullptr;
		vk::DeviceSize size = 0;
		struct FreeRegion{
			vk::DeviceSize size = 0;
			vk::DeviceSize offset = 0;
			FreeRegion() = default;
			FreeRegion(const vk::DeviceSize& size,const vk::DeviceSize& offset):size(size),offset(offset){}
		};
		std::vector<FreeRegion> freeregions;
	};

	class Allocator {
	public:
		Allocator() = default;
		Allocator(const Allocator& other) = delete;
		Allocator& operator=(const Allocator& other) = delete;
		Allocator(Allocator&& other) = default;
		Allocator& operator=(Allocator&& other) = default;

		Allocator& setDevice(const vk::Device& device);
		Allocator& setPhysicalDevice(const vk::PhysicalDevice& physicaldevice);
		UT::Buffer createBuffer(const UT::BufferCreateInfo& createinfo);
		UT::Image createImage(const UT::ImageCreateInfo& createinfo);
		Allocation allocate(const vk::DeviceSize& size, const vk::DeviceSize& alignment, const uint32_t& memorytypeindex);
		void free(const Allocation& allocation);
		void destroyBuffer(const vk::Buffer& buffer);
		void destroyImage(const vk::Image& image);
		void destroy();
	private:
		std::optional<uint32_t> findMemorytypeIndex(const uint32_t& typeFilter, vk::MemoryPropertyFlags memorypropertyflags);		
		MemoryBlock& createMemoryBlock(const vk::DeviceSize& size, const uint32_t& memorytypeindex);
		void mergeFreeRegions(MemoryBlock& block);
		vk::DeviceSize nextBlockSize = 16 * 1024 * 1024;       // 初始块大小 16MB
		const vk::DeviceSize maxBlockSize = 256 * 1024 * 1024; // 最大块大小 256MB
		vk::DeviceSize getNextBlockSize(const vk::DeviceSize& requestSize);
		vk::Device device = nullptr;
		vk::PhysicalDevice physicaldevice = nullptr;
		std::vector<MemoryBlock> memoryblocks;
	};
}