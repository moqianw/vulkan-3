#pragma once
#include <vector>

#include "../utils/Vertex.hpp"
#include "../utils/Buffer.hpp"

#include "GameObject.hpp"
#include "Material.hpp"
namespace GM {
	class MeshLoadInfo {
	public:
		MeshLoadInfo() = default;
		MeshLoadInfo& setDevice(const vk::Device& device) {
			this->device = device;
			return *this;
		}
		MeshLoadInfo& ReleaseMeshDataEnable(const bool& is) {
			releasedate = is;
			return *this;
		}
		MeshLoadInfo& setCommandBuffer(vk::CommandBuffer& command) {
			commandbuffer = &command;
			return *this;
		}
		MeshLoadInfo& setQueueFamilyIndices(const uint32_t& index) {
			queuefamilyindices = index;
			return *this;
		}
		MeshLoadInfo& setPhysicalDevice(const vk::PhysicalDevice& dev) {
			physicaldevice = dev;
			return *this;
		}
		~MeshLoadInfo() {
			device = nullptr;
			physicaldevice = nullptr;
			commandbuffer = nullptr;
		}
	protected:
		friend class Mesh;
		std::optional<uint32_t> queuefamilyindices;
		bool releasedate = false;
		vk::Device device = nullptr;
		vk::PhysicalDevice physicaldevice = nullptr;
		vk::CommandBuffer* commandbuffer = nullptr;
	};
	class Mesh{
	public:
		
		std::vector<UT::Vertex4> vertices;
		std::vector<uint32_t> indices;
		std::unique_ptr<UT::Buffer> verticesBuffer;
		std::unique_ptr<UT::Buffer> indicesBuffer;
		void destroyTampData() {
			verticesBuffer_.reset();
			indicesBuffer_.reset();
		}
		void load(MeshLoadInfo& loadinfo);
		Mesh() = default;
		~Mesh() {
			verticesBuffer.reset();
			indicesBuffer.reset();
		}
	protected:
		friend class MeshRender;
		std::unique_ptr<UT::Buffer> verticesBuffer_;
		std::unique_ptr<UT::Buffer> indicesBuffer_;
		

	};

	class MeshRender :public Component {
		
	public:
		
		std::shared_ptr<Mesh> mesh;
		std::shared_ptr<RHIMaterial> material;
		MeshRender() {
		}

		~MeshRender() {
			mesh.reset();
		}
	};
}