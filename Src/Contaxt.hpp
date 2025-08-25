#pragma once
#include <vulkan/vulkan.hpp>
#include <SDL3/SDL.h>
#include <optional>

#include "utils/CommandPool.hpp"

#include "utils/Vertex.hpp"

#include "utils/Buffer.hpp"
#include "utils/Image.hpp"

#include "Render.hpp"
#include "GameObject/Scene.hpp"
namespace CT {
	class ContaxtCreateInfo {
		friend class Contaxt;
	private:
		int w = 0, h = 0;
		std::string windowname;
	public:
		ContaxtCreateInfo():windowname(""){}
		ContaxtCreateInfo& setWidgh(const int& w) {
			this->w = w;
			return *this;
		}
		ContaxtCreateInfo& setHeght(const int& h) {
			this->h = h;
			return *this;
		}
		ContaxtCreateInfo& setWindowName(const std::string& name) {
			windowname = name;
			return *this;
		}
	};
	
	class Contaxt {
	public:
		struct QueueFamilyIndices {
			std::optional<uint32_t> graphicsFamily;
			std::optional<uint32_t> presentFamily;

			bool isComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
		};
		Contaxt(const ContaxtCreateInfo& createinfo);
		void Render();
		~Contaxt();
	private:
		void draw();
		void initWorld();

		Contaxt& create(const ContaxtCreateInfo& createinfo);
		Contaxt& createInstance();
		Contaxt& createWindow(const int& w_,const int& h_, const std::string& name);
		Contaxt& createSurface();
		Contaxt& pickPhysicalDevice();

		QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice device, vk::SurfaceKHR surface);
		Contaxt& createDevice();
		Contaxt& createSwapChain();
		Contaxt& createImageView();

		Contaxt& setRenderData();
		Contaxt& createCommandPool();
		Contaxt& initScene();
		Contaxt& destroy();
	protected:

		CT::Render render;
		GM::Scene scene;
		std::shared_ptr<SDL_Window> window;
		int32_t w = 1080, h = 960;

		vk::Instance instance = nullptr;
		vk::SurfaceKHR surface = nullptr;

		vk::PhysicalDevice physicaldevice = nullptr;

		vk::Device device = nullptr;

		std::optional<QueueFamilyIndices> queuefamilyindices;
		vk::Queue graphqueue = nullptr;
		vk::Queue presentqueue = nullptr;
		std::optional<vk::SurfaceFormatKHR> surfaceformat;
		vk::SwapchainKHR swapchain = nullptr;
		std::optional<vk::Extent2D> swapchainextent;
		std::vector<vk::Image> swapchainImages;
		std::vector<vk::ImageView> swapchainImageViews;

		std::shared_ptr<UT::CommandPool> commandpool;

	};
}