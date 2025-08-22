#pragma once

#include <vulkan/vulkan.hpp>

namespace GM {
	class PipelineManager {
	public:
		PipelineManager() = default;
		~PipelineManager();

		PipelineManager& setDevice(const vk::Device& device);
		vk::Pipeline createGraphPipeline(
			const vk::RenderPass& renderpass,
			const vk::PipelineLayout& pipelinelayout,
			const std::vector<vk::PipelineShaderStageCreateInfo>& shaderstages);
		vk::PipelineLayout createPipelineLayout(
			const std::vector<vk::DescriptorSetLayout>& setlayouts,
			const std::vector<vk::PushConstantRange>& pushconstants);
		vk::Pipeline getPipeline(const uint32_t& index) {
			if (index >= pipelines.size()) return nullptr;
			return pipelines[index];
		}
		vk::PipelineLayout getPipelineLayout(const uint32_t& index) {
			if (index >= pipelinelayouts.size()) return nullptr;
			return pipelinelayouts[index];
		}
		void destroy();
		void init();
	private:
		vk::Device device = nullptr;
		std::vector<vk::Pipeline> pipelines;
		vk::PipelineCache pipelinecache = nullptr;
		std::vector<vk::PipelineLayout> pipelinelayouts;
	};
}