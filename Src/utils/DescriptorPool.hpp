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
		~DescriptorPool() = default;
		std::vector<vk::DescriptorSet> allocateDescriptorSets(const std::vector<vk::DescriptorSetLayout>& layouts);
		void reset();
		void free(const std::vector<vk::DescriptorSet>& setsToFree);
		DescriptorPool(const DescriptorPool& other);
		DescriptorPool& operator=(const DescriptorPool& other);
		DescriptorPool& operator=(std::nullptr_t) {
			descriptorpool = nullptr;
			descriptorsets = {};
		}
		operator bool() {
			return descriptorpool;
		}
		bool operator!() {
			return !descriptorpool;
		}
		void destroy();
	private:
		vk::Device device = nullptr;
		vk::DescriptorPool descriptorpool;
		std::vector<vk::DescriptorSet> descriptorsets;
	};
	enum class DescriptorPoolSizeFlagBits {
		eGeneral,//普通
		eShadow,//阴影
		eCompute,//计算
		eTiny//小型
	};

	class DescriptorSetManager {
	public:
		
		void init();
		void destroy();
		DescriptorSetManager& setDevice(const vk::Device& device);
		std::vector<vk::DescriptorSet> allocateDescriptorSet(const std::vector<vk::DescriptorSetLayout>& layouts);
	private:
		void createDescriptorPool(const DescriptorPoolSizeFlagBits& flag);
		std::vector<vk::DescriptorPoolSize> generalpoolSizes;
		std::vector<vk::DescriptorPoolSize> shadowpoolSizes;
		std::vector<vk::DescriptorPoolSize> computepoolSizes;
		std::vector<vk::DescriptorPoolSize> tinypoolSizes;
		std::vector<DescriptorPool> pools;
		vk::Device device = nullptr;
	};
}