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

struct ChooseMainHitboxData {
	int modelIdx;
	int objectIdx;
	bool mainHitboxChoosen;

	ChooseMainHitboxData() :modelIdx(-1), objectIdx(-1), mainHitboxChoosen(false) {}
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
	static bool currMainHitbox;
	//static ChooseMainHitboxData currMainHitbox;

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

	static void SetCurrModelIdx(int idx);
	static void SetCurrObjectIdx(int idx);
	static void SetCurrHitboxJointIdx(int idx);
	static void SetCurrMainHitbox();
	static void SetCurrentAnimationIndx(int idx);
	static int GetCurrModelIdx() { return currModelIdx; }
	static int GetCurrObjectIdx() { return currObjectIdx; }
	static int GetCurrHitboxJointIdx() { return currHitboxJointIdx; }
	static int GetCurrAnimationIdx() { return currAnimationIdx; }
	static bool GetCurrMainHitboxChosen() { return currMainHitbox; }

	static void Init();
	static void Process();
	static void End();
};