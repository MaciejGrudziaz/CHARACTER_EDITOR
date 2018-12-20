#pragma once
#include <stb_image.h>
#include "Shader.h"

class ObjectShaderManager {
protected:
	//-------------------------------------------------------------------------
	//----------------------------------DATA-----------------------------------
	//-------------------------------------------------------------------------
	Shader* shaderProgram;
	Shader* hitboxShaderProgram;
	Shader* hitboxComputeShaderProgram;

	int modelLoc;
	int viewLoc;
	int projectionLoc;

	int modelHitboxLoc;
	int viewHitboxLoc;
	int projectionHitboxLoc;

	int hitboxColor1Loc;
	int hitboxColor2Loc;
	int hitboxSetJointIdxLoc;

	int modelHitboxComputeLoc;
	
	int objectBufferVertexAttribCount;
	int hitboxObjectBufferVertexAttribCount;

	unsigned VBO;
	unsigned VAO;
	unsigned texture;

	unsigned hitboxVBO;
	unsigned hitboxVAO;
	unsigned hitboxIndicesBuffer;

	unsigned hitboxComputeOutBuffer;
	unsigned hitboxComputeInBuffer;
	void* hitboxComputeOutBufferPtr;

	int textureAvailableLoc;

	//-------------------------------------------------------------------------
	//-----------------------------ANIMATIONS DATA-----------------------------
	//-------------------------------------------------------------------------

	unsigned jointsPrevMatricesBuffer;
	unsigned jointsNextMatricesBuffer;
	void *jointsPrevMatBufferPtr, *jointsNextMatBufferPtr;

	int interpolationLoc;
	int hitboxInterpolationLoc;
	int hitboxComputeInterpoaltionLoc;

	//-------------------------------------------------------------------------
	//----------------------------PRIVATE FUNCTIONS----------------------------
	//-------------------------------------------------------------------------
	virtual void GenerateBuffersAndArrays();

	virtual void LoadModelMatrixLoc();
	virtual void LoadViewMatrixLoc();
	virtual void LoadProjectionMatrixLoc();
	virtual void LoadTextureAvailableLoc();

	virtual void LoadHitboxModelMatrixLoc();
	virtual void LoadHitboxViewMatrixLoc();
	virtual void LoadHitboxProjectionMatrixLoc();

	virtual void LoadHitboxColor1Loc();
	virtual void LoadHitboxColor2Loc();
	virtual void LoadHitboxSetCurrJointIdxLoc();

	virtual void LoadHitboxComputeModelMatrixLoc();

	virtual void LoadInterpolationLoc();
	virtual void LoadHitboxInterpolationLoc();
	virtual void LoadHitboxComputeInterpolationLoc();

	void WriteErrorToFile(const char* message);

public:
	ObjectShaderManager();

	int LoadMainShaderProgram(Shader* shaderProgram_);
	int LoadHitboxVisualizationShaderProgram(Shader* shaderProgram_);
	int LoadHitboxComputeShaderProgram(Shader* shaderProgram_);

	void SetObjectBufferVertexAttribCount(int vertexAttribCount_) { objectBufferVertexAttribCount = vertexAttribCount_; }
	void SetHitboxObjectBufferVertexAttribCount(int vertexAttribCount_) { hitboxObjectBufferVertexAttribCount = vertexAttribCount_; }

	virtual void SetVAO(float* verticesBuffer, int verticesCount) = 0;
	virtual void SetHitboxVAO(float* verticesBuffer, unsigned* indicesBuffer, int verticesCount) = 0;
	virtual void SetHitboxComputeBuffers(float* verticesBuffer_, int verticesCount_);

	virtual void SetJointsBuffers(int jointsCount) = 0;

	virtual void LoadTexture(const char* filename);

	const Shader* GetMainShader() const { return shaderProgram; }
	const Shader* GetHitboxShader()const { return hitboxShaderProgram; }
	const Shader* GetHitboxComputeShader()const { return hitboxComputeShaderProgram; }

	virtual int GetModelLoc()const { return modelLoc; }
	virtual int GetViewLoc()const { return viewLoc; }
	virtual int GetProjectionLoc()const { return projectionLoc; }
	virtual int GetModelHitboxLoc()const { return modelHitboxLoc; }
	virtual int GetViewHitboxLoc()const { return viewHitboxLoc; }
	virtual int GetProjectionHitboxLoc()const { return projectionHitboxLoc; }
	virtual int GetHitboxColor1Loc()const { return hitboxColor1Loc; }
	virtual int GetHitboxColor2Loc()const { return hitboxColor2Loc; }
	virtual int GetHitboxSetJointIdxLoc()const { return hitboxSetJointIdxLoc; }
	virtual int GetModelHitboxComputeLoc()const { return modelHitboxComputeLoc; }
	virtual unsigned GetVBO()const { return VBO; }
	virtual unsigned GetVAO()const { return VAO; }
	virtual unsigned GetTexture()const { return texture; }
	virtual unsigned GetHitboxVBO()const { return hitboxVAO; }
	virtual unsigned GetHitboxVAO()const { return hitboxVAO; }
	virtual unsigned GetHitboxIndicesBuffer()const { return hitboxIndicesBuffer; }
	virtual unsigned GetHitboxComputeOutBuffer()const { return hitboxComputeOutBuffer; }
	virtual unsigned GetHitboxComputeInBuffer()const { return hitboxComputeInBuffer; }
	virtual void* GetHitboxComputeOutBuferPtr()const { return hitboxComputeOutBufferPtr; }
	virtual int GetTextureAvailableLoc()const { return textureAvailableLoc; }
	virtual unsigned GetJointsPrevMatricesBuffer()const { return jointsPrevMatricesBuffer; }
	virtual unsigned GetJointsNextMatricesBuffer()const { return jointsNextMatricesBuffer; }
	virtual int GetInterpolationLoc()const { return interpolationLoc; }
	virtual int GetHitboxInterpolationLoc()const { return hitboxInterpolationLoc; }
	virtual int GetHitboxComputeInterpolationLoc()const { return hitboxComputeInterpoaltionLoc; }
	virtual void* GetJointsPrevMatBufferPtr() { return jointsPrevMatBufferPtr; }
	virtual void* GetJointsNextMatBufferPtr() { return jointsNextMatBufferPtr; }
	//virtual unsigned GetHitboxComputeInBuffer() { return hitboxComputeInBuffer; }
	//virtual unsigned GetHitboxComputeOutBuffer() { return hitboxComputeOutBuffer; }


	virtual void Init() = 0;

	virtual ~ObjectShaderManager() {
		glUnmapNamedBuffer(hitboxComputeOutBuffer);

		glDeleteVertexArrays(1, &VAO);
		glDeleteVertexArrays(1, &hitboxVAO);

		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &hitboxVBO);
		glDeleteBuffers(1, &hitboxIndicesBuffer);
		glDeleteBuffers(1, &hitboxComputeInBuffer);
		glDeleteBuffers(1, &hitboxComputeOutBuffer);
	}
};

class DynamicObjectShaderManager :public ObjectShaderManager {
	void GenerateBuffersAndArrays();

public:
	void SetVAO(float* verticesBuffer, int verticesCount);
	void SetHitboxVAO(float* verticesBuffer, unsigned* indicesBuffer, int verticesCount);

	void SetJointsBuffers(int jointsCount);

	void Init();

	~DynamicObjectShaderManager() {
		glUnmapNamedBuffer(jointsPrevMatricesBuffer);
		glUnmapNamedBuffer(jointsNextMatricesBuffer);

		glDeleteBuffers(1, &jointsPrevMatricesBuffer);
		glDeleteBuffers(1, &jointsNextMatricesBuffer);
	}
};

class StaticObjectShaderManager :public ObjectShaderManager {

	void SetJointsBuffers(int jointsCount) {}

	int GetJointsPrevMatricesLoc()const { return -1; }
	int GetJointsNextMatricesLoc()const { return -1; }
	int GetInterpolationLoc()const { return -1; }
	int GetJointsHitboxPrevMatricesLoc()const { return -1; }
	int GetJointsHitboxNextMatricesLoc()const { return -1; }
	int GetHitboxInterpolationLoc()const { return -1; }
	int GetJointsHitboxComputePrevMatricesLoc()const { return -1; }
	int GetJointsHitboxComputeNextMatricesLoc()const { return -1; }
	int GetHitboxComputeInterpolationLoc()const { return -1; }

	void* GetJointsPrevMatBufferPtr(int jointIdx) { return nullptr; }
	void* GetJointsNextMatBufferPtr(int jointIdx) { return nullptr; }
	void* GetJointsHitboxPrevMatBufferPtr(int jointIdx) { return nullptr; }
	void* GetJointsHitboxNextMatBufferPtr(int jointIdx) { return nullptr; }
	void* GetJointsHitboxComputePrevMatBufferPtr(int jointIdx) { return nullptr; }
	void* GetJointsHitboxComputeNextMatBufferPtr(int jointIdx) { return nullptr; }

public:

	void SetVAO(float* verticesBuffer, int verticesCount);
	void SetHitboxVAO(float* verticesBuffer, unsigned* indicesBuffer, int verticesCount);

	void Init();
};