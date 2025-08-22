#include "Render.hpp"

namespace CT {

	Render::~Render()
	{
		destroy();
	}
	void Render::begin() {
		auto& frame = currentFrame();
		auto result = device.waitForFences(frame.inFlightFences, VK_TRUE, std::numeric_limits<uint64_t>::max());
		if (result != vk::Result::eSuccess) throw std::runtime_error("wait............");

		device.resetFences(frame.inFlightFences);

		// 先拿图像（使用本帧的 imageAvailable）
		auto acquire = device.acquireNextImageKHR(
			swapchain,
			std::numeric_limits<uint64_t>::max(),
			frame.imageAvailableSemaphores,    // wait by GPU before drawing
			nullptr
		);
		if (acquire.result != vk::Result::eSuccess && acquire.result != vk::Result::eSuboptimalKHR)
			throw std::runtime_error("Failed to acquire swapchain image");
		imageindex = acquire.value;
		// 先等上一帧结束，再reset


		// 两个 ClearValue：0->color, 1->depth
		std::array<vk::ClearValue, 2> clearValues{};
		clearValues[0].setColor(vk::ClearColorValue(std::array<float, 4>{1.f, 1.f, 1.f, 1.f}));
		clearValues[1].setDepthStencil(vk::ClearDepthStencilValue(1.0f, 0)); // 深度=1, 模板=0

		// 开始录制

		vk::CommandBufferBeginInfo begininfo;
		begininfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

		vk::RenderPassBeginInfo rpBegin;
		rpBegin.setRenderPass(renderpass)
			.setFramebuffer(framebuffers[imageindex])
			.setRenderArea(vk::Rect2D({ 0,0 }, { swapchainextent->width, swapchainextent->height }))
			.setClearValues(clearValues);  // 传入两个 clear
		auto& commandbuffer = frame.commandbuffer;
		commandbuffer.begin(begininfo);
		commandbuffer.beginRenderPass(rpBegin, vk::SubpassContents::eInline);
		commandbuffer.setViewport(0, { vk::Viewport(0, 0, static_cast<float>(swapchainextent->width), static_cast<float>(swapchainextent->height), 0.0f, 1.0f) });
		commandbuffer.setScissor(0, { vk::Rect2D({ 0,0 }, { static_cast<uint32_t>(swapchainextent->width), static_cast<uint32_t>(swapchainextent->height) }) });
	}
	void Render::end() {
		
		auto& frame = currentFrame();
		auto& commandbuffer = frame.commandbuffer;
		commandbuffer.endRenderPass();
		commandbuffer.end();

		// 提交（等待 imageAvailable，信号 renderFinished）
		vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		vk::SubmitInfo submitInfo;
		submitInfo
			.setWaitSemaphores(frame.imageAvailableSemaphores)
			.setWaitDstStageMask(waitStage)
			.setCommandBuffers(commandbuffer)
			.setSignalSemaphores(frame.renderFinishedSemaphores);

		grapicesqueue.submit(submitInfo, frame.inFlightFences);

		// 呈现（等待 renderFinished）
		vk::PresentInfoKHR presentInfo;
		presentInfo
			.setWaitSemaphores(frame.renderFinishedSemaphores)
			.setSwapchains(swapchain)
			.setImageIndices(imageindex);

		auto pres = presentqueue.presentKHR(presentInfo);
		if (pres == vk::Result::eErrorOutOfDateKHR || pres == vk::Result::eSuboptimalKHR) {
			// 这里按你的架构触发重建 swapchain（窗口大小变化或失效）
			// recreateSwapchain();
		}
		else if (pres != vk::Result::eSuccess) {
			throw std::runtime_error("Failed to present image.");
		}

		nextFrame();
	}
	void Render::render(const std::vector<std::shared_ptr<GM::GameObject>>& objects)
	{
		auto& frame = currentFrame();
		auto& commandbuffer = frame.commandbuffer;
		for (auto& obj : objects) {
			auto renderer = obj->getComponent<GM::MeshRender>();
			if (!renderer) continue;

			auto rhimaterial = renderer->material;
			bindMaterial(rhimaterial);
			bindMesh(renderer->mesh);

			auto model = obj->transform.trans;
			obj->transform.trans = glm::rotate(model, glm::radians(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			commandbuffer.pushConstants(rhimaterial->pipelinelayout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(glm::mat4), &model);
		}
		
		
		// 更新 mvp


	}
	void Render::bindMaterial(const std::shared_ptr<GM::RHIMaterial>& material) {
		auto& cmd = currentFrame().commandbuffer;
		cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, material->pipeline);
		//cmd.bindDescriptorSets(
		//	vk::PipelineBindPoint::eGraphics,
		//	material->pipelinelayout,
		//	0,
		//	material->descriptorset,
		//	{}
		//);
		
	}

	void Render::bindMesh(const std::shared_ptr<GM::Mesh>& mesh) {
		auto& cmd = currentFrame().commandbuffer;
		vk::Buffer vertexBuffers[] = { mesh->verticesBuffer->buffer };
		vk::DeviceSize offsets[] = { 0 };
		cmd.bindVertexBuffers(0, 1, vertexBuffers, offsets);
		cmd.bindIndexBuffer(mesh->indicesBuffer->buffer, 0, vk::IndexType::eUint32);
		
	}


	void Render::init()
	{
		if (!device) throw std::runtime_error("Init Render ERROE: not set device");
		if(!commandpool) throw std::runtime_error("Init Render ERROE: not set commandpool");

		frames.resize(framecount);
		vk::SemaphoreCreateInfo createinfo;
		vk::FenceCreateInfo fencecreateinfo;
		fencecreateinfo.setFlags(vk::FenceCreateFlagBits::eSignaled);
		for (auto& frame : frames) {
			frame.imageAvailableSemaphores = device.createSemaphore(createinfo);
			frame.renderFinishedSemaphores = device.createSemaphore(createinfo);
			frame.inFlightFences = device.createFence(fencecreateinfo);
			frame.commandbuffer = commandpool->allocateCommandBuffer();
		}
		createRenderpass();
		createFrameBuffers();
	}


	void Render::createRenderpass(){
		vk::RenderPassCreateInfo createinfo;
		vk::AttachmentDescription attachment;
		vk::SubpassDependency dependency;
		vk::SubpassDescription subpass;
		attachment.setFinalLayout(vk::ImageLayout::ePresentSrcKHR)
			.setFormat(surfaceformat.value())
			.setInitialLayout(vk::ImageLayout::eUndefined)
			.setLoadOp(vk::AttachmentLoadOp::eClear)
			.setSamples(vk::SampleCountFlagBits::e1)
			.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
			.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
			.setStoreOp(vk::AttachmentStoreOp::eStore);
		dependency
			.setSrcSubpass(VK_SUBPASS_EXTERNAL)
			.setDstSubpass(0)
			.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
			.setSrcAccessMask(vk::AccessFlagBits::eNone)
			.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
			.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);

		subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
			.setColorAttachments(
				vk::AttachmentReference()
				.setAttachment(0)
				.setLayout(vk::ImageLayout::eColorAttachmentOptimal));

		//深度附件

		vk::AttachmentDescription depthattachment;
		depthattachment.setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
			.setFormat(vk::Format::eD32Sfloat)
			.setInitialLayout(vk::ImageLayout::eUndefined)
			.setLoadOp(vk::AttachmentLoadOp::eClear)
			.setSamples(vk::SampleCountFlagBits::e1)
			.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
			.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
			.setStoreOp(vk::AttachmentStoreOp::eStore);
		vk::AttachmentReference depthAttachmentRef;
		depthAttachmentRef.setAttachment(1)  // 假设 color attachment 是 0
			.setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

		subpass
			.setPDepthStencilAttachment(&depthAttachmentRef);
		std::vector<vk::AttachmentDescription> attachments = { attachment,depthattachment };
		createinfo
			.setAttachmentCount(2)
			.setAttachments(attachments)
			.setDependencyCount(1)
			.setDependencies({ dependency })
			.setSubpassCount(1)
			.setSubpasses({ subpass });
		renderpass = device.createRenderPass(createinfo);
		if (!renderpass) throw std::runtime_error("create renderpass false");
	}

	void Render::createFrameBuffers()
	{
		if(framecount == 0) throw std::runtime_error("Init Render ERROE: not framecount error");
		if(!swapchainextent) throw std::runtime_error("Init Render ERROE: not set swapchainextent");
		if(swapchainimageviews.empty()) throw std::runtime_error("Init Render ERROE: not set swapchainimageviews");
		framebuffers.resize(framecount);
		vk::FramebufferCreateInfo createinfo;
		createinfo
			.setHeight(swapchainextent->height)
			.setLayers(1)
			.setRenderPass(renderpass)
			.setWidth(swapchainextent->width);
		for (size_t i = 0; i < framebuffers.size(); i++) {
			std::vector<vk::ImageView> imageviews = { swapchainimageviews[i],depthimage->imageview };
			createinfo.setAttachments(imageviews); // 单元素 vector
			framebuffers[i] = device.createFramebuffer(createinfo);
		}
	}

	void Render::destroy() {
		if (!device) return;
		depthimage.reset();
		for (auto& framebuffer : framebuffers) {
			if (framebuffer) device.destroyFramebuffer(framebuffer);
		}
		commandpool.reset();
		if (renderpass) device.destroyRenderPass(renderpass);

		for (auto& frame : frames) {
			device.destroySemaphore(frame.imageAvailableSemaphores);
			device.destroySemaphore(frame.renderFinishedSemaphores);
			device.destroyFence(frame.inFlightFences);
		}
		device = nullptr;
	}



	Render& Render::setDevice(const vk::Device& device) {
		this->device = device;
		return *this;
	}
	Render& Render::setDepthImage(const std::shared_ptr<UT::Image> image) {
		this->depthimage = image;
		return *this;
	}
	Render& Render::setSwapchainImageViews(const std::vector<vk::ImageView>& imageviews)
	{
		this->swapchainimageviews = imageviews;
		return *this;
	}
	Render& Render::setSwapchain(const vk::SwapchainKHR& swapchain)
	{
		this->swapchain = swapchain;

		return *this;
	}
	Render& Render::setFrameCount(const uint32_t& count)
	{
		this->framecount = count;
		return *this;
	}
	Render& Render::setSurfaceFormat(const vk::Format& format)
	{
		this->surfaceformat = format;
		return *this;
	}

	Render& Render::setGraphQueue(const vk::Queue& queue)
	{
		this->grapicesqueue = queue;
		return *this;
	}
	Render& Render::setPresentQueue(const vk::Queue& queue) {
		this->presentqueue = queue;
		return *this;
	}
	Render& Render::setSwapchainExtent(const vk::Extent2D& extent)
	{
		this->swapchainextent = extent;
		return *this;
		
	}
	Render& Render::setCommandPool(const std::shared_ptr<UT::CommandPool> pool) {
		this->commandpool = pool;
		return *this;
	}
	vk::RenderPass const Render::getRenderPass()
	{
		if (!renderpass) throw std::runtime_error("get Renderpass error: renderpass not create");
		return renderpass;
	}
}