#pragma once

#include "Allocation.hpp"
#include "../../GameObject/Mesh.hpp"

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
		ResourceManager& setQueueFamilyIndex(const uint32_t& index) {
			this->queuefamilyindex = index;
			return *this;
		}
		GM::Mesh createMesh(const GM::MeshCreateInfo& createinfo);
		void loadData(const GM::Mesh& mesh, const GM::MeshLoadInfo& loadinfo);
		void init();
		void destroy();
	private:
		vk::Device device = nullptr;
		vk::PhysicalDevice physicaldevice = nullptr;
		std::optional<uint32_t> queuefamilyindex;
		Allocator allocator;

		std::vector<UT::Buffer> buffers;
		std::vector<UT::Image> images;
		std::vector<GM::Mesh> meshs;
	};
}