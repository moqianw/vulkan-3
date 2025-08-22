#pragma once

#include <vulkan/vulkan.hpp>

#include <optional>


namespace UT {
	class ImageCreateInfo {
	public:
		friend class Image;
		ImageCreateInfo& setDevice(const vk::Device& device) {
			this->device = device;
			return *this;
		}
		ImageCreateInfo& setPhysicalDevice(const vk::PhysicalDevice& device) {
			this->physicaldevice = device;
			return *this;
		}
		ImageCreateInfo& setImageCreateInfo(const vk::ImageCreateInfo& createinfo) {
			this->imagecreateinfo = createinfo;
			return *this;
		}
		ImageCreateInfo& setImageViewCreateInfo(const vk::ImageViewCreateInfo& createinfo) {
			this->imageviewcreateinfo = createinfo;
			return *this;
		}
		ImageCreateInfo(){}
		ImageCreateInfo(const ImageCreateInfo& it) 
		:
			imagecreateinfo(it.imagecreateinfo),
			imageviewcreateinfo(it.imageviewcreateinfo),
			device(it.device),
			physicaldevice(it.physicaldevice),
			memorypropertyflags(it.memorypropertyflags)
		{

		}
		ImageCreateInfo& operator=(const ImageCreateInfo& it) {
			imagecreateinfo = it.imagecreateinfo;
			imageviewcreateinfo = it.imageviewcreateinfo;
			device = it.device;
			physicaldevice = it.physicaldevice;
			memorypropertyflags = it.memorypropertyflags;
		}
		ImageCreateInfo& createImageViewEnable(const bool& is) {
			createimageview = is;
			return *this;
		}
		ImageCreateInfo& mapMemoryEnable(const bool& is) {
			mapmemory = is;
			return *this;
		}
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
		ImageCreateInfo& setMemoryPropertyFlags(const vk::MemoryPropertyFlags& memorypropertyflags) {
			this->memorypropertyflags = memorypropertyflags;
			return *this;
		}
	private:
		vk::Device device = nullptr;
		vk::ImageCreateInfo imagecreateinfo;
		vk::ImageViewCreateInfo imageviewcreateinfo;
		vk::PhysicalDevice physicaldevice = nullptr;
		vk::MemoryPropertyFlags memorypropertyflags;
		bool createimageview = false;
		bool mapmemory = false;
	};
	class Image {
	public:
		vk::Image image = nullptr;
		vk::ImageView imageview = nullptr;
		Image(Image&& other) noexcept
			: device(other.device), image(other.image), memory(other.memory), imageview(other.imageview)
		{
			other.image = nullptr;
			other.memory = nullptr;
			other.imageview = nullptr;
			other.device = nullptr;
		}
		Image(const Image& other) = delete;
		Image& operator=(Image&& other) noexcept {
			if (this != &other) {
				if (device){
					if (imageview) device.destroyImageView(imageview);
					if (image) device.destroyImage(image);
					if (memory) device.freeMemory(memory);
				}
				device = other.device;
				image = other.image;
				memory = other.memory;
				imageview = other.imageview;

				other.image = nullptr;
				other.memory = nullptr;
				other.imageview = nullptr;
				other.device = nullptr;
			}
			return *this;
		}
		Image& operator=(const Image& other) = delete;
		Image(ImageCreateInfo& createinfo);
		~Image();
	protected:
	private:
		vk::DeviceMemory memory = nullptr;
		vk::Device device = nullptr;
	};
}