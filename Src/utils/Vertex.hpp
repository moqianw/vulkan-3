#pragma once
#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>
namespace UT {
	class Vertex2 {
	public:
		glm::vec3 position;
		glm::vec4 color;
		static std::vector<vk::VertexInputAttributeDescription> getVertexInputAttributeDescription(uint32_t binding = 0);
		static vk::VertexInputBindingDescription getVertexInputBindingDescription(uint32_t binding = 0);
	};
	class Vertex3 {
	public:
		glm::vec3 position;
		glm::vec3 color;
		glm::vec2 texcoord;
		static std::vector<vk::VertexInputAttributeDescription> getVertexInputAttributeDescription(uint32_t binding = 0);
		static vk::VertexInputBindingDescription getVertexInputBindingDescription(uint32_t binding = 0);
	};
	class Vertex4 {
	public:
		glm::vec3 position;
		glm::vec3 color;
		glm::vec2 texcoord;
		glm::vec3 normal;
		static std::vector<vk::VertexInputAttributeDescription> getVertexInputAttributeDescription(uint32_t binding = 0);
		static vk::VertexInputBindingDescription getVertexInputBindingDescription(uint32_t binding = 0);
	};
}