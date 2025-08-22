#pragma once

#include "../utils/ShaderStage.hpp"
#include <vector>
#include <string>
namespace GM {
	class ShaderCreateInfo {

	public:
		struct ShaderInfo {
			const char* path;
			vk::ShaderStageFlagBits flag;
			const char* entername;
			ShaderInfo& setPath(const char* path) {
				this->path = path;
				return* this;;
			}
			ShaderInfo& setShaderStageFlagBits(const vk::ShaderStageFlagBits& flag) {
				this->flag = flag;
				return *this;
			}
			ShaderInfo& setEnterName(const char* name) {
				this->entername = name;
				return *this;
			}
			ShaderInfo(const char* path, const vk::ShaderStageFlagBits& flag, const char* name)
				:path(path),flag(flag),entername(name)
			{

			}
			ShaderInfo(const ShaderInfo& other) :path(other.path), flag(other.flag), entername(other.entername) {

			}
			ShaderInfo& operator=(const ShaderInfo& other) {
				path = other.path;
				flag = other.flag;
				entername = other.entername;
				return *this;
			}
		};
		ShaderCreateInfo& setDevice(const vk::Device& device) {
			this->device = device;
			return *this;
		}
		ShaderCreateInfo& setShaderInfos(const std::vector<ShaderInfo>& shaderinfos) {
			this->shaderinfos = shaderinfos;
			return *this;
		}
	private:
		friend class Shader;

		vk::Device device = nullptr;
		std::vector<ShaderInfo> shaderinfos;
	};

	class Shader {
	public:
		Shader(const ShaderCreateInfo& createinfo);
		std::vector<vk::PipelineShaderStageCreateInfo> getPipelineShaderStageCreateInfos() {
			std::vector< vk::PipelineShaderStageCreateInfo> infos;
			infos.resize(stages.size());
			for (int i = 0; i < infos.size(); i++) {
				infos[i] = stages[i].first;
			}
			return infos;
		}
		void destroy() {
			if (!device) throw std::runtime_error("Destroy Shader ERROE: not det device");
			for (auto& [first, second] : stages) {
				if(second) device.destroyShaderModule(second);
			}
		}
		~Shader() {
			destroy();
		}
	private:
		std::vector<std::pair<vk::PipelineShaderStageCreateInfo, vk::ShaderModule>> stages;
		vk::Device device = nullptr;
	};
	class ShaderManager {
	public:
		ShaderManager() = default;
		std::shared_ptr<Shader> loadShader(const ShaderCreateInfo& createinfo) {
			auto sh = std::make_shared<Shader>(createinfo);
			shaders.push_back(sh);
			return sh;
		}
		std::vector<std::shared_ptr<Shader>> shaders;

		void destroy() {
			shaders.clear();
		}
	};
}