#include "Material.hpp"
#include "../utils/Vertex.hpp"
#include "../utils/File.hpp"
namespace GM {
	void MaterialManager::begin(const vk::Device& device) {
		this->device = device;
		vk::PipelineCacheCreateInfo createinfo;
		createinfo.setInitialDataSize(0);
	}
	void MaterialManager::destroy() {
		descriptorpools.clear();
		for (auto& mat : materials) {
			mat.reset(); // shared_ptr 自动释放
		}
		materials.clear();

		// 清理 pipeline layouts + descriptor set layouts
		for (auto& setlayout: descriptorsetlayouts) {

			if (setlayout) {
				device.destroyDescriptorSetLayout(setlayout);
			}
		}
		descriptorsetlayouts.clear();

	}
    std::shared_ptr<Material> MaterialManager::createMaterial(const std::shared_ptr<Shader>& shader)
    {
        auto mat = std::make_shared<Material>();
        mat->shader = shader;
        materials.push_back(mat);
        return mat;
    }
    std::shared_ptr<RHIMaterial> MaterialManager::createRHIMaterial(const std::shared_ptr<MaterialInstance>& material)
    {
		auto rhimaterial = std::make_shared<RHIMaterial>();
		rhimaterial->material = material;
		return rhimaterial;
    }

	
	std::vector<vk::DescriptorSet> MaterialManager::createDescriptorSet(const std::vector<vk::DescriptorSetLayout>& setlayout)
	{
		if (descriptorpools.empty()) {
			std::vector<vk::DescriptorPoolSize> poolsize;
			poolsize.push_back(vk::DescriptorPoolSize().setDescriptorCount(100).setType(vk::DescriptorType::eUniformBuffer));
			vk::DescriptorPoolCreateInfo poolinfo;
			poolinfo.setFlags(vk::DescriptorPoolCreateFlagBits::eUpdateAfterBind)
				.setMaxSets(100)
				.setPoolSizes(poolsize);
			UT::DescriptorPoolCreateInfo createinfo;
			createinfo.setDevice(device)
				.setDescriptorPoolCreateInfo(poolinfo);
			descriptorpools.push_back(std::make_shared<UT::DescriptorPool>(createinfo));
		}
		return descriptorpools[0]->allocateDescriptorSets(setlayout);
	}
	void MaterialManager::addDescriptorSetLayouts(const std::vector<vk::DescriptorSetLayout>& setlayouts)
	{
		descriptorsetlayouts.insert(descriptorsetlayouts.end(), setlayouts.begin(), setlayouts.end());
	}
}