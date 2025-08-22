#include "Buffer.hpp"
namespace UT {

	Buffer::Buffer(const BufferCreateInfo& createinfo)
	{
		this->device = createinfo.device;
		if (!device) throw std::runtime_error("create buffer not set device");
		buffer = device.createBuffer(createinfo.buffercreateinfo);


		auto memoryrequirements = device.getBufferMemoryRequirements(buffer);
		if (createinfo.allocateMemory) {

			vk::MemoryAllocateInfo allocateinfo;
			auto index = createinfo.findMemorytypeIndex(createinfo.physicaldevice
				,memoryrequirements.memoryTypeBits,createinfo.memorypropertyflags).value();
			allocateinfo.setAllocationSize(memoryrequirements.size)
				.setMemoryTypeIndex(index);
			memory = device.allocateMemory(allocateinfo);
			device.bindBufferMemory(buffer, memory, 0);
		}

		if (createinfo.mapMemory && memory) {
			ptr = device.mapMemory(memory, 0, memoryrequirements.size);
		}
	}
	Buffer::~Buffer() {
		cleanup();
	}
	void Buffer::cleanup() {
		if (!device) return;

		if (ptr && memory) device.unmapMemory(memory);
		if (memory) device.freeMemory(memory);
		if (buffer) device.destroyBuffer(buffer);
	}
}