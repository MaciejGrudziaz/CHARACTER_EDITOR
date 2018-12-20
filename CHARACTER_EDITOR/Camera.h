#pragma once
#include <gl\glew.h>
#include <Windows.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "InputManager.h"
#include "ResourcesManager.h"
#include "CharacterManager.h"

class Camera {
	float lookAngle;

	glm::vec3 position;
	glm::vec3 lookVec;
	glm::vec3 upVec;

	glm::mat4 view;

	float speed;
	float turnSensitivity;

	void Forward();
	void Backward();
	void Left();
	void Right();
	void Up();
	void Down();
	void LookLeft();
	void LookRight();

public:
	Camera();
	Camera(glm::vec3 position_, glm::vec3 lookVec_, glm::vec3 upVec_);

	void Update(){ view = glm::lookAt(position, position + lookVec, upVec); }

	void SetPosition(glm::vec3 position_) { position = position_; view = glm::lookAt(position, position + lookVec, upVec); }
	void SetLookVec(glm::vec3 lookVec_) { lookVec = lookVec_; view = glm::lookAt(position, position + lookVec, upVec); }
	void SetUpVec(glm::vec3 upVec_) { upVec = upVec_; view = glm::lookAt(position, position + lookVec, upVec); }
	void SetLookAngle(float lookAngle_);

	glm::vec3 GetPosition() const { return position; }
	glm::vec3 GetLookVec() const { return lookVec; }
	glm::vec3 GetUpVec() const { return upVec; }
	const glm::mat4& GetView() const { return view; }
	float GetLookAngle()const { return lookAngle; }

	void Process();
};