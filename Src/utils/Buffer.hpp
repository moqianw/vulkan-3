#pragma once
#include <vulkan/vulkan.hpp>
#include <optional>
namespace UT {
	class BufferCreateInfo {
	protected:
		friend class Buffer;
		vk::BufferCreateInfo buffercreateinfo;
		vk::MemoryPropertyFlags memorypropertyflags;
		bool allocateMemory = false;
		bool mapMemory = false;
		vk::Device device = nullptr;
		vk::PhysicalDevice physicaldevice = nullptr;
		static std::optional<uint32_t> findMemorytypeIndex(const vk::PhysicalDevice& physicaldevice
			, const uint32_t& typeFilter, vk::MemoryPropertyFlags memorypropertyflags)
		{
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
	public:
		BufferCreateInfo(){

		}
		BufferCreateInfo(const BufferCreateInfo& it) :
			buffercreateinfo(it.buffercreateinfo),
			allocateMemory(it.allocateMemory),
			mapMemory(it.mapMemory),
			device(it.device),
			memorypropertyflags(it.memorypropertyflags)
			,physicaldevice(it.physicaldevice)
		{
		}
		BufferCreateInfo& operator=(const BufferCreateInfo& it)
		{
			buffercreateinfo = it.buffercreateinfo;
			allocateMemory = it.allocateMemory;
			mapMemory = it.mapMemory;
			device = it.device;
			memorypropertyflags = it.memorypropertyflags;
			physicaldevice = it.physicaldevice;
		}
		BufferCreateInfo& setBufferCreateInfo(const vk::BufferCreateInfo& buffercreateinfo) {
			this->buffercreateinfo = buffercreateinfo;
			return *this;
		}
		BufferCreateInfo& setMemoryPropertyFlags(const vk::MemoryPropertyFlags& memorypropertyflags) {
			this->memorypropertyflags = memorypropertyflags;
			return *this;
		}
		BufferCreateInfo& allocateMemoryEnable(const bool& is) {
			this->allocateMemory = is;
			return *this;
		}
		BufferCreateInfo& mapMemoryEnable(const bool& is) {
			this->mapMemory = is;
			return *this;
		}
		BufferCreateInfo& setDevice(const vk::Device& device) {
			this->device = device;
			return *this;
		}
		BufferCreateInfo& setPhysicalDevice(const vk::PhysicalDevice& device) {
			this->physicaldevice = device;
			return *this;
		}
	};
	class Buffer {
	public:
		Buffer(const BufferCreateInfo& createinfo);
		~Buffer();
		Buffer(const Buffer&) = delete;
		Buffer& operator=(const Buffer&) = delete;

		Buffer(Buffer&& other) noexcept
			: device(other.device), buffer(other.buffer), memory(other.memory), ptr(other.ptr)
		{
			other.buffer = nullptr;
			other.memory = nullptr;
			other.ptr = nullptr;
		}

		Buffer& operator=(Buffer&& other) noexcept {
			if (this != &other) {
				cleanup();
				device = other.device;
				buffer = other.buffer;
				memory = other.memory;
				ptr = other.ptr;

				other.buffer = nullptr;
				other.memory = nullptr;
				other.ptr = nullptr;
			}
			return *this;
		}
		vk::Buffer buffer = nullptr;
		void* ptr = nullptr;
	protected:
	private:

		vk::Device device = nullptr;
		
		vk::DeviceMemory memory = nullptr;

		void cleanup();
	};
}