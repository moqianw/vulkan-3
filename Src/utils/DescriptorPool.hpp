#pragma once

#include <vulkan/vulkan.hpp>

/*
 * 使用示例：
 *
 * DescriptorSetManager manager;
 * manager.setDevice(device).init();
 *
 * // 方式1：使用便利方法
 * auto sets = manager.allocateGeneral(layouts);
 *
 * // 方式2：使用完整配置
 * DescriptorSetAllocateInfo info;
 * info.setDescriptorSetLayouts(layouts)
 *     .setDescriptorPoolSizeFlags(DescriptorPoolSizeFlagBits::eCompute)
 *     .setDescriptorPoolCreateFlags(vk::DescriptorPoolCreateFlagBits::eUpdateAfterBind);
 * auto sets = manager.allocateDescriptorSet(info);
 */
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
		friend class DescriptorPool_;
		vk::DescriptorPoolCreateInfo createinfo;
		vk::Device device = nullptr;
		
	};
	class DescriptorPool_ {
	public:

		DescriptorPool_() = default;
		DescriptorPool_(const DescriptorPoolCreateInfo& createinfo);
		~DescriptorPool_() = default;
		std::vector<vk::DescriptorSet> allocateDescriptorSets(const std::vector<vk::DescriptorSetLayout>& layouts);
		void reset();
		void free(const std::vector<vk::DescriptorSet>& setsToFree);
		DescriptorPool_(const DescriptorPool_& other) = delete;
		DescriptorPool_& operator=(const DescriptorPool_& other) = delete;

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
	using DescriptorPool = std::shared_ptr<DescriptorPool_>;


	enum class DescriptorPoolSizeFlagBits {
		eGeneral,//��ͨ
		eShadow,//��Ӱ
		eCompute,//����
		eTiny//С��
	};
	class DescriptorSetAllocateInfo {
	public:
		DescriptorSetAllocateInfo& setDescriptorSetLayouts(const std::vector<vk::DescriptorSetLayout>& layouts) {
			this->layouts = layouts;
			return *this;
		}
		DescriptorSetAllocateInfo& setDescriptorPoolCreateFlags(const vk::DescriptorPoolCreateFlags& poolflags) {
			this->poolflags = poolflags;
			return *this;
		}	
		DescriptorSetAllocateInfo& setDescriptorPoolSizeFlags(const DescriptorPoolSizeFlagBits& poolsizetype) {
			this->poolsizetype = poolsizetype;
			return *this;
		}
		std::vector<vk::DescriptorSetLayout> layouts;
		vk::DescriptorPoolCreateFlags poolflags;
		DescriptorPoolSizeFlagBits poolsizetype;
	};
	class DescriptorSetManager {
	public:

		void init();
		void destroy();
		DescriptorSetManager& setDevice(const vk::Device& device);

		// 核心分配接口
		std::vector<vk::DescriptorSet> allocateDescriptorSet(const DescriptorSetAllocateInfo& allocateinfo);

	private:
		DescriptorPool createDescriptorPool(
			const DescriptorPoolSizeFlagBits& poolsizeflag, const vk::DescriptorPoolCreateFlags& flag);


		std::vector<vk::DescriptorPoolSize> generalpoolSizes;
		std::vector<vk::DescriptorPoolSize> shadowpoolSizes;
		std::vector<vk::DescriptorPoolSize> computepoolSizes;
		std::vector<vk::DescriptorPoolSize> tinypoolSizes;

		std::vector<DescriptorPool> contpools;
		std::vector<DescriptorPool> framepools;
		vk::Device device = nullptr;
	};
}