#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <fbxsdk.h>

struct BasicObject {
	struct Face {
		int indices[3];
		glm::vec4 normals[3];
		glm::vec2 uv[3];
	};

	struct Keyframe {
		int frameNum;
		glm::mat4 globalTransform;
		Keyframe* nextFrame;
	};

	struct AnimationInfo {
		std::string name;
		int frameCount;
	};

	struct AnimationJoint {
		std::string name;
		int frameCount;
		std::vector<Keyframe*> frames;
		AnimationJoint():frameCount(0) {}
		~AnimationJoint() {
			for (BasicObject::Keyframe* frame : frames)
				delete frame;
		}
	};

	struct Joint {
		int parentIndex;
		std::string name;

		glm::mat4 globalBindposeInverse;

		std::vector<AnimationJoint*> animations;

		Joint() :parentIndex(-1) {}

		~Joint() {
			for (AnimationJoint* anim : animations)
				delete anim;
		}
	};

	struct Skeleton {
		int FindJointIndexUsingName(std::string jointName) {
			for (unsigned i = 0; i < joints.size(); ++i) {
				if (joints[i]->name == jointName) return i;
			}
			return -1;
		}

		std::vector<Joint*> joints;

		~Skeleton() {
			for (Joint* joint : joints)
				delete joint;
		}
	};

	struct BlendingIndexWeightPair {
		float jointIndex;
		float weight;
	};

	struct ControlPoint {
		glm::vec4 coords;
		std::vector<BlendingIndexWeightPair> blendingInfo;
	};

	glm::mat4 globalTransform;

	std::vector<ControlPoint> controlPoints;

	std::vector<Face> faces;

	std::string name;
	std::string textureName;

	Skeleton skeleton;

	std::vector<AnimationInfo> animationsInfo;
};