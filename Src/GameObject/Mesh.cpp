#include "Mesh.hpp"

namespace GM {

	void Mesh::load(MeshLoadInfo& loadinfo)
	{
		if (!loadinfo.device) throw std::runtime_error("not set device");
		if (!loadinfo.physicaldevice) throw std::runtime_error("not set physicaldevice");
		if (!loadinfo.queuefamilyindices) throw std::runtime_error("not set queuefamilyindices");
		if (!loadinfo.commandbuffer) throw std::runtime_error("not set commandbuffer");
		if (vertices.empty() || indices.empty()) throw std::runtime_error("not set meshdata");
		vk::BufferCreateInfo vbuffercreateinfo;
		vbuffercreateinfo.setQueueFamilyIndices({ loadinfo.queuefamilyindices.value()})
			.setUsage(vk::BufferUsageFlagBits::eTransferSrc)
			.setSharingMode(vk::SharingMode::eExclusive)
			.setSize(vk::DeviceSize(sizeof(UT::Vertex4) * vertices.size()));
		UT::BufferCreateInfo vhcreateinfo;

		vhcreateinfo.setDevice(loadinfo.device)
			.setBufferCreateInfo(vbuffercreateinfo)
			.setMemoryPropertyFlags(vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent)
			.allocateMemoryEnable(true)
			.mapMemoryEnable(true)
			.setPhysicalDevice(loadinfo.physicaldevice);
		verticesBuffer_ = std::make_unique<UT::Buffer>(vhcreateinfo);

		memcpy(verticesBuffer_->ptr, vertices.data(), sizeof(UT::Vertex4) * vertices.size());

		//device
		if (!verticesBuffer) {
			vk::BufferCreateInfo vdbuffercreateinfo;
			vdbuffercreateinfo.setQueueFamilyIndices({ loadinfo.queuefamilyindices.value() })
				.setUsage(vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer)
				.setSharingMode(vk::SharingMode::eExclusive)
				.setSize(vk::DeviceSize(sizeof(UT::Vertex4) * vertices.size()));
			UT::BufferCreateInfo vdcreateinfo;

			vdcreateinfo.setDevice(loadinfo.device)
				.setBufferCreateInfo(vdbuffercreateinfo)
				.setMemoryPropertyFlags(vk::MemoryPropertyFlagBits::eDeviceLocal)
				.allocateMemoryEnable(true)
				.mapMemoryEnable(false)
				.setPhysicalDevice(loadinfo.physicaldevice);
			verticesBuffer = std::make_unique<UT::Buffer>(vdcreateinfo);
			
		}
		vk::BufferCopy vbuffercopy;
		vbuffercopy.setDstOffset(0)
			.setSrcOffset(0)
			.setSize(sizeof(UT::Vertex4) * vertices.size());

		loadinfo.commandbuffer->copyBuffer(verticesBuffer_->buffer, verticesBuffer->buffer, vbuffercopy);
		//indices
		//host
		vk::BufferCreateInfo ibuffercreateinfo;
		ibuffercreateinfo.setQueueFamilyIndices({ loadinfo.queuefamilyindices.value() })
			.setUsage(vk::BufferUsageFlagBits::eTransferSrc)
			.setSharingMode(vk::SharingMode::eExclusive)
			.setSize(vk::DeviceSize(sizeof(uint32_t) * indices.size()));
		UT::BufferCreateInfo ihcreateinfo;

		ihcreateinfo.setDevice(loadinfo.device)
			.setBufferCreateInfo(ibuffercreateinfo)
			.setMemoryPropertyFlags(vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent)
			.allocateMemoryEnable(true)
			.mapMemoryEnable(true)
			.setPhysicalDevice(loadinfo.physicaldevice);
		indicesBuffer_ = std::make_unique<UT::Buffer>(ihcreateinfo);

		//device
		memcpy(indicesBuffer_->ptr, indices.data(), sizeof(uint32_t) * indices.size());

		if (!indicesBuffer) {
			vk::BufferCreateInfo buffercreateinfo;
			buffercreateinfo.setQueueFamilyIndices({ loadinfo.queuefamilyindices.value() })
				.setUsage(vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer)
				.setSharingMode(vk::SharingMode::eExclusive)
				.setSize(vk::DeviceSize(sizeof(uint32_t) * indices.size()));
			UT::BufferCreateInfo idcreateinfo;

			idcreateinfo.setDevice(loadinfo.device)
				.setBufferCreateInfo(buffercreateinfo)
				.setMemoryPropertyFlags(vk::MemoryPropertyFlagBits::eDeviceLocal)
				.allocateMemoryEnable(true)
				.mapMemoryEnable(false)
				.setPhysicalDevice(loadinfo.physicaldevice);
			indicesBuffer = std::make_unique<UT::Buffer>(idcreateinfo);
		}
		
		vk::BufferCopy ibuffercopy;
		ibuffercopy.setDstOffset(0)
			.setSrcOffset(0)
			.setSize(sizeof(uint32_t) * indices.size());
		loadinfo.commandbuffer->copyBuffer(indicesBuffer_->buffer, indicesBuffer->buffer, ibuffercopy);
		if (loadinfo.releasedate) {
			vertices = {};
			indices = {};
		}
	}
}