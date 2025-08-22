#include "Pipeline.hpp"
#include "../utils/Vertex.hpp"
namespace GM {
	PipelineManager& PipelineManager::setDevice(const vk::Device& device) {
		this->device = device;
		return *this;
	}
	void PipelineManager::init() {
		if (!device) throw std::runtime_error("Init PipelineManager ERROR: not set device");
		vk::PipelineCacheCreateInfo cacheinfo;	
		pipelinecache = device.createPipelineCache(cacheinfo);
	}
	PipelineManager::~PipelineManager()
	{
		destroy();
	}
	void PipelineManager::destroy() {
		if (!device) return;
		for (auto& pipeline : pipelines) {
			if (pipeline) device.destroyPipeline(pipeline);
		}
		pipelines.clear();
		for (auto& layout : pipelinelayouts) {
			if (layout) device.destroyPipelineLayout(layout);
		}
		pipelinelayouts.clear();
		if (pipelinecache) device.destroyPipelineCache(pipelinecache);
		pipelinecache = nullptr;
	}

	vk::Pipeline PipelineManager::createGraphPipeline(
		const vk::RenderPass& renderpass,
		const vk::PipelineLayout& pipelinelayout,
		const std::vector<vk::PipelineShaderStageCreateInfo>& shaderstages) {

		vk::GraphicsPipelineCreateInfo createinfo;
		//着色器


		//顶点输入
		vk::PipelineVertexInputStateCreateInfo vertexinputstate;
		std::vector<vk::VertexInputBindingDescription> vertexbindings = { UT::Vertex4::getVertexInputBindingDescription(0) };
		auto vertexattribute = UT::Vertex4::getVertexInputAttributeDescription(0);
		vertexinputstate.setVertexBindingDescriptions(vertexbindings)
			.setVertexAttributeDescriptions(vertexattribute);

		///顶点聚集
		vk::PipelineInputAssemblyStateCreateInfo inputassembly;

		inputassembly
			.setPrimitiveRestartEnable(false)
			.setTopology(vk::PrimitiveTopology::eTriangleList);

		//viewport
		vk::PipelineViewportStateCreateInfo viewport;

		viewport.setViewports({ })
			.setScissors({ })
			.setViewportCount(1)
			.setScissorCount(1);

		//光栅化
		vk::PipelineRasterizationStateCreateInfo rasterizationstate;
		rasterizationstate.setCullMode(vk::CullModeFlagBits::eBack)
			.setRasterizerDiscardEnable(false)
			.setFrontFace(vk::FrontFace::eCounterClockwise)
			.setLineWidth(1)
			.setPolygonMode(vk::PolygonMode::eFill);

		//多重采样
		vk::PipelineMultisampleStateCreateInfo multisamplestate;
		multisamplestate.setSampleShadingEnable(false)
			.setRasterizationSamples(vk::SampleCountFlagBits::e1);

		//深度测试
		vk::PipelineDepthStencilStateCreateInfo depthstencilstate;
		depthstencilstate.setDepthTestEnable(true)
			.setDepthWriteEnable(true)
			.setDepthCompareOp(vk::CompareOp::eLess)
			.setDepthBoundsTestEnable(false)
			.setStencilTestEnable(false);

		//颜色混合

		vk::PipelineColorBlendStateCreateInfo colorblendstate;
		vk::PipelineColorBlendAttachmentState attachment;
		attachment.setBlendEnable(true)
			.setAlphaBlendOp(vk::BlendOp::eAdd)
			.setColorBlendOp(vk::BlendOp::eAdd)
			.setColorWriteMask(vk::ColorComponentFlagBits::eA
				| vk::ColorComponentFlagBits::eR
				| vk::ColorComponentFlagBits::eG
				| vk::ColorComponentFlagBits::eB)
			.setDstAlphaBlendFactor(vk::BlendFactor::eZero)
			.setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
			.setDstColorBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha)
			.setSrcColorBlendFactor(vk::BlendFactor::eSrcAlpha);
		colorblendstate.setAttachments({ attachment })
			.setLogicOpEnable(false);

		//dynamicstate
		vk::PipelineDynamicStateCreateInfo dynamicstate;
		std::vector<vk::DynamicState> dynamicstates = { vk::DynamicState::eViewport,vk::DynamicState::eScissor };
		dynamicstate.setDynamicStates(dynamicstates);

		if (!pipelinelayout) throw std::runtime_error("not create pipelinelayout");
		if (!renderpass) throw std::runtime_error("not create renderpass");
		createinfo.setPVertexInputState(&vertexinputstate)
			.setStages(shaderstages)
			.setPInputAssemblyState(&inputassembly)
			.setPViewportState(&viewport)
			.setPDynamicState(&dynamicstate)
			.setPRasterizationState(&rasterizationstate)
			.setPMultisampleState(&multisamplestate)
			.setPDepthStencilState(&depthstencilstate)
			.setPColorBlendState(&colorblendstate)
			.setLayout(pipelinelayout) // pipelinelayout
			.setRenderPass(renderpass); // renderpass

		auto result = device.createGraphicsPipeline(pipelinecache, createinfo);
		if (result.result != vk::Result::eSuccess) throw std::runtime_error("create grapics pipeline false");
		if (!result.value) {
			throw std::runtime_error("Pipeline is null, check shader interface, render pass, or vertex layout");
		}
		auto graphicepipeline = result.value;
		pipelines.push_back(graphicepipeline);
		return graphicepipeline;
	}
	vk::PipelineLayout PipelineManager::createPipelineLayout(
		const std::vector<vk::DescriptorSetLayout>& setlayouts,
		const std::vector<vk::PushConstantRange>& pushconstants
	) {

		vk::PipelineLayoutCreateInfo createinfo;
		createinfo.setSetLayouts(setlayouts)
			.setPushConstantRanges(pushconstants);
		auto pipelinelayout = device.createPipelineLayout(createinfo);
		if (!pipelinelayout) throw std::runtime_error("create pipelinelayout false");
		pipelinelayouts.push_back(pipelinelayout);
		return pipelinelayout;
	}
}