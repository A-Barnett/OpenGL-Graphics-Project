#include "Camera.h"
#include "../nclgl/Window.h"
#include <algorithm>

void Camera::UpdateCamera(float dt) {
	pitch -= (Window::GetMouse()->GetRelativePosition().y);
	yaw -= (Window::GetMouse()->GetRelativePosition().x);
	pitch = std::min(pitch, 90.0f);
	pitch = std::max(pitch, -90.0f);
	if (yaw < 0) {
		yaw += 360.0f;
	}
	if (yaw > 360) {
		yaw -= 360.0f;
	}

	Matrix4 rotation = Matrix4::Rotation(yaw, Vector3(0, 1, 0));
	Vector3 forward = rotation * Vector3(0, 0, -1);
	Vector3 right = rotation * Vector3(1, 0, 0);
	float speed = 1000.0f * dt;


	if (Window::GetKeyboard()->KeyDown(KEYBOARD_W)) {
		position += forward * speed;
	}

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_S)) {
		position -= forward * speed;
	}

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_A)) {
		position -= right * speed;
	}

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_D)) {
		position += right * speed;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_SHIFT)) {
		position.y += speed/2;

	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_CONTROL)) {
		position.y -= speed/2;
	}
}

Matrix4 Camera::BuildViewMatrix() {
	return Matrix4::Rotation(-pitch, Vector3(1, 0, 0)) * Matrix4::Rotation(-yaw, Vector3(0, 1, 0)) * Matrix4::Translation(-position);
}

//Vector3 Camera::getRotation() const {
//	Vector3 rotation;
//	rotation.x = cosf(yaw * (3.14159f / 180.0f)) * cosf(pitch * (3.14159f / 180.0f));
//	rotation.y = sinf(pitch * (3.14159f / 180.0f));
//	rotation.z = sinf(yaw * (3.14159f / 180.0f)) * cosf(pitch * (3.14159f / 180.0f));
//	return rotation.Normalised();
//}

Vector3 Camera::getRotation() const {
	Vector3 rotation;
	rotation.x = sinf(yaw * (3.14159f / 180.0f)) * cosf(pitch * (3.14159f / 180.0f));
	rotation.y = sinf(pitch * (3.14159f / 180.0f));
	rotation.z = cosf(yaw * (3.14159f / 180.0f)) * cosf(pitch * (3.14159f / 180.0f));
	return rotation.Normalised();
}

std::pair<float, float> Camera::getRotations() const{
	return { yaw,pitch };
}




