#include "Animation.h"

AnimationManager::AnimationManager(BasicObject* basicObject_): basicObject(basicObject_),jointMatChange(false){
	baseAnimationFPS = 24;
	baseAnimationFrameTime = 1.0 / static_cast<float>(baseAnimationFPS);
	interpolationVal = 0.0;
	anyActive = 0.0;
	maxFrameCount = 0;
	jointsPrevMatrices = jointsNextMatrices = nullptr;

	glm::mat4 identity(1.0f);
	jointsMatricesCount = basicObject->skeleton.joints.size();
	jointsPrevMatrices = new float[16 * jointsMatricesCount];
	jointsNextMatrices = new float[16 * jointsMatricesCount];
	for (unsigned i = 0; i < jointsMatricesCount; ++i) {
		for (unsigned j = 0; j < 16; ++j) {
			jointsPrevMatrices[16 * i + j] = glm::value_ptr(identity)[j];
			jointsNextMatrices[16 * i + j] = glm::value_ptr(identity)[j];
		}
	}

	InitAnimationsVec();
}

void AnimationManager::Start(int animationIndex_) {
	if (animationIndex_ < 0 || animationIndex_ >= animations.size()) {
		WriteErrorToFile("Wrong animation index: " + std::to_string(animationIndex_));
		throw std::exception("Wrong animation index!");
	}

	animations[animationIndex_]->active = true;
	animations[animationIndex_]->currentFrame = 0;
	anyActive = true;
	prevTime = boost::chrono::high_resolution_clock::now();
}

void AnimationManager::Start(std::string animationName_) {
	int index = -1;
	for (unsigned i = 0; i < animations.size(); ++i) {
		if (animations[i]->name == animationName_) {
			index = i;
			break;
		}
	}
	if (index == -1) {
		WriteErrorToFile("Wrong animation name: " + animationName_);
		throw std::exception("Wrong namiation name!");
	}

	animations[index]->active = true;
	animations[index]->currentFrame = 0;
	anyActive = true;
	prevTime = boost::chrono::high_resolution_clock::now();
}

void AnimationManager::Stop(int animationIndex_) {
	if (animationIndex_ < 0 || animationIndex_ >= animations.size()) {
		WriteErrorToFile("Wrong animation index: " + std::to_string(animationIndex_));
		throw std::exception("Wrong animation index!");
	}

	animations[animationIndex_]->active = false;
	animations[animationIndex_]->currentFrame = 0;
	anyActive = false;
	for (Animation* anim : animations) {
		if (anim->active == true) {
			anyActive = true;
			break;
		}
	}
}

void AnimationManager::Process() {
	if (anyActive) {
		if (interpolationVal >= 1.0) {
			++currentFrameCount;
			jointMatChange = true;
			if (currentFrameCount > maxFrameCount - 1) {
				currentFrameCount = 0;
				time = 0.0;
			}
			interpolationVal = 0.0;

			SetAnimationsNextFrame();

			for (unsigned i = 0; i < jointsMatricesCount; ++i) {
				if (basicObject->skeleton.joints[i]->animations.size() > 0) {
					glm::mat4 nextJointTransform,currJointTransform;
					//currJointTransform = GetAnimationsCurrJointMatrix(i);
					nextJointTransform = GetAnimationsNextJointMatrix(i);

					//if (currJointTransform != nextJointTransform) {
						//void* prevMat, *nextMat;
						//prevMat = glMapNamedBufferRange(shaderManager->GetJointsPrevMatricesBuffer(), i*(16 * sizeof(float)), 16 * sizeof(float), GL_MAP_WRITE_BIT);
						//nextMat = glMapNamedBufferRange(shaderManager->GetJointsNextMatricesBuffer(), i*(16 * sizeof(float)), 16 * sizeof(float), GL_MAP_WRITE_BIT);
						//memcpy(prevMat, glm::value_ptr(currJointTransform), 16 * sizeof(float));
						//memcpy(nextMat, glm::value_ptr(nextJointTransform), 16 * sizeof(float));
						//glUnmapNamedBuffer(shaderManager->GetJointsPrevMatricesBuffer());
						//glUnmapNamedBuffer(shaderManager->GetJointsNextMatricesBuffer());
					//}

					for (unsigned j = 0; j < 4; ++j) {
						for (unsigned k = 0; k < 4; ++k) {
							jointsPrevMatrices[16 * i + j * 4 + k] = jointsNextMatrices[16 * i + j * 4 + k];
							jointsNextMatrices[16 * i + j * 4 + k] = nextJointTransform[j][k];
						}
					}
				}
			}

			//void *prevMat, *nextMat;
			//prevMat= glMapNamedBufferRange(shaderManager->GetJointsPrevMatricesBuffer(), 0, jointsMatricesCount * 16 * sizeof(float), GL_MAP_WRITE_BIT);
			//nextMat= glMapNamedBufferRange(shaderManager->GetJointsNextMatricesBuffer(), 0, jointsMatricesCount * 16 * sizeof(float), GL_MAP_WRITE_BIT);
			//memcpy(prevMat, jointsPrevMatrices, jointsMatricesCount * 16 * sizeof(float));
			//memcpy(nextMat, jointsNextMatrices, jointsMatricesCount * 16 * sizeof(float));
			//glUnmapNamedBuffer(shaderManager->GetJointsPrevMatricesBuffer());
			//glUnmapNamedBuffer(shaderManager->GetJointsNextMatricesBuffer());
		}
		else {
			jointMatChange = false;
			time += ResourcesManager::targetFrameTime;
			prevTime = boost::chrono::high_resolution_clock::now();
			interpolationVal = (time / baseAnimationFrameTime) - static_cast<float>(currentFrameCount);
		}
	}
}

glm::mat4 AnimationManager::GetJointPrevFrameTransformMatrix(int jointIdx) {
	if (jointIdx >= 0 && jointIdx < basicObject->skeleton.joints.size()) {
		glm::mat4 prevFrame = glm::mat4(1.0f);

		for (unsigned i = 0; i < animations.size(); ++i) {
			if (basicObject->skeleton.joints[jointIdx]->animations.size() > 0) {
				prevFrame *= (basicObject->skeleton.joints[jointIdx]->animations[i]->frames[animations[i]->currentFrame]->globalTransform*basicObject->skeleton.joints[jointIdx]->globalBindposeInverse);
			}
		}

		return prevFrame;
	}
	else throw std::exception();
}

glm::mat4 AnimationManager::GetJointNextFrameTransformMatrix(int jointIdx) {
	if (jointIdx >= 0 && jointIdx < basicObject->skeleton.joints.size()) {
		glm::mat4 nextFrame = glm::mat4(1.0f);;

		for (unsigned i = 0; i < animations.size(); ++i) {
			if (basicObject->skeleton.joints[jointIdx]->animations.size() > 0) {
				if (animations[i]->currentFrame<animations[i]->frameCount - 1)
					nextFrame *= (basicObject->skeleton.joints[jointIdx]->animations[i]->frames[animations[i]->currentFrame]->globalTransform*basicObject->skeleton.joints[jointIdx]->globalBindposeInverse);
				else nextFrame *= (basicObject->skeleton.joints[jointIdx]->animations[i]->frames[0]->globalTransform*basicObject->skeleton.joints[jointIdx]->globalBindposeInverse);
			}
		}

		return nextFrame;
	}
	else throw std::exception();
}

glm::vec4 AnimationManager::TransformVertexByCurrentJointMatrix(glm::vec4 vertex, int jointIdx) {
	if (jointIdx >= 0 && jointIdx < basicObject->skeleton.joints.size()) {
		glm::mat4 prevFrame, nextFrame;
		glm::vec4 outVertex;
		prevFrame = GetJointPrevFrameTransformMatrix(jointIdx);
		nextFrame = GetJointNextFrameTransformMatrix(jointIdx);

		outVertex = ((1.0f - interpolationVal)*prevFrame + interpolationVal * nextFrame)*vertex;

		return outVertex;
	}
	else throw std::exception();
}

//---------------------------------------------------------------------------------------
//----------------------------------------PRIVATE----------------------------------------
//---------------------------------------------------------------------------------------

void AnimationManager::InitAnimationsVec() {
	maxFrameCount = 0;
	if (basicObject->animationsInfo.size() > 0)
		maxFrameCount = basicObject->animationsInfo[0].frameCount;
	for (unsigned i = 0; i < basicObject->animationsInfo.size(); ++i) {
		animations.push_back(new Animation(basicObject->animationsInfo[i].name, basicObject->animationsInfo[i].frameCount));
		if (basicObject->animationsInfo[i].frameCount > maxFrameCount)
			maxFrameCount = basicObject->animationsInfo[i].frameCount;
	}
}

void AnimationManager::SetAnimationsNextFrame() {
	for (Animation* animation : animations) {
		if (animation->active) {
			++animation->currentFrame;
			if (animation->currentFrame > animation->frameCount - 1)
				animation->currentFrame = 0;
		}
	}
}

glm::mat4 AnimationManager::GetAnimationsCurrJointMatrix(unsigned jointIdx) {
	glm::mat4 jointTransformMatrix;
	if (basicObject->skeleton.joints[jointIdx]->animations.size() > 0) {
		for (unsigned i = 0; i < animations.size(); ++i) {
			if (animations[i]->active) {
				if (animations[i]->currentFrame < animations[i]->frameCount)
					jointTransformMatrix = (basicObject->skeleton.joints[jointIdx]->animations[i]->frames[animations[i]->currentFrame]->globalTransform * basicObject->skeleton.joints[jointIdx]->globalBindposeInverse);// *jointTransformMatrix;
				else jointTransformMatrix = (basicObject->skeleton.joints[jointIdx]->animations[i]->frames[0]->globalTransform * basicObject->skeleton.joints[jointIdx]->globalBindposeInverse);// *jointTransformMatrix;
			}
		}
	}

	return jointTransformMatrix;
}

glm::mat4 AnimationManager::GetAnimationsNextJointMatrix(unsigned jointIdx) {
	glm::mat4 jointTransformMatrix(1.0f);
	if (basicObject->skeleton.joints[jointIdx]->animations.size() > 0) {
		for (unsigned i = 0; i < animations.size(); ++i) {
			if (animations[i]->active) {
				if (animations[i]->currentFrame < animations[i]->frameCount - 1)
					jointTransformMatrix *= (basicObject->skeleton.joints[jointIdx]->animations[i]->frames[animations[i]->currentFrame + 1]->globalTransform * basicObject->skeleton.joints[jointIdx]->globalBindposeInverse);// *jointTransformMatrix;
				else jointTransformMatrix *= (basicObject->skeleton.joints[jointIdx]->animations[i]->frames[0]->globalTransform * basicObject->skeleton.joints[jointIdx]->globalBindposeInverse);// *jointTransformMatrix;
			}
		}
	}

	return jointTransformMatrix;
}

void AnimationManager::WriteErrorToFile(std::string message) {
	std::fstream file("LOGS/MODEL_ANIMATION_ERRORLOG.txt", std::ios::out | std::ios::app);
	assert(file.is_open());
	SYSTEMTIME time;
	GetSystemTime(&time);
	file << time.wDay << "." << time.wMonth << "." << time.wYear << " " << time.wHour << ":" << time.wMinute << ":" << time.wSecond << " - ";
	file << message << std::endl;
	file.close();
}
