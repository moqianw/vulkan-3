#include "Contaxt.hpp"
#include <SDL3/SDL_vulkan.h>
#include <iostream>
#include <vector>
#include <string>
#include "utils/ShaderStage.hpp"
#include "utils/Vertex.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "utils/File.hpp"

namespace CT {
	Contaxt::Contaxt(const ContaxtCreateInfo& createinfo)
	{
		create(createinfo);
	}
	void Contaxt::Render()
	{
		bool running = true;
		SDL_Event event;
		initWorld();
		while (running) {
			while (SDL_PollEvent(&event)) {
				if (event.type == SDL_EVENT_QUIT)
					running = false;

			}
			SDL_Delay(16);

			draw();
		}
	}
	Contaxt::~Contaxt()
	{
		destroy();
	}
	void Contaxt::draw()
	{

		render.begin();
		render.render(scene.getGameObjects());
		auto& cmd = render.currentFrame().commandbuffer;
		cmd.bindDescriptorSets(
			vk::PipelineBindPoint::eGraphics,
			scene.materialmanager.pipelinelayouts[0].first,
			0,
			scene.cameras[0].descriptorset,
			{}
		);
		auto size = scene.getGameObjects()[0]->getComponent<GM::MeshRender>()->mesh->indices.size();
		cmd.drawIndexed(size, 1, 0, 0, 0);
		render.end();
		
	}

	void Contaxt::initWorld()
	{

	}
	
	Contaxt& Contaxt::create(const ContaxtCreateInfo& createinfo)
	{
		try {
			createWindow(createinfo.w,createinfo.h,createinfo.windowname);
			createInstance();
			createSurface();
			pickPhysicalDevice();
			createDevice();
			createSwapChain();
			createImageView();
			createDepthImageView();
			createCommandPool();
			setRenderData();
			createScene();
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;
		}

		return *this;
	}
	Contaxt& Contaxt::createInstance()
	{

		// Vulkan application info
		vk::ApplicationInfo appInfo;
		appInfo.setApiVersion(VK_API_VERSION_1_3)
			.setApplicationVersion(1)
			.setEngineVersion(1)
			.setPApplicationName("MyVulkanApp")
			.setPEngineName("NoEngine");

		// Get required extensions from SDL3
		uint32_t extension_count = 0;
		if (!SDL_Vulkan_GetInstanceExtensions(&extension_count)) {
			throw std::runtime_error("Could not get Vulkan extension count from SDL: " + std::string(SDL_GetError()));
		}
		std::vector<const char*> extensions;

		auto extension = SDL_Vulkan_GetInstanceExtensions(&extension_count);
		for (uint32_t i = 0; i < extension_count; i++) {
			extensions.push_back(extension[i]);
		}
		std::vector<const char*> layers = { "VK_LAYER_KHRONOS_validation" };

		// Create Vulkan instance
		vk::InstanceCreateInfo instanceCreateInfo;
		instanceCreateInfo.setPApplicationInfo(&appInfo)
			.setEnabledExtensionCount(static_cast<uint32_t>(extensions.size()))
			.setPEnabledExtensionNames(extensions)
			.setEnabledLayerCount(static_cast<uint32_t>(layers.size()))
			.setPEnabledLayerNames(layers);

		instance = vk::createInstance(instanceCreateInfo);

		return *this;
	}
	Contaxt& Contaxt::createWindow(const int& w_,const int& h_ ,const std::string& name)
	{
		this->w = w_;
		this->h = h_;
		if (!SDL_Init(SDL_INIT_VIDEO))
			throw std::runtime_error("Could not initialize SDL: " + std::string(SDL_GetError()));
		window = std::shared_ptr<SDL_Window>(
			SDL_CreateWindow(name.c_str(), w, h, SDL_WINDOW_VULKAN)
			, SDL_DestroyWindow);
		if (!window) {
			SDL_Quit();
			throw std::runtime_error("Could not create SDL window: " + std::string(SDL_GetError()));
		}

		return *this;
	}
	Contaxt& Contaxt::createSurface()
	{

		// Create surface
		VkSurfaceKHR c_surface;
		if (!SDL_Vulkan_CreateSurface(window.get(), static_cast<VkInstance>(instance), nullptr,&c_surface)) {
			throw std::runtime_error("Failed to create Vulkan surface: " + std::string(SDL_GetError()));
		}
		surface = vk::SurfaceKHR(c_surface);

		if (!surface) throw std::runtime_error("create surface false");
		return *this;
	}
	Contaxt& Contaxt::pickPhysicalDevice()
	{
		auto devices = instance.enumeratePhysicalDevices();
		for (auto& t : devices) {
			std::cout << t.getProperties().deviceName << std::endl;
		}
		physicaldevice = devices.front();
		return *this;
	}
	Contaxt::QueueFamilyIndices Contaxt::findQueueFamilies(vk::PhysicalDevice device, vk::SurfaceKHR surface) {
		QueueFamilyIndices indices;
		auto queueFamilies = device.getQueueFamilyProperties();
		for (uint32_t i = 0; i < queueFamilies.size(); i++) {
			if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics)
				indices.graphicsFamily = i;
			if (device.getSurfaceSupportKHR(i, surface))
				indices.presentFamily = i;
			if (indices.isComplete()) break;
		}
		return indices;
	}
	Contaxt& Contaxt::createDevice()
	{

		queuefamilyindices = findQueueFamilies(physicaldevice, surface);
		//auto extensions = physicaldevice.enumerateDeviceExtensionProperties();
		//for (auto& ex : extensions) {
		//	std::cout << ex.extensionName << std::endl;
		//}
		
		std::vector<const char*> deviceExtensions = {
			"VK_KHR_swapchain"
		};
		if (queuefamilyindices->graphicsFamily.has_value()) {
			vk::DeviceQueueCreateInfo queuecreateinfo;
			float priority = 1.0f;
			queuecreateinfo.setQueueFamilyIndex(queuefamilyindices->graphicsFamily.value())
				.setPQueuePriorities(&priority)
				.setQueueCount(1);

			vk::DeviceCreateInfo createinfo;
			createinfo.setFlags(vk::DeviceCreateFlags())
				.setQueueCreateInfoCount(1)
				.setQueueCreateInfos({ queuecreateinfo })
				.setEnabledExtensionCount(static_cast<uint32_t>(deviceExtensions.size()))
				.setPEnabledExtensionNames(deviceExtensions);
			device = physicaldevice.createDevice(createinfo);

			graphqueue = device.getQueue(queuefamilyindices->graphicsFamily.value(), 0);
		}
		if (queuefamilyindices->presentFamily.has_value()) {
			if (queuefamilyindices->graphicsFamily.value() != queuefamilyindices->presentFamily.value()) {
				vk::DeviceQueueCreateInfo queuecreateinfo;
				float priority = 1.0f;
				queuecreateinfo.setQueueFamilyIndex(queuefamilyindices->presentFamily.value())
					.setPQueuePriorities(&priority)
					.setQueueCount(1);

				vk::DeviceCreateInfo createinfo;
				createinfo.setFlags(vk::DeviceCreateFlags())
					.setQueueCreateInfoCount(1)
					.setQueueCreateInfos({ queuecreateinfo })
					.setEnabledExtensionCount(static_cast<uint32_t>(deviceExtensions.size()))
					.setPEnabledExtensionNames(deviceExtensions);
				device = physicaldevice.createDevice(createinfo);

				presentqueue = device.getQueue(queuefamilyindices->presentFamily.value(), 0);
			}
			else presentqueue = graphqueue;
		}
		return *this;
	}
	Contaxt& Contaxt::createSwapChain()
	{
		auto surfacecaps =  physicaldevice.getSurfaceCapabilitiesKHR(surface);
		
		auto surfaceformats = physicaldevice.getSurfaceFormatsKHR(surface);

		auto surfacepresentmodes =  physicaldevice.getSurfacePresentModesKHR(surface);
		//选择合适的format

		for (auto& format : surfaceformats) {
			if (format.format == vk::Format::eR8G8B8A8Srgb
				&& format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
			{
				surfaceformat = format;
				break;
			}
		}
		if (!surfaceformat) throw std::runtime_error("Cannot find fit format");
		//选择合适的presentmode
		std::optional<vk::PresentModeKHR> choicepresentmode;
		for (auto& presentmode : surfacepresentmodes) {
			if (presentmode == vk::PresentModeKHR::eMailbox) {
				choicepresentmode = presentmode;
				break;
			}
		}
		if(!choicepresentmode) throw std::runtime_error("Cannot find fit presentmode");
		

		vk::SwapchainCreateInfoKHR createinfo;
		swapchainextent = surfacecaps.currentExtent;
		createinfo.setSurface(surface)
			.setClipped(true)
			.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
			.setImageArrayLayers(1)
			.setImageFormat(surfaceformat->format)
			.setMinImageCount(
				std::clamp<uint32_t>(
					2,static_cast<uint32_t>(surfacecaps.minImageCount)
					, static_cast<uint32_t>(surfacecaps.maxImageCount))
			)
			.setImageColorSpace(surfaceformat->colorSpace)
			.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
			.setPresentMode(choicepresentmode.value())
			.setImageExtent(swapchainextent.value());
		if (queuefamilyindices->graphicsFamily.value() == queuefamilyindices->presentFamily.value()) {
			createinfo
				.setQueueFamilyIndexCount(1)
				.setQueueFamilyIndices({ queuefamilyindices->graphicsFamily.value() })
				.setImageSharingMode(vk::SharingMode::eExclusive);
		}
		else {
			uint32_t indexs[] = { queuefamilyindices->graphicsFamily.value() ,queuefamilyindices->presentFamily.value() };
			createinfo
				.setQueueFamilyIndexCount(2)
				.setQueueFamilyIndices(indexs)
				.setImageSharingMode(vk::SharingMode::eConcurrent);
		}

		swapchain = device.createSwapchainKHR(createinfo);
		return *this;
	}
	Contaxt& Contaxt::createImageView()
	{
		swapchainImages = device.getSwapchainImagesKHR(swapchain);
		if (swapchainImages.empty()) throw std::runtime_error("Cannot get swapchain images");

		swapchainImageViews.resize(swapchainImages.size());
		vk::ImageViewCreateInfo createinfo;
		vk::ImageSubresourceRange subrange;
		subrange.setAspectMask(vk::ImageAspectFlagBits::eColor)
			.setBaseArrayLayer(0)
			.setBaseMipLevel(0)
			.setLayerCount(1)
			.setLevelCount(1);
		vk::ComponentMapping componentmapping;
		componentmapping.setA(vk::ComponentSwizzle::eA)
			.setB(vk::ComponentSwizzle::eB)
			.setG(vk::ComponentSwizzle::eG)
			.setR(vk::ComponentSwizzle::eR);
		createinfo.setFormat(surfaceformat->format)
			.setSubresourceRange(subrange)
			.setComponents(componentmapping)
			.setViewType(vk::ImageViewType::e2D)
			;


		for (uint32_t i = 0; i < swapchainImageViews.size(); i++) {
			createinfo.setImage(swapchainImages[i]);
			swapchainImageViews[i] = device.createImageView(createinfo);
		}
		return *this;
	}

	Contaxt& Contaxt::setRenderData()
	{
		render.device = device;
		render.depthimage = depthimage;
		render.swapchainextent = swapchainextent;
		render.swapchainimageviews = swapchainImageViews;
		render.surfaceformat = surfaceformat->format;
		render.framecount = static_cast<uint32_t>(swapchainImages.size());
		render.grapicefamilyindex = queuefamilyindices->graphicsFamily.value();
		render.presentfamilyindex = queuefamilyindices->presentFamily.value();
		render.grapicesqueue = graphqueue;
		render.presentqueue = presentqueue;
		render.swapchain = swapchain;
		render.init();
		return *this;
	}

	Contaxt& Contaxt::createDepthImageView()
	{
		UT::ImageCreateInfo createinfo;
		vk::ImageCreateInfo imagecreateinfo;
		imagecreateinfo.setExtent({ swapchainextent->width,swapchainextent->height, 1 })
			.setImageType(vk::ImageType::e2D)
			.setUsage(vk::ImageUsageFlagBits::eDepthStencilAttachment)
			.setMipLevels(1)
			.setArrayLayers(1)
			.setFormat(vk::Format::eD32Sfloat)
			.setTiling(vk::ImageTiling::eOptimal)
			.setSamples(vk::SampleCountFlagBits::e1)
			.setSharingMode(vk::SharingMode::eExclusive);

		

		vk::ImageViewCreateInfo imageviewcreateinfo;
		imageviewcreateinfo.setFormat(vk::Format::eD32Sfloat)
			.setViewType(vk::ImageViewType::e2D)
			.setSubresourceRange({
			vk::ImageAspectFlagBits::eDepth,
			0, 1, 0, 1
				});
		createinfo.setDevice(device)
			.setImageCreateInfo(imagecreateinfo)
			.setImageViewCreateInfo(imageviewcreateinfo)
			.setMemoryPropertyFlags(vk::MemoryPropertyFlagBits::eDeviceLocal)
			.setPhysicalDevice(physicaldevice)
			.createImageViewEnable(true)
			.mapMemoryEnable(true);
		depthimage = std::make_shared<UT::Image>(createinfo);
		if (!depthimage) throw std::runtime_error("create depthimage false");
		
		return *this;
	}
	Contaxt& Contaxt::createCommandPool()
	{
		UT::CommandPoolCreateInfo createinfo;
		vk::CommandPoolCreateInfo poolcreateinfo;
		poolcreateinfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
			.setQueueFamilyIndex(queuefamilyindices->graphicsFamily.value());
		createinfo.setDevice(device)
			.setCommandPoolCreateInfo(poolcreateinfo);
		commandpool = std::make_shared<UT::CommandPool>(createinfo);
		
		return *this;
	}

	Contaxt& Contaxt::createScene()
	{
		
		scene.setDevice(device)
			.setPhysicalDevice(physicaldevice)
			.setQueueFamilyIndex(queuefamilyindices->graphicsFamily.value())
			.setRenderPass(render.renderpass)
			.setCommandPool(commandpool)
			.setPresentQueue(presentqueue);
		scene.init();
		return *this;
	}



	Contaxt& Contaxt::destroy()
	{
		try {
			if (device) {
				device.waitIdle();
	
				scene.destroy();
				if (commandpool) {
					commandpool.reset();
				}

				for (auto& view : this->swapchainImageViews) {
					device.destroyImageView(view);
				}

				render.destroy();
				depthimage.reset();

				if (swapchain)
					device.destroySwapchainKHR(swapchain);
				
				device.destroy();
			}
			
			physicaldevice = nullptr;

			if(instance && surface)
				instance.destroySurfaceKHR(surface);
			if(instance)
				instance.destroy();
			if (window)
			{
				window.reset();
				SDL_Quit();
			}
			
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;
		}
		return *this;
	}
}