#include "DescriptorPool.hpp"
namespace UT {
	DescriptorPool::DescriptorPool(const DescriptorPool& other) :
		device(other.device),
		descriptorpool(other.descriptorpool),
		descriptorsets(other.descriptorsets)
	{

	}
	DescriptorPool& DescriptorPool::operator=(const DescriptorPool& other) {
		device = other.device;
		descriptorpool = other.descriptorpool;
		descriptorsets = other.descriptorsets;
		return *this;
	}

	DescriptorPool::DescriptorPool(const DescriptorPoolCreateInfo& createinfo)
		: device(createinfo.device)
	{
		if (!device) throw std::runtime_error("Create DescriptorPool ERROR: not set device");
		descriptorpool = device.createDescriptorPool(createinfo.createinfo);
		if(!descriptorpool) throw std::runtime_error("Create DescriptorPool ERROR: create false");
	}
	void DescriptorPool::destroy() {
		if (device && descriptorpool) device.destroyDescriptorPool(descriptorpool);
		descriptorsets = {};
		device = nullptr;
		descriptorpool = nullptr;
	}
	std::vector<vk::DescriptorSet> DescriptorPool::allocateDescriptorSets(const std::vector<vk::DescriptorSetLayout>& layouts) {
		vk::DescriptorSetAllocateInfo allocateinfo;
		allocateinfo.setDescriptorPool(descriptorpool)
			.setDescriptorSetCount(static_cast<uint32_t>(layouts.size()))
			.setSetLayouts(layouts);
		auto sets =  device.allocateDescriptorSets(allocateinfo);
		descriptorsets.insert(descriptorsets.end(), sets.begin(), sets.end());
		return sets;
	}
	void DescriptorPool::reset() {
		device.resetDescriptorPool(descriptorpool);
		descriptorsets.clear();
	}
	void DescriptorPool::free(const std::vector<vk::DescriptorSet>& setsToFree) {
		device.freeDescriptorSets(descriptorpool, setsToFree);
		for (auto& s : setsToFree) {
			descriptorsets.erase(
				std::remove(descriptorsets.begin(), descriptorsets.end(), s),
				descriptorsets.end()
			);
		}
	}
	DescriptorSetManager& DescriptorSetManager::setDevice(const vk::Device& device) {
		this->device = device;
		return *this;
	}
	std::vector<vk::DescriptorSet> DescriptorSetManager::allocateDescriptorSet(const std::vector<vk::DescriptorSetLayout>& layouts)
	{
		
		return std::vector<vk::DescriptorSet>();
	}
	void DescriptorSetManager::createDescriptorPool(const DescriptorPoolSizeFlagBits& flag)
	{
		DescriptorPoolCreateInfo createinfo;
		vk::DescriptorPoolCreateInfo poolcreateinfo;
		poolcreateinfo.setFlags(vk::DescriptorPoolCreateFlagBits::eUpdateAfterBind)
			.setMaxSets(1)
			.setPoolSizes(computepoolSizes);
		createinfo.setDevice(device)
			.setDescriptorPoolCreateInfo(poolcreateinfo);
		DescriptorPool pool(createinfo);
	
	}
	void DescriptorSetManager::init() {
		generalpoolSizes = {
			{vk::DescriptorType::eUniformBuffer,        1000},
			{vk::DescriptorType::eCombinedImageSampler, 1000},
			{vk::DescriptorType::eStorageBuffer,        500},
			{vk::DescriptorType::eStorageImage,         200},
			{vk::DescriptorType::eSampler,              200},
		};
		shadowpoolSizes = {
			{vk::DescriptorType::eUniformBuffer,        200},
			{vk::DescriptorType::eCombinedImageSampler, 200},
		};
		computepoolSizes = {
			{vk::DescriptorType::eStorageBuffer,  500},
			{vk::DescriptorType::eStorageImage,   500},
			{vk::DescriptorType::eSampler,        100},
		};
		tinypoolSizes = {
			{vk::DescriptorType::eUniformBuffer,        50},
			{vk::DescriptorType::eCombinedImageSampler, 50},
		};
		if (!device) throw std::runtime_error("Init DescriptorSetManager ERROR: not set device");
	}
	void DescriptorSetManager::destroy() {
		pools.clear();
	}
}