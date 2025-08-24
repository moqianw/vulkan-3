#pragma once
#include <vector>

#include "../utils/Vertex.hpp"
#include "../utils/Buffer.hpp"

#include "GameObject.hpp"
#include "Material.hpp"

namespace GM {
	class MeshCreateInfo {
	public:
		MeshCreateInfo() = default;
		MeshCreateInfo& setVerticesCount(const uint32_t& count) {
			this->verticescount = count;
			return *this;
		}
		MeshCreateInfo& setVerticesSize(const uint32_t& size) {
			this->verticessize = size;
			return *this;
		}
		MeshCreateInfo& setIndicesCount(const uint32_t& count) {
			this->indicescount = count;
			return *this;
		}
		MeshCreateInfo& setIndicesSize(const uint32_t& size) {
			this->indicessize = size;
			return *this;
		}
	public:
		std::optional<uint32_t> verticessize;
		std::optional<uint32_t> verticescount;
		std::optional<uint32_t> indicessize;
		std::optional<uint32_t> indicescount;
		
	};
	class MeshLoadInfo {
	public:
		MeshLoadInfo& setCommandBuffer(const vk::CommandBuffer& commandbuffer) {
			this->commandbuffer = commandbuffer;
			return *this;
		}
		MeshLoadInfo& setVectices(const std::vector<UT::Vertex4>& vertices) {
			this->vertices = vertices;
			return *this;
		}
		MeshLoadInfo& setIndices(const std::vector<uint32_t>& indices) {
			this->indices = indices;
			return *this;
		}
		vk::CommandBuffer commandbuffer;
		std::vector<UT::Vertex4> vertices;
		std::vector<uint32_t> indices;
	};
	class Mesh{
	public:
		
		uint64_t count = 0;
		UT::Buffer verticesBuffer;
		UT::Buffer indicesBuffer;
		Mesh() = default;
		Mesh& operator=(const Mesh& other);
		Mesh(const Mesh& other);
		~Mesh() {

		}
	};

	class MeshRender :public Component {
		
	public:
		
		Mesh mesh;
		std::shared_ptr<RHIMaterial> material;
		MeshRender() {
		}

		~MeshRender() {
			material.reset();
		}
	};
}