#pragma once

#include <glm/glm.hpp>
#include "../utils/Buffer.hpp"
namespace GM {
	class Camera {
	public:
		struct mat{
			glm::mat4 view;
			glm::mat4 proj;
			
		}mat;

		glm::vec3 position;
		glm::vec3 target;
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
		Camera();
		~Camera();
		void updata();
		UT::Buffer uniformbuffer;
		vk::DescriptorSet descriptorset = nullptr;
		vk::PipelineLayout pipelinelayout = nullptr;

	};
}