#include "Camera.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
namespace GM {
	Camera::Camera()
	{
		float fov = glm::radians(45.0f);       // ��Ұ�Ƕ�
		float aspect = 1080 / (float)960; // ��Ļ��߱�
		float nearPlane = 0.1f;
		float farPlane = 100.0f;
		position = glm::vec3(0.0f, 0.0f, -5.0f);
		target = glm::vec3(0.0f, 0.0f, 1.0f);
		mat.proj = glm::perspective(fov, aspect, nearPlane, farPlane);
		mat.proj[1][1] *= -1; // Vulkan ����
		mat.view = glm::lookAt(
				position,        // �����λ�� (eye)
				target,     // ����ĵ� (center)
				up // �Ϸ��� (up)
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