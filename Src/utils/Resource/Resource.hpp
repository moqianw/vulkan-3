#pragma once

#include "Allocation.hpp"
#include <vector>
namespace RE {

	class ResourceManager {
	public:
		UT::Buffer createBuffer(const UT::BufferCreateInfo& createinfo);
		UT::Image createImage(const UT::ImageCreateInfo& createinfo);
		void destroyBuffer(const UT::Buffer& buffer);
		void destroyImage(const UT::Image& image);
		ResourceManager& setDevice(const vk::Device& device) {
			this->device = device;
			return *this;
		}
		ResourceManager& setPhysicalDevice(const vk::PhysicalDevice& device) {
			this->physicaldevice = device;
			return *this;
		}


		void init();
		void destroy();
	private:
		vk::Device device = nullptr;
		vk::PhysicalDevice physicaldevice = nullptr;
		Allocator allocator;
		std::vector<UT::Buffer> buffers;
		std::vector<UT::Image> images;
	};
}