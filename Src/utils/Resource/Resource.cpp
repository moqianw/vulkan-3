#include "Resource.hpp"
namespace RE {
	void ResourceManager::init() {
		allocator.setDevice(device)
			.setPhysicalDevice(physicaldevice);
	}
	void ResourceManager::destroy() {
		buffers.clear();
		images.clear();
		allocator.destroy();
	}
	UT::Buffer ResourceManager::createBuffer(const UT::BufferCreateInfo& createinfo)
	{
		auto buffer = allocator.createBuffer(createinfo);
		buffers.push_back(buffer);
		return buffer;
	}

	UT::Image ResourceManager::createImage(const UT::ImageCreateInfo& createinfo)
	{
		auto image = allocator.createImage(createinfo);
		images.push_back(image);
		return image;
	}
}