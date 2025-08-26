#pragma once

#include <vulkan/vulkan.hpp>
namespace UT {
	class DescriptorPoolCreateInfo {
	public:
		DescriptorPoolCreateInfo() = default;
		DescriptorPoolCreateInfo& setDevice(const vk::Device& device) {
			this->device = device;
			return *this;
		}
		DescriptorPoolCreateInfo& setDescriptorPoolCreateInfo(const vk::DescriptorPoolCreateInfo& createinfo) {
			this->createinfo = createinfo;
			return *this;
		}
		DescriptorPoolCreateInfo(const DescriptorPoolCreateInfo& other) :
			device(other.device),
			createinfo(other.createinfo)
		{

		}
		DescriptorPoolCreateInfo& operator=(const DescriptorPoolCreateInfo& other) {
			this->device = other.device;
			this->createinfo = other.createinfo;
			return *this;
		}
	private:
		friend class DescriptorPool;
		vk::DescriptorPoolCreateInfo createinfo;
		vk::Device device = nullptr;
	};
	class DescriptorPool {
	public:
		DescriptorPool() = default;
		DescriptorPool(const DescriptorPoolCreateInfo& createinfo);
		~DescriptorPool();
		std::vector<vk::DescriptorSet> allocateDescriptorSets(const std::vector<vk::DescriptorSetLayout>& layouts);
		void reset();
		void free(const std::vector<vk::DescriptorSet>& setsToFree);
		DescriptorPool(const DescriptorPool& other);
		DescriptorPool& operator=(const DescriptorPool& other);
	private:
		vk::Device device = nullptr;
		vk::DescriptorPool descriptorpool;
		std::vector<vk::DescriptorSet> descriptorsets;
	};


	class DescriptorPoolManage {
	public:


	private:
		std::vector<DescriptorPool> pools;
		vk::Device device = nullptr;
	};
}