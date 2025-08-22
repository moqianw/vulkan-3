#include "Image.hpp"

namespace UT {
	Image::Image(ImageCreateInfo& createinfo) {
		device = createinfo.device;
		if (!device) throw std::runtime_error("Image create Error: not set device");

		image = device.createImage(createinfo.imagecreateinfo);
		if (!image) throw std::runtime_error("create image false");

		if (createinfo.mapmemory) {
			auto requirement = device.getImageMemoryRequirements(image);
			vk::MemoryAllocateInfo allocateinfo;
			allocateinfo.setAllocationSize(requirement.size)
				.setMemoryTypeIndex(createinfo.findMemorytypeIndex(createinfo.physicaldevice, requirement.memoryTypeBits, createinfo.memorypropertyflags).value());
			memory = device.allocateMemory(allocateinfo);
			device.bindImageMemory(image, memory, 0);
		}

		if (createinfo.createimageview) {
			createinfo.imageviewcreateinfo.setImage(image);
			imageview = device.createImageView(createinfo.imageviewcreateinfo);
			if(!imageview) throw std::runtime_error("create imageview false");
		}


	}
	Image::~Image() {
		if (!device) return;
		if (imageview) device.destroyImageView(imageview);
		if (image) device.destroyImage(image);
		if (memory) device.freeMemory(memory);
	}
}