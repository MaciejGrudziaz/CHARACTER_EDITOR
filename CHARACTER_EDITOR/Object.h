#pragma once
#include <GL/glew.h>
#include <Windows.h>
#include <boost/chrono.hpp>
#include <assert.h>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>
#include <map>
#include <set>
#include "BasicObject.h"
#include "ResourcesManager.h"
#include "Shader.h"
#include "FBXloader.h"
#include "ObjectShaderManager.h"
#include "Animation.h"
#include "Hitbox.h"

const std::string g_modelsDirectory = "MODELS/";

class Object {
protected:
	AnimationManager* animationManager;
	BasicObject* basicObject;
	ObjectShaderManager* shaderManager;

	int objectBufferVertexAttribCount;
	int hitboxObjectBufferVertexAttribCount;

	float* vertices;
	unsigned verticesCount;
	float* hitboxVertices;
	float* hitboxOutVertices;
	unsigned hitboxVerticesCount;	//³¹czna liczba zmiennych - liczba wierzcholków * 4(parametry na wierzcho³ek)
	int updateHitboxVerticesInBuffer;

	glm::mat4 model;
	bool modelUpdate;
	glm::mat4 view;
	bool viewUpdate;
	glm::mat4 projection;
	bool projectionUpdate;	

	int textureAvailable;

	typedef std::map<int, Hitbox*> HitboxMap;
	HitboxMap hitboxes;
	Hitbox* mainHitbox;

	typedef std::map<int, std::vector<Hitbox*> > LinkedHitboxesMap;
	LinkedHitboxesMap linkedHitboxes;

	int currentHitboxJointIdx;
	bool mainHitboxChosen;

	void CreateHitboxes_LoadVertices();
	void CreateHitboxes_HitboxCreateAlgorithm();
	glm::vec3 CreateHitboxes_HitboxCreateAlgorithm_FindCenter(const Hitbox* hitbox);
	void CreateHitboxes_HitboxCreateAlgorithm_CalcVectorsFromCenter(const Hitbox* hitbox, glm::vec3 center, std::vector<glm::vec3>& vectors);
	void CreateHitboxes_HitboxCreateAlgorithm_CalcLocalAxis(Hitbox* hitbox, const std::vector<glm::vec3>& vectors, Hitbox::Axis& localAxis);
	void CreateHitboxes_HitboxCreateAlgorithm_CalcLocalAxis_CreateBasicIndexGroups(const std::vector<glm::vec3>& vectors, std::map<int, Hitbox::IndexGroup>& indexes);
	void CreateHitboxes_HitboxCreateAlgorithm_CalcLocalAxis_LinkIndexGroups(std::map<int, Hitbox::IndexGroup>& indexes, std::map<int, Hitbox::IndexGroup>& linkIndexes);
	void CreateHitboxes_HitboxCreateAlgorithm_CalcLocalAxis_CalcNewCoords(const Hitbox* hitbox, std::map<int, Hitbox::IndexGroup>& linkIndexes, std::vector<glm::vec3>& newVertices, std::vector<glm::vec3>& newVectors);
	void CreateHitboxes_HitboxCreateAlgorithm_CalcLocalAxis_CalcAxisY(const std::vector<glm::vec3>& newVertices, const std::vector<glm::vec3>& newVectors, std::pair<int, int>& idxPairAxisY, Hitbox::Axis& localAxis);
	void CreateHitboxes_HitboxCreateAlgorithm_CalcLocalAxis_CalcAxisXZ(const std::vector<glm::vec3>& newVertices, const std::vector<glm::vec3>& newVectors, std::pair<int, int> idxPairAxisY, Hitbox::Axis& localAxis);
	bool CreateHitboxes_HitboxCreateAlgorithm_CalcLocalAxis_CheckIdxGroup(int checkGroup, int currGroup, std::map<int, Hitbox::IndexGroup>& indexes, Hitbox::IndexGroup* currNewIdxGroup);
	void CreateHitboxes_HitboxCreateAlgorithm_CalcMoveInLocalAxis(const std::vector<glm::vec3>& vectors, const Hitbox::Axis& localAxis, Hitbox::MoveValues& axisMoveVal);
	void CreateHitboxes_HitboxCreateAlgorithm_CalcHitboxVertices(glm::vec3 center, const Hitbox::Axis& localAxis, const Hitbox::MoveValues& axisMoveVal, Hitbox* hitbox);

	void CreateHitboxes_CalcMainHitbox();

	void CreateHitboxes_CalcHitboxesNormals();
	glm::vec3 CreateHitboxes_CalcHitboxesNormals_GetNormalFromSurface(glm::vec4& vert1, glm::vec4& vert2, glm::vec4& vert3);

	void WriteErrorToFile(std::string message);

	virtual void UpdateHitboxes() = 0;
	virtual void SetVerticesBuffer()=0;

public:
	Object();
	
	void CreateHitboxes();
	void LoadHitboxBufferData();
	void AddHitbox(Hitbox* hitbox) { hitboxes.insert(HitboxMap::value_type(hitbox->jointIdx, hitbox)); }
	void AddMainHitbox(Hitbox* hitbox) { mainHitbox = hitbox; }

	void LoadShader(Shader* shaderProgram_);

	void LoadHitboxShader(Shader* hitboxBasicShaderProgram_);
	void LoadHitboxComputeShader(Shader* hitboxComputeShaderProgram);

	int LoadTexture(std::string filename);

	int StartAnimation(int animationIndex_);
	int StopAnimation(int animationIndex_);

	void SetBasicObject(BasicObject* basicObject_) { basicObject = basicObject_; /*LoadHitboxes();*/ }

	glm::mat4 GetModelMatrix()const { return model; }
	glm::mat4 GetViewMatrix()const { return view; }
	glm::mat4 GetProjectionMatrix()const { return projection; }

	ObjectShaderManager* GetShaderManager() { return shaderManager; }
	AnimationManager* GetAnimationManager() { return animationManager; }

	const float* GetHitboxOutVertices()const { return hitboxOutVertices; }
	const float* GetHitboxInVertices()const { return hitboxVertices; }
	unsigned GetHitboxVerticesBufferSize()const { return hitboxVerticesCount; }

	void ChangeHitboxCoords(int hitboxJointIdx,glm::vec4 newVerticesCoords[]);
	void LinkHitboxes(int parentHitboxJointIdx, std::vector<int>& childHitboxJointIdx);
	void ResetHitboxVertices(int hitboxJointIdx);
	void UnlinkHitboxes(int parentHitboxJointIdx);

	void SetModelMatrix(glm::mat4 model_) { model = model_; modelUpdate = true; }
	void SetViewMatrix(glm::mat4 view_) { view = view_; viewUpdate = true; }
	void SetProjectionMatrix(glm::mat4 projection_) { projection = projection_; projectionUpdate = true; }

	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

	virtual ~Object() {
		delete[]vertices;
		delete[]hitboxVertices;
		delete[]hitboxOutVertices;
		delete shaderManager;
		delete animationManager;
		delete mainHitbox;

		for (HitboxMap::iterator it = hitboxes.begin(); it != hitboxes.end(); ++it)
			delete it->second;
		hitboxes.clear();
		for (std::map<int, std::vector<Hitbox*> >::iterator it = linkedHitboxes.begin(); it != linkedHitboxes.end(); ++it)
			for (Hitbox* hitbox : it->second)
				delete hitbox;
		linkedHitboxes.clear();
	}

	//----------------------------FOR CONSOLE EDIT-----------------------------
	BasicObject* GetBasicObject() { return basicObject; }
	Hitbox* GetHitbox(int idx) { 
		if (idx >= 0 && idx < hitboxes.size()) {
			HitboxMap::iterator it = (hitboxes.find(idx));
			if (it != hitboxes.end()) return it->second;
			else return nullptr;
		}
		else if (idx == -1) return mainHitbox;
		else return nullptr;
	}
	Hitbox* GetMainHitbox() { return mainHitbox; }
	int GetHitboxCount()const { return hitboxes.size(); }
	void SetCurrentHitboxJointIdx(int idx) { currentHitboxJointIdx = idx; }
	void SetCurrentMainHitbox() { currentHitboxJointIdx = -1; mainHitboxChosen = true; }
	void ResetCurrentMainHitbox() { mainHitboxChosen = false; currentHitboxJointIdx = -2; }
	virtual HitboxAxisShader* GetHitboxAxisShader() = 0;
	//-------------------------------------------------------------------------
};

class DynamicObject :public Object {
	void SetVerticesBuffer();

	void UpdateHitboxes();

public:
	DynamicObject() { shaderManager = new DynamicObjectShaderManager(); }

	void Init();

	void Update();

	void Draw();

	//----------------------------FOR CONSOLE EDIT-----------------------------
	HitboxAxisShader* GetHitboxAxisShader() { return new DynamicAxisHitboxShader(); }
	//-------------------------------------------------------------------------
};

class StaticObject :public Object {
	void SetVerticesBuffer();

	void UpdateHitboxes();

public:
	StaticObject() { shaderManager = new StaticObjectShaderManager(); }

	void Init();

	void Update();

	void Draw();

	//----------------------------FOR CONSOLE EDIT-----------------------------
	HitboxAxisShader* GetHitboxAxisShader() { return new StaticAxisHitboxShader(); }
	//-------------------------------------------------------------------------
};