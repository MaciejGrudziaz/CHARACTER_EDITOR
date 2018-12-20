#pragma once
#include <boost/chrono.hpp>
#include <boost/thread.hpp>
#include <queue>
#include "WinAPIwindowManager.h"
#include "Model.h"
#include "CameraManager.h"
#include "CharacterManager.h"
#include "BasicModelManager.h"
#include "ResourcesManager.h"
#include "ShaderManager.h"

struct BasicShaderStruct {
	int shaderProgram;
	int projectionMatLoc;
	int viewMatLoc;
	bool render;

	BasicShaderStruct() :shaderProgram(-1), projectionMatLoc(-1), viewMatLoc(-1), render(true){}
};

class Graphics {
	static BasicShaderStruct ground;
	static BasicShaderStruct axis;
	static std::atomic<int> modelLoading;
	static std::string modelFilename;

	static int currModelIdx;
	static int currObjectIdx;
	static int currHitboxJointIdx;
	static int currAnimationIdx;

	static HitboxAxisShader* hitboxAxis;

	static void DrawAxis();
	static void DrawGround();

	static void LoadBasicShaderStructs();
	static void LoadModel_GL(const char* filename);
	static void LoadHitboxAxisShader();

	static void DeleteModel_GL();

	static void Sync();

public:
	static void SetGroundVisibility(bool visibility) { ground.render = visibility; }
	static void SetAxisVisibility(bool visibility) { axis.render = visibility; }
	static void SetModelSemafor() { ++modelLoading; }
	static void ResetModelSemafor() { --modelLoading; }
	static int GetModelSemafor() { return modelLoading; }
	static void LoadModel(std::string filename) { modelFilename = filename; SetModelSemafor(); }
	static void DeleteModel() { modelLoading = 3; }

	static void SetCurrModelIdx(int idx) { 
		if (idx >= 0 && idx < CharacterManager::GetCharactersCount()) currModelIdx = idx; 
	}
	static void SetCurrObjectIdx(int idx) { if (idx >= 0 && idx < CharacterManager::GetCharacter(currModelIdx)->GetObjectsCount()) currObjectIdx = idx; }
	static void SetCurrHitboxJointIdx(int idx);
	static void SetCurrentAnimationIndx(int idx);
	static int GetCurrModelIdx() { return currModelIdx; }
	static int GetCurrObjectIdx() { return currObjectIdx; }
	static int GetCurrHitboxJointIdx() { return currHitboxJointIdx; }
	static int GetCurrAnimationIdx() { return currAnimationIdx; }

	static void Init();
	static void Process();
	static void End();
};