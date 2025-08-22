#pragma once
#include <vulkan/vulkan.hpp>
#include <string>
#include <optional>

namespace UT {
	class ShaderStagesCreater {
	public:
		ShaderStagesCreater();
		static std::optional<std::pair<vk::PipelineShaderStageCreateInfo,vk::ShaderModule>> createShaderStageCreateInfo(
			const vk::Device& device, const std::vector<char>& data,
			const vk::ShaderStageFlagBits& flag,
			const char* entername);
		~ShaderStagesCreater();
	private:
	protected:
	};
}