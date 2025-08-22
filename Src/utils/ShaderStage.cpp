#include "ShaderStage.hpp"

namespace UT {
	ShaderStagesCreater::ShaderStagesCreater()
	{
	}
	std::optional<std::pair<vk::PipelineShaderStageCreateInfo, vk::ShaderModule>>
		ShaderStagesCreater::createShaderStageCreateInfo(
			const vk::Device& device,
			const std::vector<char>& data,
			const vk::ShaderStageFlagBits& flag,
			const char* entername
		)
	{
		vk::PipelineShaderStageCreateInfo createinfo;
		vk::ShaderModuleCreateInfo modulecreateinfo;
		modulecreateinfo.setCodeSize(static_cast<uint32_t>(data.size()))
			.setPCode(reinterpret_cast<const uint32_t*>(data.data()));
		auto module = device.createShaderModule(modulecreateinfo);
		createinfo.setStage(flag)
			.setModule(module)
			.setPName(entername);
		return std::pair<vk::PipelineShaderStageCreateInfo, vk::ShaderModule>(createinfo, module);
	}
	ShaderStagesCreater::~ShaderStagesCreater()
	{
	}
}