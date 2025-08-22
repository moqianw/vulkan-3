#pragma once

#include <vulkan/vulkan.hpp>
namespace UT {
	class DescriptorPoolCreateInfo {
	public:
		DescriptorPoolCreateInfo& setDevice(const vk::Device& device) {
			this->device = device;
			return *this;
		}
		DescriptorPoolCreateInfo& setDescriptorPoolCreateInfo(const vk::DescriptorPoolCreateInfo& createinfo) {
			this->createinfo = createinfo;
			return *this;
		}
	private:
		friend class DescriptorPool;
		vk::DescriptorPoolCreateInfo createinfo;
		vk::Device device = nullptr;
	};
	class DescriptorPool {
	public:
		DescriptorPool(const DescriptorPoolCreateInfo& createinfo);
		~DescriptorPool();
		std::vector<vk::DescriptorSet> allocateDescriptorSets(const std::vector<vk::DescriptorSetLayout>& layouts);
		void reset();
		void free(const std::vector<vk::DescriptorSet>& setsToFree);
	private:
		vk::Device device = nullptr;
		vk::DescriptorPool descriptorpool;
		std::vector<vk::DescriptorSet> descriptorsets;
	};
}