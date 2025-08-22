#pragma once

#include <vulkan/vulkan.hpp>
#include "Shader.hpp"
#include "../utils/DescriptorPool.hpp"
namespace GM {
	class Material {
	public:
		std::shared_ptr<Shader> shader;
		~Material() {
			shader.reset();
		}
	};
	class MaterialInstance {
	public:
		std::shared_ptr<Material> parent;
		~MaterialInstance() {
			parent.reset();
		}
	};
	class RHIMaterial{
	public:
		std::shared_ptr<MaterialInstance> material;
		vk::DescriptorSet descriptorset = nullptr;
		vk::Pipeline pipeline = nullptr;
		vk::PipelineLayout pipelinelayout = nullptr;
		~RHIMaterial() {
			material.reset();
			descriptorset = nullptr;
			pipeline = nullptr;
			pipelinelayout = nullptr;
		}
	};
	class MaterialManager {
	public:
		void begin(const vk::Device& device);
		void destroy();
		MaterialManager() = default;
		~MaterialManager() {
			destroy();
		}

		std::shared_ptr<Material> createMaterial(const std::shared_ptr<Shader>& shader);
		std::shared_ptr<RHIMaterial> createRHIMaterial(const std::shared_ptr<MaterialInstance>& material);

		vk::Pipeline createGraphPipeline(
			const vk::RenderPass& renderpass,
			const vk::PipelineLayout& pipelinelayout,
			const std::vector<vk::PipelineShaderStageCreateInfo>& shaderstages);
		vk::PipelineLayout createPipelineLayout(
			const std::vector<vk::DescriptorSetLayout>& setlayouts,
			const std::vector<vk::PushConstantRange>& pushconstants);
		
		std::vector<vk::DescriptorSet> createDescriptorSet(
			const std::vector<vk::DescriptorSetLayout>& setlayout);
		std::vector<std::pair<vk::PipelineLayout, std::vector<vk::DescriptorSetLayout>>> pipelinelayouts;
	private:
		std::vector<std::shared_ptr<UT::DescriptorPool>> descriptorpools;
		std::vector<std::shared_ptr<Material>> materials;
		std::vector<vk::Pipeline> pipelines;

		vk::Device device = nullptr;
		vk::PipelineCache pipelinecache = nullptr;
	};
}