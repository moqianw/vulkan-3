#include "Vertex.hpp"

namespace UT {
	std::vector<vk::VertexInputAttributeDescription> Vertex2::getVertexInputAttributeDescription(uint32_t binding)
	{
		std::vector<vk::VertexInputAttributeDescription> descriptions(2);
		descriptions[0]
			.setBinding(binding)
			.setLocation(0)
			.setFormat(vk::Format::eR32G32B32Sfloat)
			.setOffset(offsetof(Vertex2, position));
		descriptions[1]
			.setBinding(binding)
			.setLocation(1)
			.setFormat(vk::Format::eR32G32B32A32Sfloat)
			.setOffset(offsetof(Vertex2, color));
		return descriptions;
	}
	vk::VertexInputBindingDescription Vertex2::getVertexInputBindingDescription(uint32_t binding)
	{
		vk::VertexInputBindingDescription description;
		description.setBinding(binding)
			.setInputRate(vk::VertexInputRate::eVertex)
			.setStride(sizeof(Vertex2));
		return description;
	}
	std::vector<vk::VertexInputAttributeDescription> Vertex3::getVertexInputAttributeDescription(uint32_t binding)
	{
		std::vector<vk::VertexInputAttributeDescription> descriptions(3);
		descriptions[0]
			.setBinding(binding)
			.setLocation(0)
			.setFormat(vk::Format::eR32G32B32Sfloat)
			.setOffset(offsetof(Vertex3, position));
		descriptions[1]
			.setBinding(binding)
			.setLocation(1)
			.setFormat(vk::Format::eR32G32B32A32Sfloat)
			.setOffset(offsetof(Vertex3, color));
		descriptions[2]
			.setBinding(binding)
			.setLocation(2)
			.setFormat(vk::Format::eR32G32Sfloat)
			.setOffset(offsetof(Vertex3, texcoord));
		return descriptions;
	}
	vk::VertexInputBindingDescription Vertex3::getVertexInputBindingDescription(uint32_t binding)
	{
		vk::VertexInputBindingDescription description;
		description.setBinding(binding)
			.setInputRate(vk::VertexInputRate::eVertex)
			.setStride(sizeof(Vertex3));
		return description;
	}
	std::vector<vk::VertexInputAttributeDescription> Vertex4::getVertexInputAttributeDescription(uint32_t binding)
	{
		std::vector<vk::VertexInputAttributeDescription> descriptions(4);
		descriptions[0]
			.setBinding(binding)
			.setLocation(0)
			.setFormat(vk::Format::eR32G32B32Sfloat)
			.setOffset(offsetof(Vertex4, position));
		descriptions[1]
			.setBinding(binding)
			.setLocation(1)
			.setFormat(vk::Format::eR32G32B32A32Sfloat)
			.setOffset(offsetof(Vertex4, color));
		descriptions[2]
			.setBinding(binding)
			.setLocation(2)
			.setFormat(vk::Format::eR32G32Sfloat)
			.setOffset(offsetof(Vertex4, texcoord));
		descriptions[3]
			.setBinding(binding)
			.setLocation(3)
			.setFormat(vk::Format::eR32G32B32Sfloat)
			.setOffset(offsetof(Vertex4, normal));
		return descriptions;
	}
	vk::VertexInputBindingDescription Vertex4::getVertexInputBindingDescription(uint32_t binding)
	{
		vk::VertexInputBindingDescription description;
		description.setBinding(binding)
			.setInputRate(vk::VertexInputRate::eVertex)
			.setStride(sizeof(Vertex4));
		return description;
	}
}