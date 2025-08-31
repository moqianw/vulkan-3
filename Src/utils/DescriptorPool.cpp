#include "DescriptorPool.hpp"
#include <unordered_set>
#include <algorithm>
namespace UT {

	DescriptorPool_::DescriptorPool_(const DescriptorPoolCreateInfo& createinfo)
		: device(createinfo.device)
	{
		if (!device) throw std::runtime_error("Create DescriptorPool ERROR: not set device");

		try {
			descriptorpool = device.createDescriptorPool(createinfo.createinfo);
			if(!descriptorpool) throw std::runtime_error("Create DescriptorPool ERROR: create false");
		} catch (const vk::SystemError& e) {
			throw std::runtime_error(std::string("Create DescriptorPool ERROR: ") + e.what());
		}
	}
	void DescriptorPool_::destroy() {
		if (device && descriptorpool) device.destroyDescriptorPool(descriptorpool);
		descriptorsets = {};
		device = nullptr;
		descriptorpool = nullptr;
	}
	std::vector<vk::DescriptorSet> DescriptorPool_::allocateDescriptorSets(const std::vector<vk::DescriptorSetLayout>& layouts) {
		vk::DescriptorSetAllocateInfo allocateinfo;
		allocateinfo.setDescriptorPool(descriptorpool)
			.setDescriptorSetCount(static_cast<uint32_t>(layouts.size()))
			.setSetLayouts(layouts);

		auto sets = device.allocateDescriptorSets(allocateinfo);

		descriptorsets.insert(descriptorsets.end(), sets.begin(), sets.end());
		return sets;
	}
	void DescriptorPool_::reset() {
		device.resetDescriptorPool(descriptorpool);
		descriptorsets.clear();
	}
	void DescriptorPool_::free(const std::vector<vk::DescriptorSet>& setsToFree) {
		if (setsToFree.empty()) return;

		device.freeDescriptorSets(descriptorpool, setsToFree);

		// 优化：使用 unordered_set 提高查找效率
		std::unordered_set<vk::DescriptorSet> toRemove(setsToFree.begin(), setsToFree.end());
		descriptorsets.erase(
			std::remove_if(descriptorsets.begin(), descriptorsets.end(),
				[&toRemove](const vk::DescriptorSet& set) {
					return toRemove.find(set) != toRemove.end();
				}),
			descriptorsets.end()
		);
	}
	DescriptorSetManager& DescriptorSetManager::setDevice(const vk::Device& device) {
		this->device = device;
		return *this;
	}
	std::vector<vk::DescriptorSet> DescriptorSetManager::allocateDescriptorSet(
		const DescriptorSetAllocateInfo& allocateinfo)
	{
		if (allocateinfo.layouts.empty()) {
			throw std::runtime_error("DescriptorSetManager::allocateDescriptorSet ERROR: empty layouts");
		}

		for (auto pool : contpools) {
			try {
				return pool->allocateDescriptorSets(allocateinfo.layouts);
			}
			catch (vk::OutOfPoolMemoryError&) {
				continue;
			}
		}

		auto pool = createDescriptorPool(allocateinfo.poolsizetype, allocateinfo.poolflags);

		try {
			return pool->allocateDescriptorSets(allocateinfo.layouts);
		} catch (const std::exception& e) {
			throw std::runtime_error(std::string("DescriptorSetManager::allocateDescriptorSet ERROR: ") + e.what());
		}
	}
	DescriptorPool DescriptorSetManager::createDescriptorPool(const DescriptorPoolSizeFlagBits& poolsizeflag, const vk::DescriptorPoolCreateFlags& flag)
	{
		DescriptorPoolCreateInfo createinfo;
		vk::DescriptorPoolCreateInfo poolcreateinfo;
		
		poolcreateinfo.setFlags(flag);
		switch (poolsizeflag)
		{
		case DescriptorPoolSizeFlagBits::eGeneral:
			poolcreateinfo.setMaxSets(512)
				.setPoolSizes(generalpoolSizes);
			break;
		case DescriptorPoolSizeFlagBits::eShadow:
			poolcreateinfo.setMaxSets(256)
				.setPoolSizes(shadowpoolSizes);
			break;

		case DescriptorPoolSizeFlagBits::eCompute:
			poolcreateinfo.setMaxSets(256)
				.setPoolSizes(computepoolSizes);
			break;
		case DescriptorPoolSizeFlagBits::eTiny:
			poolcreateinfo.setMaxSets(64)
				.setPoolSizes(tinypoolSizes);
			break;
		default:
			throw std::runtime_error("create DescriptorPool ERROR: false PoolSizeFlag");
			break;
		}
		createinfo.setDevice(device)
			.setDescriptorPoolCreateInfo(poolcreateinfo);
		DescriptorPool pool = std::make_shared<DescriptorPool_>(createinfo);
		contpools.push_back(pool);
		return pool;
	}
	void DescriptorSetManager::init() {
		if (!device) throw std::runtime_error("Init DescriptorSetManager ERROR: not set device");

		// 初始化池大小配置
		generalpoolSizes = {
			{vk::DescriptorType::eUniformBuffer,        1024},
			{vk::DescriptorType::eCombinedImageSampler, 1024},
			{vk::DescriptorType::eStorageBuffer,        512},
			{vk::DescriptorType::eStorageImage,         256},
			{vk::DescriptorType::eSampler,              256},
		};
		shadowpoolSizes = {
			{vk::DescriptorType::eUniformBuffer,        256},
			{vk::DescriptorType::eCombinedImageSampler, 256},
		};
		computepoolSizes = {
			{vk::DescriptorType::eStorageBuffer,  512},
			{vk::DescriptorType::eStorageImage,   512},
			{vk::DescriptorType::eSampler,        128},
		};
		tinypoolSizes = {
			{vk::DescriptorType::eUniformBuffer,        64},
			{vk::DescriptorType::eCombinedImageSampler, 64},
		};

		// 预创建常用池
		createDescriptorPool(DescriptorPoolSizeFlagBits::eGeneral,
							vk::DescriptorPoolCreateFlagBits::eUpdateAfterBind);
		createDescriptorPool(DescriptorPoolSizeFlagBits::eTiny,
							vk::DescriptorPoolCreateFlagBits::eUpdateAfterBind);
	}

	void DescriptorSetManager::destroy() {
		for (auto& pool : contpools) {
			if (pool) {
				pool->destroy();
			}
		}
		contpools.clear();
		for (auto& pool : framepools) {
			if (pool) {
				pool->destroy();
			}
		}
		framepools.clear();
	}
}