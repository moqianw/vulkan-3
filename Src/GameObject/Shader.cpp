#include "Shader.hpp"
#include "../utils/File.hpp"
namespace GM {
	Shader::Shader(const ShaderCreateInfo& createinfo):device(createinfo.device)
	{
		if (!device) throw std::runtime_error("Create Shader ERROR: not set device");
		
		for (auto& info : createinfo.shaderinfos) {
			auto tamp = UT::ShaderStagesCreater::createShaderStageCreateInfo(
				device, UT::FileReader::read(info.path), info.flag, info.entername);
			if (!tamp.has_value()) throw std::runtime_error("Create Shader ERROR: create shader stage false");
			stages.push_back(tamp.value());
		}
	}
}