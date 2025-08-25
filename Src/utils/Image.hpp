#pragma once

#include <vulkan/vulkan.hpp>

#include <optional>


namespace UT {
	class ImageCreateInfo {
	public:
		ImageCreateInfo& setImageCreateInfo(const vk::ImageCreateInfo& createinfo) {
			this->imagecreateinfo = createinfo;
			return *this;
		}
		ImageCreateInfo& setImageViewCreateInfo(const vk::ImageViewCreateInfo& createinfo) {
			this->imageviewcreateinfo = createinfo;
			return *this;
		}
		ImageCreateInfo(){}
		ImageCreateInfo(const ImageCreateInfo& it) = default;
		ImageCreateInfo& operator=(const ImageCreateInfo& it) = default;
		ImageCreateInfo& setCreateImageViewEnable(const bool& is) {
			createimageview = is;
			return *this;
		}
		ImageCreateInfo& setAllocateMemoryEnable(const bool& is) {
			allocatememory = is;
			return *this;
		}
		ImageCreateInfo& setMemoryPropertyFlags(const vk::MemoryPropertyFlags& memorypropertyflags) {
			this->memorypropertyflags = memorypropertyflags;
			return *this;
		}
		ImageCreateInfo& setMapMemoryEnable(const bool& is) {
			mapmemory = is;
			return *this;
		}

	public:

		vk::ImageCreateInfo imagecreateinfo;
		vk::ImageViewCreateInfo imageviewcreateinfo;

		vk::MemoryPropertyFlags memorypropertyflags;
		bool allocatememory = false;
		bool createimageview = false;
		bool mapmemory = false;
	};
	struct Image {
	public:
		Image(const Image& other);
		Image& operator=(const Image& other);
		Image() = default;
		~Image() = default;
		vk::Image image = nullptr;
		vk::ImageView imageview = nullptr;
		vk::DeviceMemory memory = nullptr;
		vk::DeviceSize size = 0;
		vk::DeviceSize offset = 0;
	};
}