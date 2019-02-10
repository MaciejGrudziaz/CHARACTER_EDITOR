#include "Hitbox.h"

unsigned Hitbox::indices[] = {
	0, 1, 1, 2, 2, 3, 3, 0,
	0, 4, 1, 5, 2, 6, 3, 7,
	4, 5, 5, 6, 6, 7, 7, 4
};

Hitbox::Hitbox() :jointIdx(-1), name(""), damageMultiplier(1.0){ }

//-----------------------------------------------------------------------------
//-----------------------------HITBOX AXIS SHADER------------------------------
//-----------------------------------------------------------------------------

void HitboxAxisShader::GetHitboxJointIdxLoc() {
	hitboxJointIdxLoc = glGetUniformLocation(shader->GetProgram(), "hitboxJointIdx");

	if (hitboxJointIdxLoc < 0)
		throw std::exception();
}

void HitboxAxisShader::GetLocalAxisLoc() {
	localAxisLoc = glGetUniformLocation(shader->GetProgram(), "localAxis");
	if (localAxisLoc < 0)
		throw std::exception();
}

void HitboxAxisShader::GetModelLoc() {
	modelLoc = glGetUniformLocation(shader->GetProgram(), "model");
	if (modelLoc < 0)
		throw std::exception();
}

void HitboxAxisShader::GetViewLoc() {
	viewLoc = glGetUniformLocation(shader->GetProgram(), "view");
	if (viewLoc < 0)
		throw std::exception();
}

void HitboxAxisShader::GetProjectionLoc() {
	projectionLoc = glGetUniformLocation(shader->GetProgram(), "projection");
	if (projectionLoc < 0)
		throw std::exception();
}

void HitboxAxisShader::GetInterpolationLoc() {
	interpolationLoc = glGetUniformLocation(shader->GetProgram(), "interpolation");
	if (interpolationLoc < 0)
		throw std::exception();
}

void HitboxAxisShader::InitHitboxVerticesBuffer() {
	glGenBuffers(1, &hitboxVerticesBuffer);

	glUseProgram(shader->GetProgram());
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, hitboxVerticesBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 32 * sizeof(float), NULL, GL_DYNAMIC_DRAW);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, hitboxVerticesBuffer);

	transformedHitboxBuffPtr = glMapBufferRange(GL_SHADER_STORAGE_BUFFER,NULL,32*sizeof(float), GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
}

void HitboxAxisShader::LoadLocalAxisBuffer(const Hitbox::Axis& localAxis_) {
	for (int i = 0; i < 3; ++i) {
		localAxisBuff[i] = localAxis_.x[i];
		localAxisBuff[3 + i] = localAxis_.y[i];
		localAxisBuff[6 + i] = localAxis_.z[i];
	}
}

void HitboxAxisShader::LoadHitboxBufferData(const float* hitboxTransformedVertices, unsigned hitboxTransformedVerticesSize) {
	int startIdx = -1;
	
	for (unsigned i = 3; i < hitboxTransformedVerticesSize; i+=4) {
		if (hitboxTransformedVertices[i] == hitboxJointIdx) {
			startIdx = i - 3;
			break;
		}
	}

	if (startIdx != -1) {
		for (int i = startIdx, j = 0; i < startIdx+32, j < 32; ++i, ++j) {
			transformedHitboxBuff[j] = hitboxTransformedVertices[i];
			((float*)(transformedHitboxBuffPtr))[j] = hitboxTransformedVertices[i];
		}
	}
	else {
		for (int i = 0; i < 32; ++i) {
			transformedHitboxBuff[i] = 0.0f;
			((float*)(transformedHitboxBuffPtr))[i] = 0.0f;
		}
	}
}

//-----------------------------------------------------------------------------
//-------------------------DYNAMIC HITBOX AXIS SHADER--------------------------
//-----------------------------------------------------------------------------

void DynamicAxisHitboxShader::Init(Shader* shader_,glm::mat4 projection_, int hitboxJointIdx_) {
	if (shader_ != nullptr) {
		shader = shader_;
		projection = projection_;
		hitboxJointIdx = hitboxJointIdx_;

		GetHitboxJointIdxLoc();
		GetLocalAxisLoc();
		GetModelLoc();
		GetViewLoc();
		GetProjectionLoc();
		GetInterpolationLoc();

		InitHitboxVerticesBuffer();
	}
}

void DynamicAxisHitboxShader::Update(glm::mat4 model_, glm::mat4 view_, float interpolation_) {
	view = view_;
	model = model_;
	interpolation = interpolation_;
}

void DynamicAxisHitboxShader::Draw() {
	glUseProgram(shader->GetProgram());

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, hitboxVerticesBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, prevJointsMatBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, nextJointsMatBuffer);

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glUniform1f(interpolationLoc, interpolation);
	glUniform1i(hitboxJointIdxLoc, hitboxJointIdx);
	glUniform3fv(localAxisLoc, 3, localAxisBuff);

	glDrawArrays(GL_LINES, 0, 6);
}

//-----------------------------------------------------------------------------
//--------------------------STATIC HITBOX AXIS SHADER--------------------------
//-----------------------------------------------------------------------------

void StaticAxisHitboxShader::Init(Shader* shader_, glm::mat4 projection_, int hitboxJointIdx_) {
	if (shader_ != nullptr) {
		shader = shader_;
		projection = projection_;
		hitboxJointIdx = hitboxJointIdx_;

		GetHitboxJointIdxLoc();
		GetLocalAxisLoc();
		GetModelLoc();
		GetViewLoc();
		GetProjectionLoc();

		InitHitboxVerticesBuffer();
	}
}

void StaticAxisHitboxShader::Update(glm::mat4 model_, glm::mat4 view_, float interpolation) {
	model = model_;
	view = view_;
}

void StaticAxisHitboxShader::Draw() {
	glUseProgram(shader->GetProgram());

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, hitboxVerticesBuffer);

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glUniform1i(hitboxJointIdxLoc, hitboxJointIdx);
	glUniform3fv(localAxisLoc, 3, localAxisBuff);

	glDrawArrays(GL_LINES, 0, 6);
}