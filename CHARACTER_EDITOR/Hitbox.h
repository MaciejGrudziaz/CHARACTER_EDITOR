#pragma once
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <set>
#include <vector>
#include <string>

struct Hitbox {
	struct Axis {
		glm::vec3 x;
		glm::vec3 y;
		glm::vec3 z;

		Axis() :x(glm::vec3(1.0f, 0.0f, 0.0f)), y(glm::vec3(0.0f, 1.0f, 0.0f)), z(glm::vec3(0.0f, 0.0f, 1.0f)) {}
	};
	struct MoveValues {
		float xMove[2];
		float yMove[2];
		float zMove[2];

		MoveValues() :xMove{ 0.0f,0.0f }, yMove{ 0.0f,0.0f }, zMove{ 0.0f,0.0f } {}
	};
	struct IndexGroup {
		std::set<int> idx;
		int baseIdx;
		int status;
		enum Status { OPEN, CLOSED, IN_USE };

		IndexGroup(int baseIdx_) :baseIdx(baseIdx_), status(OPEN), idx(std::set<int>()) {}
	};

	std::string name;
	std::vector<int> controlPointsIdx;

	Axis localAxis;
	glm::vec4 initVertices[8];
	glm::vec4 basicVertices[8];
	glm::vec4 transformVertices[8];
	glm::vec3 basicNormals[6];
	glm::vec3 transformNormals[6];
	int jointIdx;
	static unsigned indices[12 * 2];

	double damageMultiplier;

	Hitbox();
};

//-----------------------------------------------------------------------------
//-----------------------------HITBOX AXIS SHADER------------------------------
//-----------------------------------------------------------------------------

class HitboxAxisShader {
protected:
	Shader* shader;

	int hitboxJointIdxLoc;
	int localAxisLoc;
	int modelLoc;
	int viewLoc;
	int projectionLoc;
	int interpolationLoc;

	glm::mat4 view;
	glm::mat4 projection;
	glm::mat4 model;
	float interpolation;
	int hitboxJointIdx;

	unsigned prevJointsMatBuffer;
	unsigned nextJointsMatBuffer;
	unsigned hitboxVerticesBuffer;

	float localAxisBuff[9];
	float transformedHitboxBuff[4 * 8];
	void* transformedHitboxBuffPtr;

public:

	HitboxAxisShader() :transformedHitboxBuffPtr(nullptr), shader(nullptr), hitboxJointIdxLoc(-1), localAxisLoc(-1), modelLoc(-1), viewLoc(-1), projectionLoc(-1), interpolationLoc(-1) { }

	void GetHitboxJointIdxLoc();
	void GetLocalAxisLoc();
	void GetModelLoc();
	void GetViewLoc();
	void GetProjectionLoc();
	void GetInterpolationLoc();

	void InitHitboxVerticesBuffer();

	void SetHitboxJointIdx(int hitboxJointIdx_) { hitboxJointIdx = hitboxJointIdx_; }
	void SetProjection(glm::mat4 projection_) { projection = projection_; }
	void SetPrevJointsMatBuffer(unsigned prevJointsMatBuffer_) { prevJointsMatBuffer = prevJointsMatBuffer_; }
	void SetNextJointsMatBuffer(unsigned nextJointsMatBuffer_) { nextJointsMatBuffer = nextJointsMatBuffer_; }
	//void SetTransformedHitboxVerticesBuffer(unsigned transformedHitboxVerticesBuffer_) { transformedHitboxVerticesBuffer = transformedHitboxVerticesBuffer_; }
	void LoadLocalAxisBuffer(const Hitbox::Axis& localAxis_);
	void LoadHitboxBufferData(const float* hitboxTransformedVertices,unsigned hitboxTransformedVerticesSize);

	virtual void Init(Shader* shader_,glm::mat4 projection, int hitboxJointIdx_) = 0;
	virtual void Update(glm::mat4 view_, glm::mat4 model_, float interpolation_) = 0;
	virtual void Draw() = 0;

	virtual ~HitboxAxisShader() {
		if (transformedHitboxBuffPtr != nullptr) {
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, hitboxVerticesBuffer);
			glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
		}
	}
};

class DynamicAxisHitboxShader :public HitboxAxisShader {
public:
	void Init(Shader* sahder_,glm::mat4 projection_, int hitboxJointIdx_);
	void Update(glm::mat4 model_, glm::mat4 view_, float interpolation_);
	void Draw();
};

class StaticAxisHitboxShader :public HitboxAxisShader {
public:
	void Init(Shader* shader_, glm::mat4 projection_, int hitboxJointIdx_);
	void Update(glm::mat4 model_, glm::mat4 view_, float interpolation = 0.0f);
	void Draw();
};