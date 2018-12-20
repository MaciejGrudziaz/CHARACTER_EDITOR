#pragma once
#include <glm/glm.hpp>
#include <fbxsdk.h>
#include <exception>
#include <string>
#include <fstream>
#include <Windows.h>
#include "BasicModel.h"

class FBXloader {
	FBXloader():errorCode(0) {}
	FBXloader(const FBXloader&) {}
	FBXloader& operator=(const FBXloader&) {}

	void GetMeshNode(FbxNode* rootNode, FbxNode** meshNode, FbxMesh** mesh);

	void LoadVertices(BasicObject* object, FbxMesh* mesh);
	void LoadGlobalTransformMatrix(BasicObject* object, FbxNode* meshNode);
	void LoadFaces(BasicObject* object, FbxMesh* mesh);
	void LoadNormals(BasicObject* object, FbxMesh* mesh);
	void LoadUV(BasicObject* object, FbxMesh* mesh);
	void LoadTextures(BasicObject* object, FbxNode* meshNode);

	void ProcessSkeletonHierarchy(BasicObject* object,FbxNode* inRootNode);
	void ProcessSkeletonHierarchyRecursively(BasicObject* object, FbxNode* inNode, int myIndex, int inParentIndex);
	void ProcessJointsAndAnimations(BasicObject* object,FbxNode* inNode, FbxScene* mFBXScene);
	FbxAMatrix GetGeometryTransformation(FbxNode* inNode);

	void LoadFbxMatToGlmMat(const FbxAMatrix& fbxMat, glm::mat4& glmMat);
	void WriteErrorToFile(std::string message);

	int errorCode;

public:
	static FBXloader* GetInstance();

	int LoadModel(const char* filename, BasicModel* object);

	int GetErrorCode()const { return errorCode; }
};