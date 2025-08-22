#include "Camera.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
namespace GM {
	Camera::Camera()
	{
		float fov = glm::radians(45.0f);       // 视野角度
		float aspect = 1080 / (float)960; // 屏幕宽高比
		float nearPlane = 0.1f;
		float farPlane = 100.0f;
		position = glm::vec3(0.0f, 0.0f, -5.0f);
		target = glm::vec3(0.0f, 0.0f, 1.0f);
		mat.proj = glm::perspective(fov, aspect, nearPlane, farPlane);
		mat.proj[1][1] *= -1; // Vulkan 修正
		mat.view = glm::lookAt(
				position,        // 摄像机位置 (eye)
				target,     // 看向的点 (center)
				up // 上方向 (up)
			);

	}
	Camera::~Camera()
	{
		uniformbuffer.reset();
		descriptorset = nullptr;
	}
	void Camera::updata()
	{
	}
}