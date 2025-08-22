#include "DescriptorPool.hpp"
namespace UT {
	DescriptorPool::DescriptorPool(const DescriptorPoolCreateInfo& createinfo)
		: device(createinfo.device)
	{
		if (!device) throw std::runtime_error("Create DescriptorPool ERROR: not set device");
		descriptorpool = device.createDescriptorPool(createinfo.createinfo);
		if(!descriptorpool) throw std::runtime_error("Create DescriptorPool ERROR: create false");
	}
	DescriptorPool::~DescriptorPool() {
		device.destroyDescriptorPool(descriptorpool);
		descriptorsets = {};
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
}