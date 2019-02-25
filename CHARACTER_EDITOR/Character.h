#pragma once
#include <vector>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <boost/chrono.hpp>
#include "Shader.h"
#include "FBXloader.h"
#include "ResourcesManager.h"
#include "Model.h"

class Character {
	std::string name;
	glm::vec3 position;
	float rotation;

	glm::mat4 transform;
	glm::mat4 scale;
	bool transformUpdate;

	Model model;

	void WriteErrorToFile(std::string message);

public:
	Character():position(glm::vec3(0.0f)),name("Character"),rotation(0.0f),transformUpdate(false),scale(glm::mat4(1.0f)) {}
	Character(std::string name_):position(glm::vec3(0.0f)), name(name_), rotation(0.0f), transformUpdate(false),scale(glm::mat4(1.0f)) { }

	void InitModel(BasicModel* basicModel_) { model.Init(basicModel_); }

	void LoadShaderProgramToAll(Shader* shaderProgram);
	void LoadObjectShaderProgram(int index, Shader* shaderProgram);

	void LoadHitboxShaderProgramToAll(Shader* hitboxShaderProgram_);
	void LoadObjectHitboxShaderProgram(int index, Shader* sahderProgram);

	void LoadHitboxComputeShaderProgram(Shader* shaderProgram);

	void StartAnimation(unsigned index);

	void SetName(std::string name_) { name = name_; }
	void SetPosition(const glm::vec3& position_) { position = position_; transformUpdate = true; }
	void SetViewMatrix(glm::mat4 view_);
	void SetProjectionMatrix(glm::mat4 projection_);
	void SetScaleMat(glm::mat4 newScale) { scale = newScale; }
	void SetScaleMat(float x, float y, float z) { scale = glm::scale(glm::vec3(x, y, z)); }
	void SetScaleMat(float val) { scale = glm::scale(glm::vec3(val)); }

	glm::mat4 GetScaleMat()const { return scale; }

	std::string GetName()const { return name; }
	glm::vec3 GetPosition()const { return position; }
	unsigned GetObjectsCount()const { return model.GetObjectsCount(); }
	bool IsObjectDynamic(int index);

	void Transform(const glm::mat4& transform_) { transform = transform_; transformUpdate = true; }

	void Init();

	void Update();

	void Draw() { model.Draw(); }

	//----------------------------FOR CONSOLE EDIT-----------------------------
	Model* GetModel() { return &model; }
	void SetCurrentJointIdx(int objectIdx, int hitboxIdx) {
		Object* obj = model.GetObject_(objectIdx);
		if (obj != nullptr)
			obj->SetCurrentHitboxJointIdx(hitboxIdx);
	}
	//-------------------------------------------------------------------------
};
