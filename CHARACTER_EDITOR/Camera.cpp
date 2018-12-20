#include "Camera.h"

Camera::Camera() {
	position = glm::vec3(0.0f);
	lookVec = glm::vec3(0.0f, 0.0f, -1.0f);
	upVec = glm::vec3(0.0f, 1.0f, 0.0f);
	lookAngle = 0.0f;
	speed = 2.0f;
	turnSensitivity = 10.0f;

	view = glm::lookAt(position, position + lookVec, upVec);
}

Camera::Camera(glm::vec3 position_, glm::vec3 lookVec_, glm::vec3 upVec_):position(position_),lookVec(lookVec_),upVec(upVec_) {
	lookAngle = 0.0f;
	view = glm::lookAt(position, position + lookVec, upVec);
	speed = 2.0f;
	turnSensitivity = 10.0f;
}

void Camera::SetLookAngle(float lookAngle_) {
	lookAngle = lookAngle_;
	
	if (lookAngle < 0.0f) lookAngle = 360 + lookAngle;
	if (lookAngle > 360.0f) lookAngle = lookAngle - 360.0f;

	float x = -sin(glm::radians(lookAngle));
	float z = -cos(glm::radians(lookAngle));
	lookVec = glm::vec3(x, 0.0f, z);

	view = glm::lookAt(position, position + lookVec, upVec);
}

void Camera::Process() {
	if (InputManager::GetKeyDown(0x57)) Forward();
	if (InputManager::GetKeyDown(0x53)) Backward();
	if (InputManager::GetKeyDown(0x41)) Left();
	if (InputManager::GetKeyDown(0x44)) Right();
	if (InputManager::GetKeyDown(VK_SPACE)) Up();
	if (InputManager::GetKeyDown(VK_CONTROL)) Down();
	if (InputManager::GetKeyDown(VK_RIGHT)) LookRight();
	if (InputManager::GetKeyDown(VK_LEFT)) LookLeft();
}

//---------------------------------------------------------------------------------------
//----------------------------------------PRIVATE----------------------------------------
//---------------------------------------------------------------------------------------

void Camera::Forward() {
	glm::vec3 currPos = position;
	glm::vec3 orientVec(-sin(glm::radians(lookAngle)), 0.0f, -cos(glm::radians(lookAngle)));
	currPos += orientVec*ResourcesManager::targetFrameTime * speed;
	SetPosition(currPos);
}

void Camera::Backward() {
	glm::vec3 currPos = position;
	glm::vec3 orientVec(-sin(glm::radians(lookAngle)), 0.0f, -cos(glm::radians(lookAngle)));
	currPos -= orientVec*ResourcesManager::targetFrameTime * speed;
	SetPosition(currPos);
}

void Camera::Left() {
	glm::vec3 currPos = position;
	glm::vec3 orientVec(-sin(glm::radians(lookAngle)), 0.0f, -cos(glm::radians(lookAngle)));
	glm::vec3 cross = glm::cross(orientVec, glm::vec3(0.0f, 1.0f, 0.0f));
	currPos -= cross*ResourcesManager::targetFrameTime * speed;
	SetPosition(currPos);
}

void Camera::Right() {
	glm::vec3 currPos = position;
	glm::vec3 orientVec(-sin(glm::radians(lookAngle)), 0.0f, -cos(glm::radians(lookAngle)));
	glm::vec3 cross = glm::cross(orientVec, glm::vec3(0.0f, 1.0f, 0.0f));
	currPos += cross*ResourcesManager::targetFrameTime * speed;
	SetPosition(currPos);
}

void Camera::Up() {
	glm::vec3 currPos = position;
	currPos.y += ResourcesManager::targetFrameTime * speed;
	SetPosition(currPos);
}

void Camera::Down() {
	glm::vec3 currPos = position;
	currPos.y -= ResourcesManager::targetFrameTime * speed;
	SetPosition(currPos);
}

void Camera::LookLeft() {
	float currLookAngle = lookAngle;
	currLookAngle += ResourcesManager::targetFrameTime * turnSensitivity;
	SetLookAngle(currLookAngle);
}

void Camera::LookRight() {
	float currLookAngle = lookAngle;
	currLookAngle -= ResourcesManager::targetFrameTime * turnSensitivity;
	SetLookAngle(currLookAngle);
}
