#pragma once

#include "GameObject/Material.hpp"
#include "GameObject/Mesh.hpp"
#include "utils/CommandPool.hpp"
#include "utils/Image.hpp"
#include "GameObject/Camera.hpp"
namespace CT {
	struct Frame {

		vk::Semaphore imageAvailableSemaphores = nullptr;
		vk::Semaphore renderFinishedSemaphores = nullptr;
		vk::Fence inFlightFences = nullptr;
		vk::CommandBuffer commandbuffer = nullptr;
	};
	class RenderInitInfo {
	public:
	private:
		friend class Render;

	};
	class Render {
	public:
		Render() = default;
		~Render();
		void begin();
		void end();
		void render(const std::vector<std::shared_ptr<GM::GameObject>>& objects);
		void bindMaterial(const std::shared_ptr<GM::RHIMaterial>& material);
		void bindMesh(const GM::Mesh& mesh);
		void init();
		void destroy();
		Frame& currentFrame() {
			return frames[currentframe];
		}
		Render& setDevice(const vk::Device& device);
		Render& setRenderPass(const vk::RenderPass& renderpass);
		Render& setSurfaceFormat(const vk::Format& format);
		Render& setGraphQueue(const vk::Queue& queue);
		Render& setPresentQueue(const vk::Queue& queue);
		Render& setSwapchainExtent(const vk::Extent2D& extent);
		Render& setDepthImage(const UT::Image& depthimage);
		Render& setSwapchainImageViews(const std::vector<vk::ImageView>& imageviews);
		Render& setSwapchain(const vk::SwapchainKHR& swapchain);
		Render& setFrameCount(const uint32_t& count);
		Render& setCommandPool(const std::shared_ptr<UT::CommandPool> pool);
		vk::RenderPass const getRenderPass();
	private:

		vk::Device device = nullptr;
		vk::RenderPass renderpass = nullptr;
		std::optional<vk::Format> surfaceformat;


		vk::Queue grapicesqueue = nullptr;
		vk::Queue presentqueue = nullptr;

		std::optional<vk::Extent2D> swapchainextent;
		UT::Image depthimage;
		std::vector<vk::ImageView> swapchainimageviews;
		vk::SwapchainKHR swapchain = nullptr;
		uint32_t framecount = 0;

	private:

		//camera

		GM::Camera camera;
		std::shared_ptr<UT::Buffer> camerabuffer;
		vk::DescriptorSet cameradescriptorset = nullptr;
		///
		uint32_t imageindex = 0;
		std::shared_ptr<UT::CommandPool> commandpool;
		std::vector<vk::CommandBuffer> commandbuffers;
		void createRenderpass();
		///frame

		std::vector<vk::Framebuffer> framebuffers;
		void createFrameBuffers();
		std::vector<Frame> frames;

		uint32_t currentframe = 0;

		void nextFrame() {
			currentframe = (currentframe + 1) % framecount;
		}
	};
}