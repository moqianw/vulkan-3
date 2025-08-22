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
		void bindMesh(const std::shared_ptr<GM::Mesh>& mesh);
		void init();
		void initCamera();
		void destroy();
	private:
		friend class Contaxt;
		vk::Device device = nullptr;
		vk::RenderPass renderpass = nullptr;
		std::optional<vk::Format> surfaceformat;

		std::optional<uint32_t> grapicefamilyindex;
		std::optional<uint32_t> presentfamilyindex;
		vk::Queue grapicesqueue = nullptr;
		vk::Queue presentqueue = nullptr;

		std::optional<vk::Extent2D> swapchainextent;
		std::shared_ptr<UT::Image> depthimage;
		std::vector<vk::ImageView> swapchainimageviews;
		vk::SwapchainKHR swapchain = nullptr;
		uint32_t framecount = 0;
		Frame& currentFrame() {
			return frames[currentframe];
		}
	private:

		//camera

		GM::Camera camera;
		std::shared_ptr<UT::Buffer> camerabuffer;
		vk::DescriptorSet cameradescriptorset = nullptr;
		///
		uint32_t imageindex = 0;
		std::vector<std::unique_ptr<UT::CommandPool>> commandpools;
		std::vector<vk::CommandBuffer> commandbuffers;
		void createRenderpass();
		void createCommandPool();
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