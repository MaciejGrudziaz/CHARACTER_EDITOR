#include "Graphics.h"

BasicShaderStruct Graphics::ground;
BasicShaderStruct Graphics::axis;
std::atomic<int> Graphics::modelLoading(0);
std::string Graphics::modelFilename;
int Graphics::currModelIdx=-1;
int Graphics::currObjectIdx = -1;
int Graphics::currHitboxJointIdx=-2;
int Graphics::currAnimationIdx = -1;
//ChooseMainHitboxData Graphics::currMainHitbox;
bool Graphics::currMainHitbox = false;
HitboxAxisShader* Graphics::hitboxAxis=nullptr;

void Graphics::Init() {
	WinAPIwindowManager::RegisterWindow(new WinAPIwindow("MAIN", 1280, 720));

	Camera* camera=CameraManager::RegisterCamera(new Camera(glm::vec3(0.0f, 7.5f, 20.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

	ShaderManager::RegisterShader(new Shader("Ground","SHADERS/groundShader.vert", "SHADERS/groundShader.frag"));
	ShaderManager::RegisterShader(new Shader("Axis", "SHADERS/axisShader.vert", "SHADERS/axisShader.frag"));
	ShaderManager::RegisterShader(new Shader("DynamicObj","SHADERS/shader1_Dynamic.vert", "SHADERS/shader1_Dynamic.frag"));
	ShaderManager::RegisterShader(new Shader("StaticObj","SHADERS/shader1_Static.vert", "SHADERS/shader1_Static.frag"));
	ShaderManager::RegisterShader(new Shader("DynamicHitbox","SHADERS/shader2_Dynamic.vert", "SHADERS/shader2_Dynamic.frag"));
	ShaderManager::RegisterShader(new Shader("StaticHitbox", "SHADERS/shader2_Static.vert", "SHADERS/shader2_Static.frag"));
	ShaderManager::RegisterShader(new Shader("HitboxCompute","SHADERS/hitboxComputeShader.comp"));
	ShaderManager::RegisterShader(new Shader("DynamicHitboxAxis", "SHADERS/hitboxAxisShader_Dynamic.vert", "SHADERS/hitboxAxisShader.frag"));
	ShaderManager::RegisterShader(new Shader("StaticHitboxAxis", "SHADERS/hitboxAxisShader_Static.vert", "SHADERS/hitboxAxisShader.frag"));


	LoadBasicShaderStructs();

	//glm::mat4 projection = WinAPIwindowManager::GetWindow("MAIN")->GetProjectionMatrix();

	//BasicModel* basicModel=BasicModelManager::RegisterModel("MODELS/basic_character2_5_1_4.fbx");

	//Shader *dynSh, *statSh, *hitSh, *compSh;
	//dynSh=statSh=hitSh=compSh = nullptr;
	//dynSh=ShaderManager::RegisterShader(new Shader("objDyn","SHADERS/shader1_Dynamic.vert", "SHADERS/shader1_Dynamic.frag"));
	//statSh=ShaderManager::RegisterShader(new Shader("objStat","SHADERS/shader1_Static.vert", "SHADERS/shader1_Static.frag"));
	//hitSh=ShaderManager::RegisterShader(new Shader("hitDyn","SHADERS/shader2_Dynamic.vert", "SHADERS/shader2_Dynamic.frag"));
	//compSh=ShaderManager::RegisterShader(new Shader("hitComp","SHADERS/hitboxComputeShader.comp"));

	//shader1 = ShaderManager::GetShader(0);
	//shader1->SetName("Animation shader");
	//shader2 = ShaderManager::GetShader(1);
	//shader2->SetName("Static shader");
	//hitboxShader = ShaderManager::GetShader(2);
	//hitboxShader->SetName("Hitbox shader");
	//hitboxComputeShader = ShaderManager::GetShader(3);
	//hitboxComputeShader->SetName("Hitbox compute shader");

	//Character* character = nullptr;
	//character = CharacterManager::RegisterCharacter(new Character("TEST"));
	//character->InitModel(basicModel);
	//character->LoadShaderProgramToAll(dynSh);
	////character->LoadHitboxShaderPrograms(hitboxShader, hitboxComputeShader);
	//character->LoadHitboxComputeShaderProgram(compSh);
	//character->LoadHitboxShaderProgramToAll(hitSh);
	//character->Init();

	//character->SetViewMatrix(camera->GetView());
	//character->SetProjectionMatrix(projection);
	//character->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	//character->StartAnimation(1);

	//character = CharacterManager::RegisterCharacter(new Character("TEST2"));
	//character->InitModel(basicModel);
	//character->LoadShaderProgramToAll(shader1);
	//character->LoadHitboxShaderPrograms(hitboxShader, hitboxComputeShader);
	//character->Init();

	//character->SetViewMatrix(camera->GetView());
	//character->SetProjectionMatrix(projection);
	//character->SetPosition(glm::vec3(-7.5f, 0.0f, 0.0f));
	//character->StartAnimation(0);

	//basicModel = BasicModelManager::RegisterModel("MODELS/multi_model_no_texture.fbx");

	//character = CharacterManager::RegisterCharacter(new Character("MAP"));
	//character->InitModel(basicModel);
	//character->LoadShaderProgramToAll(shader2);
	//character->LoadHitboxShaderPrograms(hitboxShader,hitboxComputeShader);
	//character->Init();

	//character->SetViewMatrix(camera->GetView());
	//character->SetProjectionMatrix(projection);

	ResourcesManager::InitTimer();

	glEnable(GL_DEPTH_TEST);
}

void Graphics::Process() {
	ResourcesManager::StopFrameTimeMeasurment();
	ResourcesManager::StartFrameTimeMeasurment();

	int frameCount = ResourcesManager::GetFrameCount();

	for (unsigned i = 0; i < frameCount; ++i) {
		//CameraManager::GetCamera(0)->Process();

		if (modelLoading.load() <= 0) {
			for (unsigned i = 0; i < CharacterManager::GetCharactersCount(); ++i) {
				CharacterManager::GetCharacter(i)->Update();
				//CharacterManager::GetCharacter(i)->SetViewMatrix(CameraManager::GetCamera(0)->GetView());
			}
		}
	}

	if (modelLoading.load() <= 0) {
		//Sync();

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		CameraManager::GetCamera(0)->Process();

		for (unsigned i = 0; i < CharacterManager::GetCharactersCount(); ++i) {
			CharacterManager::GetCharacter(i)->SetViewMatrix(CameraManager::GetCamera(0)->GetView());
			CharacterManager::GetCharacter(i)->Draw();
		}

		if (ground.render == true) DrawGround();
		if (axis.render == true) {
			glDisable(GL_DEPTH_TEST);
			DrawAxis();
			glEnable(GL_DEPTH_TEST);
		}

		//glDisable(GL_DEPTH_TEST);
		//DrawTestAxis();
		//glEnable(GL_DEPTH_TEST);

		if (hitboxAxis != nullptr) {
			if (currMainHitbox == false) {
				if (CharacterManager::GetCharacter(currModelIdx)->GetModel()->GetObject_(currObjectIdx)->GetAnimationManager() != nullptr)
					hitboxAxis->Update(CharacterManager::GetCharacter(currModelIdx)->GetModel()->GetObject_(currObjectIdx)->GetModelMatrix(), CameraManager::GetCamera(0)->GetView(),
						CharacterManager::GetCharacter(currModelIdx)->GetModel()->GetObject_(currObjectIdx)->GetAnimationManager()->GetInterpolationVal());
				else hitboxAxis->Update(CharacterManager::GetCharacter(currModelIdx)->GetModel()->GetObject_(currObjectIdx)->GetModelMatrix(),
					CameraManager::GetCamera(0)->GetView(), 0.0f);
			}
			else {
				if (CharacterManager::GetCharacter(currModelIdx)->GetModel()->GetObject_(currObjectIdx)->GetAnimationManager() != nullptr)
					hitboxAxis->Update(CharacterManager::GetCharacter(currModelIdx)->GetModel()->GetObject_(currObjectIdx)->GetModelMatrix(),
						CameraManager::GetCamera(0)->GetView(),
						CharacterManager::GetCharacter(currModelIdx)->GetModel()->GetObject_(currObjectIdx)->GetAnimationManager()->GetInterpolationVal());
				else hitboxAxis->Update(CharacterManager::GetCharacter(currModelIdx)->GetModel()->GetObject_(currObjectIdx)->GetModelMatrix(),
					CameraManager::GetCamera(0)->GetView(), 0.0f);
			}

			glDisable(GL_DEPTH_TEST);
			hitboxAxis->Draw();
			glEnable(GL_DEPTH_TEST);
		}
	}
	else {
		//Sync();
		if (modelLoading == 1) {
			LoadModel_GL(modelFilename.c_str());
			modelFilename = "";
			ResetModelSemafor();
		}
		else if (modelLoading == 2) {
			LoadHitboxAxisShader();
			modelLoading = 0;
		}
		else if (modelLoading == 3) {
			DeleteModel_GL();
			modelLoading = 0;
		}
	}
}

void Graphics::End() {
	ResourcesManager::WriteFPSToFile();
}

//-----------------------------------------------------------------------------
//-----------------------------------PRIVATE-----------------------------------
//-----------------------------------------------------------------------------

void Graphics::DrawAxis() {
	glUseProgram(axis.shaderProgram);
	glUniformMatrix4fv(axis.viewMatLoc, 1, GL_FALSE, glm::value_ptr(CameraManager::GetCamera(0)->GetView()));
	glUniform4f(glGetUniformLocation(axis.shaderProgram, "begin"), 0.0f, 0.0f, 0.0f, 1.0f);
	glDrawArrays(GL_LINES, 0, 6);
}

void Graphics::DrawGround() {
	glUseProgram(ground.shaderProgram);
	glUniformMatrix4fv(ground.viewMatLoc, 1, GL_FALSE, glm::value_ptr(CameraManager::GetCamera(0)->GetView()));
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

//void Graphics::DrawTestAxis() {
//	if (CharacterManager::GetCharactersCount() > 0) {
//		glm::vec4 jointBindPos = CharacterManager::GetCharacter(0)->GetModel()->GetObject_(0)->GetBasicObject()->skeleton.joints[4]->bindPos;
//
//		glUseProgram(axis.shaderProgram);
//		glUniformMatrix4fv(axis.viewMatLoc, 1, GL_FALSE, glm::value_ptr(CameraManager::GetCamera(0)->GetView()));
//		glUniform4fv(glGetUniformLocation(axis.shaderProgram, "begin"), 1, glm::value_ptr(jointBindPos));
//		glDrawArrays(GL_LINES, 0, 6);
//
//		jointBindPos = CharacterManager::GetCharacter(0)->GetModel()->GetObject_(0)->GetBasicObject()->skeleton.joints[5]->bindPos;
//		glUniform4fv(glGetUniformLocation(axis.shaderProgram, "begin"), 1, glm::value_ptr(jointBindPos));
//		glDrawArrays(GL_LINES, 0, 6);
//	}
//}

void Graphics::LoadBasicShaderStructs() {
	ground.shaderProgram = ShaderManager::GetShader("Ground")->GetProgram();
	axis.shaderProgram = ShaderManager::GetShader("Axis")->GetProgram();

	ground.projectionMatLoc = glGetUniformLocation(ground.shaderProgram, "projection");
	ground.viewMatLoc = glGetUniformLocation(ground.shaderProgram, "view");

	axis.projectionMatLoc = glGetUniformLocation(axis.shaderProgram, "projection");
	axis.viewMatLoc = glGetUniformLocation(axis.shaderProgram, "view");

	glUseProgram(ground.shaderProgram);
	glUniformMatrix4fv(ground.projectionMatLoc, 1, GL_FALSE, glm::value_ptr(WinAPIwindowManager::GetMainWindow()->GetProjectionMatrix()));
	glUseProgram(axis.shaderProgram);
	glUniformMatrix4fv(axis.projectionMatLoc, 1, GL_FALSE, glm::value_ptr(WinAPIwindowManager::GetMainWindow()->GetProjectionMatrix()));
}

void Graphics::LoadModel_GL(const char* filename) {
	Shader *modelStatic, *modelDynamic, *hitboxStatic, *hitboxDynamic, *hitboxCompute;
	modelStatic = ShaderManager::GetShader("StaticObj");
	modelDynamic = ShaderManager::GetShader("DynamicObj");
	hitboxStatic = ShaderManager::GetShader("StaticHitbox");
	hitboxDynamic = ShaderManager::GetShader("DynamicHitbox");
	hitboxCompute = ShaderManager::GetShader("HitboxCompute");

	if (CharacterManager::GetCharactersCount() > 0)
		CharacterManager::DestroyCharacter(0);
	if (BasicModelManager::GetModelsCount() > 0)
		BasicModelManager::DestroyModel(0);

	//bool isDynamic = false;

	BasicModel* basicModel = BasicModelManager::RegisterModel(filename);
	if (basicModel != nullptr) {
		Character* character = nullptr;
		character = CharacterManager::RegisterCharacter(new Character("TEST"));
		character->InitModel(basicModel);
		
		std::string filenameStr(filename);
		for (int i = 0; i < character->GetObjectsCount(); ++i) {
			if (character->IsObjectDynamic(i)) {
				character->LoadObjectShaderProgram(i, modelDynamic);
				character->LoadObjectHitboxShaderProgram(i, hitboxDynamic);
			}
			else {
				character->LoadObjectShaderProgram(i, modelStatic);
				character->LoadObjectHitboxShaderProgram(i, hitboxStatic);
			}
			if (strcmp(filename + (filenameStr.size() - 4), ".mgr") == 0) {
				character->SetScaleMat(ImportFile::GetScaleMat());
				if (ImportFile::ImportedHitboxesAvailableForObject(i) == true) {
					character->GetModel()->GetObject_(i)->AddMainHitbox(ImportFile::GetMainHitbox(i));

					for (int j = 0; j < ImportFile::GetHitboxCount(i); ++j) {
						Hitbox* hitbox = ImportFile::GetHitbox(i, j);
						if (hitbox != nullptr)
							character->GetModel()->GetObject_(i)->AddHitbox(hitbox);
					}
					character->GetModel()->GetObject_(i)->LoadHitboxBufferData();
				}
				else character->GetModel()->GetObject_(i)->CreateHitboxes();
			}
			else character->GetModel()->GetObject_(i)->CreateHitboxes();
		}
		character->LoadHitboxComputeShaderProgram(hitboxCompute);
		character->Init();

		character->SetProjectionMatrix(WinAPIwindowManager::GetMainWindow()->GetProjectionMatrix());
		character->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));

		currModelIdx = currObjectIdx = currHitboxJointIdx = 0;
	}
}

void Graphics::LoadHitboxAxisShader() {
	if (hitboxAxis != nullptr) {
		delete hitboxAxis;
		hitboxAxis = nullptr;
	}

	if (!currMainHitbox) {
		if (currHitboxJointIdx >= 0) {
			Object* obj = CharacterManager::GetCharacter(currModelIdx)->GetModel()->GetObject_(currObjectIdx);
			//delete hitboxAxis;
			hitboxAxis = obj->GetHitboxAxisShader();

			if (dynamic_cast<DynamicAxisHitboxShader*>(hitboxAxis) != nullptr) {
				hitboxAxis->Init(ShaderManager::GetShader("DynamicHitboxAxis"), WinAPIwindowManager::GetMainWindow()->GetProjectionMatrix(), currHitboxJointIdx);
				hitboxAxis->SetNextJointsMatBuffer(obj->GetShaderManager()->GetJointsNextMatricesBuffer());
				hitboxAxis->SetPrevJointsMatBuffer(obj->GetShaderManager()->GetJointsPrevMatricesBuffer());
				//hitboxAxis->SetTransformedHitboxVerticesBuffer(obj->GetShaderManager()->GetHitboxComputeOutBuffer());
				hitboxAxis->LoadHitboxBufferData(obj->GetHitboxInVertices(), obj->GetHitboxVerticesBufferSize());
				hitboxAxis->LoadLocalAxisBuffer(obj->GetHitbox(currHitboxJointIdx)->localAxis);
			}
			else {
				hitboxAxis->Init(ShaderManager::GetShader("StaticHitboxAxis"), WinAPIwindowManager::GetMainWindow()->GetProjectionMatrix(), currHitboxJointIdx);
				//hitboxAxis->SetTransformedHitboxVerticesBuffer(obj->GetShaderManager()->GetHitboxComputeOutBuffer());
				hitboxAxis->LoadHitboxBufferData(obj->GetHitboxInVertices(), obj->GetHitboxVerticesBufferSize());
				hitboxAxis->LoadLocalAxisBuffer(obj->GetHitbox(currHitboxJointIdx)->localAxis);
			}
		}
	}
	else {
		Object* obj = CharacterManager::GetCharacter(currModelIdx)->GetModel()->GetObject_(currObjectIdx);
		delete hitboxAxis;
		hitboxAxis = obj->GetHitboxAxisShader();
		if (dynamic_cast<DynamicAxisHitboxShader*>(hitboxAxis) != nullptr) {
			hitboxAxis->Init(ShaderManager::GetShader("DynamicHitboxAxis"), WinAPIwindowManager::GetMainWindow()->GetProjectionMatrix(), -1);
			hitboxAxis->SetNextJointsMatBuffer(obj->GetShaderManager()->GetJointsNextMatricesBuffer());
			hitboxAxis->SetPrevJointsMatBuffer(obj->GetShaderManager()->GetJointsPrevMatricesBuffer());
			hitboxAxis->LoadHitboxBufferData(obj->GetHitboxInVertices(), obj->GetHitboxVerticesBufferSize());
			hitboxAxis->LoadLocalAxisBuffer(obj->GetMainHitbox()->localAxis);
		}
		else {
			hitboxAxis->Init(ShaderManager::GetShader("StaticHitboxAxis"), WinAPIwindowManager::GetMainWindow()->GetProjectionMatrix(), -1);
			hitboxAxis->LoadHitboxBufferData(obj->GetHitboxInVertices(), obj->GetHitboxVerticesBufferSize());
			hitboxAxis->LoadLocalAxisBuffer(obj->GetMainHitbox()->localAxis);
		}
	}
}

void Graphics::SetCurrModelIdx(int idx) {
	if (idx >= 0 && idx < CharacterManager::GetCharactersCount()) {
		if (currMainHitbox == false) {
			if (currHitboxJointIdx >= 0)
				CharacterManager::GetCharacter(currModelIdx)->GetModel()->GetObject_(currObjectIdx)->SetCurrentHitboxJointIdx(-1);

			currModelIdx = idx;
			currObjectIdx = -1;
			currHitboxJointIdx = -1;
		}
		else {
			CharacterManager::GetCharacter(currModelIdx)->GetModel()->GetObject_(currObjectIdx)->ResetCurrentMainHitbox();

			currModelIdx = idx;
			currObjectIdx = -1;
			currHitboxJointIdx = -1;
			currMainHitbox = false;
		}
	}

	modelLoading = 2;
}

void Graphics::SetCurrObjectIdx(int idx) {
	if (idx >= 0 && idx < CharacterManager::GetCharacter(currModelIdx)->GetObjectsCount()) {
		if (currMainHitbox == false) {
			if (currHitboxJointIdx >= 0)
				CharacterManager::GetCharacter(currModelIdx)->GetModel()->GetObject_(currObjectIdx)->SetCurrentHitboxJointIdx(-1);

			currObjectIdx = idx;
			currHitboxJointIdx = -1;
		}
		else {
			CharacterManager::GetCharacter(currModelIdx)->GetModel()->GetObject_(currObjectIdx)->ResetCurrentMainHitbox();

			currObjectIdx = idx;
			currHitboxJointIdx = -1;
			currMainHitbox = false;
		}
	}

	modelLoading = 2;
}

void Graphics::SetCurrHitboxJointIdx(int idx) {
	if (idx >= 0 && idx < CharacterManager::GetCharacter(currModelIdx)->GetModel()->GetObject_(currObjectIdx)->GetBasicObject()->skeleton.joints.size()) {
		if (currMainHitbox == false) {
			if (currHitboxJointIdx >= 0)
				CharacterManager::GetCharacter(currModelIdx)->GetModel()->GetObject_(currObjectIdx)->SetCurrentHitboxJointIdx(-1);

			currHitboxJointIdx = idx;
		}
		else {
			CharacterManager::GetCharacter(currModelIdx)->GetModel()->GetObject_(currObjectIdx)->ResetCurrentMainHitbox();

			currHitboxJointIdx = idx;
			currMainHitbox = false;
		}

		CharacterManager::GetCharacter(currModelIdx)->SetCurrentJointIdx(currObjectIdx, currHitboxJointIdx);
	}
	
	modelLoading = 2;
}

void Graphics::SetCurrMainHitbox() {
	if (currHitboxJointIdx >= 0)
		CharacterManager::GetCharacter(currModelIdx)->GetModel()->GetObject_(currObjectIdx)->SetCurrentHitboxJointIdx(-1);

	currHitboxJointIdx = -1;
	currMainHitbox = true;

	CharacterManager::GetCharacter(currModelIdx)->GetModel()->GetObject_(currObjectIdx)->SetCurrentMainHitbox();

	modelLoading = 2;
}

void Graphics::SetCurrentAnimationIndx(int idx) {
	if (idx >= 0 && idx < CharacterManager::GetCharacter(currModelIdx)->GetModel()->GetObject_(currObjectIdx)->GetBasicObject()->animationsInfo.size()) {
		currAnimationIdx = idx;
	}
}

void Graphics::DeleteModel_GL() {
	if (CharacterManager::GetCharactersCount() > 0)
		CharacterManager::DestroyCharacter(0);
	if (BasicModelManager::GetModelsCount() > 0)
		BasicModelManager::DestroyModel(0);

	currModelIdx = -1;
	currObjectIdx = -1;
	currHitboxJointIdx = -1;
	currAnimationIdx = -1;
	if (hitboxAxis != nullptr) {
		delete hitboxAxis;
		hitboxAxis = nullptr;
	}
}

void Graphics::Sync() {
	GLsync sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
	GLint result = GL_UNSIGNALED;
	glGetSynciv(sync, GL_SYNC_STATUS, sizeof(GLint), NULL, &result);
	while (result != GL_SIGNALED) {
		glGetSynciv(sync, GL_SYNC_STATUS, sizeof(GLint), NULL, &result);
	}
	glDeleteSync(sync);
}
