#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <boost/chrono.hpp>
#include "Shader.h"
#include "BasicObject.h"
#include "ResourcesManager.h"

struct Animation {
	std::string name;
	bool active;
	int currentFrame;
	int frameCount;

	bool switchToFirst;

	Animation(std::string name_, int frameCount_) :name(name_), frameCount(frameCount_), active(false), currentFrame(0), switchToFirst(false) {}
};

class AnimationManager {
	int baseAnimationFPS;
	float baseAnimationFrameTime;

	BasicObject* basicObject;

	bool jointMatChange;
	float* jointsPrevMatrices;
	float* jointsNextMatrices;
	float* identityMatricesJointsBuffer;
	unsigned jointsMatricesCount;

	float interpolationVal;

	int maxFrameCount;
	int currentFrameCount;
	bool anyActive;

	boost::chrono::high_resolution_clock::time_point prevTime;
	float time;

	std::vector<Animation*> animations;

	void InitAnimationsVec();

	void SetAnimationsNextFrame();

	glm::mat4 GetAnimationsNextJointMatrix(unsigned jointIdx);
	glm::mat4 GetAnimationsCurrJointMatrix(unsigned jointIdx);

	void WriteErrorToFile(std::string message);

public:
	AnimationManager(BasicObject* object_);

	void Start(int animationIndex_);
	void Start(std::string animationName_);
	void Stop(int animationIndex_);
	int GetAnimationsCount()const { return animations.size(); }

	void Process();

	unsigned GetJointsMatricesCount()const { return jointsMatricesCount; }
	float GetInterpolationVal()const { return interpolationVal; }
	const float* GetJointsNextTransformMatrices()const { return jointsNextMatrices; }
	const float* GetJointsPreviousTransformMatrices()const { return jointsPrevMatrices; }
	bool GetJointsMatChangeStatus()const { return jointMatChange; }

	glm::mat4 GetJointPrevFrameTransformMatrix(int jointIdx);
	glm::mat4 GetJointNextFrameTransformMatrix(int jointIdx);
	glm::vec4 TransformVertexByCurrentJointMatrix(glm::vec4 vertex, int jointIdx);

	Animation* GetAnimation(int index) { if (index >= 0 && index <= animations.size() - 1) return animations[index]; else return nullptr; }
	int GetBaseAnimtionFPS()const { return baseAnimationFPS; }

	~AnimationManager() {
		delete[]jointsPrevMatrices;
		delete[]jointsNextMatrices;
		delete[]identityMatricesJointsBuffer;
	}

};