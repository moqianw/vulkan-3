#include "Resource.hpp"
namespace RE {
	GM::Mesh ResourceManager::createMesh(const GM::MeshCreateInfo& createinfo)
	{
		if (!device) throw std::runtime_error("Create Mesh ERROR: not set device");
		if (!physicaldevice) throw std::runtime_error("Create Mesh ERROR: not set physicaldevice");
		if (!queuefamilyindex) throw std::runtime_error("Create Mesh ERROR: not set queuefamilyindices");
		if (
			!createinfo.verticescount.has_value() ||
			!createinfo.verticessize.has_value() ||
			!createinfo.indicescount.has_value() || 
			!createinfo.indicessize.has_value()) throw std::runtime_error("Create Mesh ERROR: not set meshdata");

		GM::Mesh mesh;
		mesh.count = createinfo.indicescount.value();
		vk::BufferCreateInfo vdbuffercreateinfo;
		vdbuffercreateinfo.setQueueFamilyIndices({ queuefamilyindex.value() })
			.setUsage(vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer)
			.setSharingMode(vk::SharingMode::eExclusive)
			.setSize(vk::DeviceSize(createinfo.verticessize.value()));
		UT::BufferCreateInfo vdcreateinfo;

		vdcreateinfo
			.setBufferCreateInfo(vdbuffercreateinfo)
			.setMemoryPropertyFlags(vk::MemoryPropertyFlagBits::eDeviceLocal)
			.setAllocateMemoryEnable(true)
			.setMapMemoryEnable(false);
		mesh.verticesBuffer = createBuffer(vdcreateinfo);


		vk::BufferCreateInfo idbuffercreateinfo;
		idbuffercreateinfo.setQueueFamilyIndices({ queuefamilyindex.value() })
			.setUsage(vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer)
			.setSharingMode(vk::SharingMode::eExclusive)
			.setSize(vk::DeviceSize(createinfo.indicessize.value()));
		UT::BufferCreateInfo idcreateinfo;

		idcreateinfo
			.setBufferCreateInfo(idbuffercreateinfo)
			.setMemoryPropertyFlags(vk::MemoryPropertyFlagBits::eDeviceLocal)
			.setAllocateMemoryEnable(true)
			.setMapMemoryEnable(false);
		mesh.indicesBuffer = createBuffer(idcreateinfo);
		return mesh;
	}
	void ResourceManager::loadData(const GM::Mesh& mesh, const GM::MeshLoadInfo& loadinfo)
	{
		if (!loadinfo.commandbuffer) throw std::runtime_error("LoadMeshData ERROR: not set commandbuffer");
		if (loadinfo.vertices.empty() || loadinfo.indices.empty()) throw std::runtime_error("LoadMeshData ERROR: not set meshdata");
		
		vk::BufferCreateInfo vbuffercreateinfo;
		vbuffercreateinfo.setQueueFamilyIndices({ queuefamilyindex.value() })
			.setUsage(vk::BufferUsageFlagBits::eTransferSrc)
			.setSharingMode(vk::SharingMode::eExclusive)
			.setSize(vk::DeviceSize(sizeof(UT::Vertex4) * loadinfo.vertices.size()));
		UT::BufferCreateInfo vhcreateinfo;

		vhcreateinfo
			.setMemoryPropertyFlags(vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent)
			.setAllocateMemoryEnable(true)
			.setMapMemoryEnable(true)
			.setBufferCreateInfo(vbuffercreateinfo);
		auto vhostbuffer = createBuffer(vhcreateinfo);
		auto size = sizeof(UT::Vertex4) * loadinfo.vertices.size();
		memcpy(vhostbuffer.ptr, loadinfo.vertices.data(), sizeof(UT::Vertex4) * loadinfo.vertices.size());

		vk::BufferCopy vbuffercopy;
		vbuffercopy.setDstOffset(0)
			.setSrcOffset(0)
			.setSize(sizeof(UT::Vertex4) * loadinfo.vertices.size());

		loadinfo.commandbuffer.copyBuffer(vhostbuffer.buffer, mesh.verticesBuffer.buffer, vbuffercopy);
		//indices
		
		//host
		vk::BufferCreateInfo ibuffercreateinfo;
		ibuffercreateinfo.setQueueFamilyIndices({ queuefamilyindex.value() })
			.setUsage(vk::BufferUsageFlagBits::eTransferSrc)
			.setSharingMode(vk::SharingMode::eExclusive)
			.setSize(vk::DeviceSize(sizeof(uint32_t) * loadinfo.indices.size()));
		UT::BufferCreateInfo ihcreateinfo;

		ihcreateinfo
			.setBufferCreateInfo(ibuffercreateinfo)
			.setMemoryPropertyFlags(vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent)
			.setAllocateMemoryEnable(true)
			.setMapMemoryEnable(true);
		auto ihostbuffer = createBuffer(ihcreateinfo);

		//device
		memcpy(ihostbuffer.ptr, loadinfo.indices.data(), sizeof(uint32_t) * loadinfo.indices.size());

		vk::BufferCopy ibuffercopy;
		ibuffercopy.setDstOffset(0)
			.setSrcOffset(0)
			.setSize(sizeof(uint32_t) * loadinfo.indices.size());
		loadinfo.commandbuffer.copyBuffer(ihostbuffer.buffer, mesh.indicesBuffer.buffer, ibuffercopy);
		
	}
	void ResourceManager::init() {
		allocator.setDevice(device)
			.setPhysicalDevice(physicaldevice);
	}
	void ResourceManager::destroy() {
		for (auto& buffer : buffers) {
			if(buffer.buffer)
				device.destroyBuffer(buffer.buffer);
		}
		buffers.clear();

		for (auto& image : images) {
			if (image.imageview) device.destroyImageView(image.imageview);
			if (image.image) device.destroyImage(image.image);
		}
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


	void ResourceManager::destroyBuffer(const UT::Buffer& buffer) {
		if (!device) throw std::runtime_error("destroy buffer ERROR: not set device");
		for (auto buff = buffers.begin(); buff != buffers.end();buff++) {
			if (buff->buffer == buffer.buffer) {
				buffers.erase(buff);
				if(buffer.buffer) device.destroyBuffer(buffer.buffer);
				return;
			}
		}
	}
	void ResourceManager::destroyImage(const UT::Image& image) {
		if(!device) throw std::runtime_error("destroy buffer ERROR: not set device");
		for (auto img = images.begin(); img != images.end(); img++) {
			if (img->image == image.image) {
				images.erase(img);
				if (image.imageview) device.destroyImageView(image.imageview);
				if(image.image) device.destroyImage(image.image);
				return;
			}
		}
	}
}