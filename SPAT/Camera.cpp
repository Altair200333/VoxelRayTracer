#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "ImGui/imgui.h"

Camera::Camera(Vector3 _front, Vector3 _right, Vector3 _up, float _fov): front(_front), right(_right), up(_up), fov(_fov)
{
}

Vector3 Camera::getRayDirectionAtPixel(float x, float y, size_t width, size_t height) const
{
	const float closeHeight = 2.0f * tanf(fov * 0.5f * glm::pi<float>() / 180.0f);
	const float scale = closeHeight / height;

	const float dx = x - static_cast<float>(width) * 0.5f;
	const float dy = y - static_cast<float>(height) * 0.5f;

	return (front + right * dx * scale + up * dy * scale).normalized();
}

void Camera::updateCameraVectors()
{
	glm::vec3 forward;
	forward.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	forward.z = sin(glm::radians(pitch));
	forward.y = -sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(forward);

	right = glm::normalize(glm::cross(forward, {0, 0, 1}));
	up = glm::normalize(glm::cross(right.toGlm(), front.toGlm()));
}

void Camera::onGuiDraw()
{
	ImGui::DragFloat("FOV", &fov, 0.1f, 5, 160);
}
