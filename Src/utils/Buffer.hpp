#pragma once
#include <vulkan/vulkan.hpp>
#include <optional>
namespace UT {
	class BufferCreateInfo {
	public:

		vk::BufferCreateInfo buffercreateinfo;
		vk::MemoryPropertyFlags memorypropertyflags;
		bool allocateMemory = false;
		bool mapmemory = false;
		BufferCreateInfo() {

		}
		BufferCreateInfo(const BufferCreateInfo& it) :
			buffercreateinfo(it.buffercreateinfo),
			allocateMemory(it.allocateMemory),
			memorypropertyflags(it.memorypropertyflags)
		{
		}
		BufferCreateInfo& operator=(const BufferCreateInfo& it)
		{
			buffercreateinfo = it.buffercreateinfo;
			allocateMemory = it.allocateMemory;
			memorypropertyflags = it.memorypropertyflags;
		}
		BufferCreateInfo& setBufferCreateInfo(const vk::BufferCreateInfo& buffercreateinfo) {
			this->buffercreateinfo = buffercreateinfo;
			return *this;
		}
		BufferCreateInfo& setMemoryPropertyFlags(const vk::MemoryPropertyFlags& memorypropertyflags) {
			this->memorypropertyflags = memorypropertyflags;
			return *this;
		}
		BufferCreateInfo& setAllocateMemoryEnable(const bool& is) {
			this->allocateMemory = is;
			return *this;
		}
		BufferCreateInfo& setMapMemoryEnable(const bool& is) {
			this->mapmemory = is;
			return *this;
		}
	};
	struct Buffer {

		Buffer() = default;
		~Buffer() = default;
		Buffer(const Buffer& other);
		Buffer& operator=(const Buffer& other);
		vk::DeviceMemory memory = nullptr;
		vk::DeviceSize size = 0;
		vk::DeviceSize offset = 0;
		vk::Buffer buffer = nullptr;
		void* ptr = nullptr;
	};
}