#pragma once

#include "Mesh.hpp"
#include "../utils/CommandPool.hpp"
#include "Camera.hpp"
namespace GM {
	class Scene {
	public:
		Scene();
		~Scene();
		void init();
		Scene& setDevice(const vk::Device& device) {
			this->device = device;
			return *this;
		}
		Scene& setPhysicalDevice(const vk::PhysicalDevice& device) {
			this->physicaldevice = device;
			return *this;
		}
		Scene& setQueueFamilyIndex(const uint32_t& index) {
			this->queuefamilyindex = index;
			return *this;
		}
		Scene& setRenderPass(const vk::RenderPass& renderpass) {
			this->renderpass = renderpass;
			return *this;
		}
		Scene& setCommandPool(std::shared_ptr<UT::CommandPool> commandpool) {
			this->commandpool = commandpool;
			return *this;
		}
		Scene& setPresentQueue(const vk::Queue& queue) {
			this->presentqueue = queue;
			return *this;
		}
		std::vector<std::shared_ptr<GM::GameObject>> const getGameObjects() {
			return gameobjects;
		}
		std::vector<GM::Camera> cameras;
		MaterialManager materialmanager;
		void destroy();
	private:
	protected:
		friend class Contaxt;
		vk::Queue presentqueue;
		vk::Device device = nullptr;
		vk::PhysicalDevice physicaldevice = nullptr;
		std::optional<uint32_t> queuefamilyindex;
		vk::RenderPass renderpass = nullptr;



		ShaderManager shadermanager;
		
		std::shared_ptr<UT::CommandPool> commandpool;
		std::vector<std::shared_ptr<GM::GameObject>> gameobjects;

	};
}