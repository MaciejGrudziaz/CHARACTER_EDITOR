#include "Character.h"

void Character::LoadShaderProgramToAll(Shader* shaderProgram) {
	if (shaderProgram != nullptr) {
		for (unsigned i = 0; i < model.GetObjectsCount(); ++i) {
			model.GetObject_(i)->LoadShader(shaderProgram);
		}
	}
}

void Character::LoadObjectShaderProgram(int index, Shader* shaderProgram) {
	if (shaderProgram != nullptr && index < model.GetObjectsCount() && index>=0) {
		model.GetObject_(index)->LoadShader(shaderProgram);
	}
}

void Character::LoadHitboxShaderProgramToAll(Shader* hitboxShaderProgram_) {
	for (unsigned i = 0; i < model.GetObjectsCount(); ++i) {
		model.GetObject_(i)->LoadHitboxShader(hitboxShaderProgram_);
	}
}

void Character::LoadObjectHitboxShaderProgram(int index, Shader* shaderProgram) {
	if (shaderProgram != nullptr && index < model.GetObjectsCount() && index>=0) {
		model.GetObject_(index)->LoadHitboxShader(shaderProgram);
	}
}

void Character::LoadHitboxComputeShaderProgram(Shader* shaderProgram) {
	for (unsigned i = 0; i < model.GetObjectsCount(); ++i) {
		model.GetObject_(i)->LoadHitboxComputeShader(shaderProgram);
	}
}

void Character::StartAnimation(unsigned index) {
	for (unsigned i = 0; i < model.GetObjectsCount(); ++i) {
		model.GetObject_(i)->StartAnimation(index);
	}
}

void Character::SetViewMatrix(glm::mat4 view_) {
	for (unsigned i = 0; i < model.GetObjectsCount(); ++i) {
		model.GetObject_(i)->SetViewMatrix(view_);
	}
}

void Character::SetProjectionMatrix(glm::mat4 projection_) {
	for (unsigned i = 0; i < model.GetObjectsCount(); ++i) {
		model.GetObject_(i)->SetProjectionMatrix(projection_);
	}
}

bool Character::IsObjectDynamic(int index) {
	if (index >= 0 && index < model.GetObjectsCount()) {
		DynamicObject* obj = dynamic_cast<DynamicObject*>(model.GetObject_(index));
		if (obj == nullptr) return false;
		else return true;
	}
}

void Character::Init() {
	for (unsigned i = 0; i < model.GetObjectsCount(); ++i) {
		model.GetObject_(i)->Init();
	}
}

void Character::Update() {
	//if (transformUpdate) {
		transform = glm::translate(transform, position);
		transform = scale * transform;
	//}

	Object* obj = nullptr;
	glm::mat4 modelMat;
	for (unsigned i = 0; i < model.GetObjectsCount(); ++i) {
		obj = model.GetObject_(i);
		if (obj != nullptr)
			obj->SetModelMatrix(transform);
	}

	transform = glm::mat4(1.0);
	model.Update();
}

//---------------------------------------------------------------------------------------
//----------------------------------------PRIVATE----------------------------------------
//---------------------------------------------------------------------------------------

void Character::WriteErrorToFile(std::string message) {
	std::fstream file("LOGS/" + name + "_ERRORLOG.txt", std::ios::out | std::ios::app);
	assert(file.is_open());
	SYSTEMTIME time;
	GetSystemTime(&time);
	file << time.wDay << "." << time.wMonth << "." << time.wYear << " " << time.wHour << ":" << time.wMinute << ":" << time.wSecond << " - ";
	file << message << std::endl;
	file.close();
}