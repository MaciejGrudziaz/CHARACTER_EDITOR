#include "ConsoleCommand.h"
#include "ExportFile.h"

bool CheckIfStringIsInt(std::string str) {
	for (int i = 0; i < str.size(); ++i) {
		if ((int)str[i] < 48 || (int)str[i]>57) {
			return false;
		}
	}

	return true;
}

bool CheckIfStringIsFloat(std::string str) {
	for (int i = 0; i < str.size(); ++i) {
		if (((int)str[i] < 48 || (int)str[i]>57) && (int)str[i] != 46 && (int)str[i] != 45) {
			return false;
		}
	}

	return true;
}

bool CheckCurrModelIdx() {
	int modelIdx = Graphics::GetCurrModelIdx();

	if (modelIdx < 0 || modelIdx >= CharacterManager::GetCharactersCount()) {
		std::cout << "Wrong model index!\nchoosen model: " << modelIdx << std::endl;
		return false;
	}

	return true;
}

bool CheckCurrObjectIdx() {
	int modelIdx = Graphics::GetCurrModelIdx();
	int objectIdx = Graphics::GetCurrObjectIdx();

	if (modelIdx < 0 || modelIdx >= CharacterManager::GetCharactersCount()) {
		std::cout << "Wrong model index!\nchoosen model: " << modelIdx << std::endl;
		return false;
	}
	else if (objectIdx < 0 || objectIdx >= CharacterManager::GetCharacter(modelIdx)->GetObjectsCount()) {
		std::cout << "Wrong object index!\nchoosen object: " << objectIdx << std::endl;
		return false;
	}

	return true;
}

bool CheckCurrHitboxIdx() {
	int modelIdx = Graphics::GetCurrModelIdx();
	int objectIdx = Graphics::GetCurrObjectIdx();
	int hitboxJointIdx = Graphics::GetCurrHitboxJointIdx();

	if (modelIdx < 0 || modelIdx >= CharacterManager::GetCharactersCount()) {
		std::cout << "Wrong model index!\nchoosen model: " << modelIdx << std::endl;
		return false;
	}
	else if (objectIdx < 0 || objectIdx >= CharacterManager::GetCharacter(modelIdx)->GetObjectsCount()) {
		std::cout << "Wrong object index!\nchoosen object: " << objectIdx << std::endl;
		return false;
	}
	else if (hitboxJointIdx < 0 || CharacterManager::GetCharacter(modelIdx)->GetModel()->GetObject_(objectIdx)->GetHitbox(hitboxJointIdx) == nullptr) {
		std::cout << "Wrong hitbox joint index!\nchoosen hitbox joint: " << hitboxJointIdx << std::endl;
		return false;
	}

	return true;
}

bool CheckCurrAnimationIdx() {
	int modelIdx = Graphics::GetCurrModelIdx();
	int objectIdx = Graphics::GetCurrObjectIdx();
	int animationIdx = Graphics::GetCurrAnimationIdx();

	if (modelIdx < 0 || modelIdx >= CharacterManager::GetCharactersCount()) {
		std::cout << "Wrong model index!\nchoosen model: " << modelIdx << std::endl;
		return false;
	}
	else if (objectIdx < 0 || objectIdx >= CharacterManager::GetCharacter(modelIdx)->GetObjectsCount()) {
		std::cout << "Wrong object index!\nchoosen object: " << objectIdx << std::endl;
		return false;
	}
	else if (animationIdx < 0 || animationIdx >= CharacterManager::GetCharacter(modelIdx)->GetModel()->GetObject_(objectIdx)->GetBasicObject()->animationsInfo.size()) {
		std::cout << "Wrong animtion index!\nchoosen animtion: " << animationIdx << std::endl;
		return false;
	}

	return true;
}

void LoadModel::Process() {
	std::string in;

	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	std::cout << "Model filename:\n>> ";
	std::cin >> in;

	if (strcmp(in.c_str() + (in.size() - 4), ".fbx") == 0 || strcmp(in.c_str() + (in.size() - 4), ".mgr") == 0) {
		FILE* file;
		fopen_s(&file, in.c_str(), "r");
		if (file == nullptr) std::cout << "File does not exist!\n";
		else {
			fclose(file);
			std::cout << "Loading model!\n";

			Graphics::LoadModel(in);
			while (Graphics::GetModelSemafor() > 0) {}

			int errorCode;
			if (strcmp(in.c_str() + (in.size() - 4), ".mgr") == 0) {
				errorCode = ImportFile::GetErrorFlag();
				if (errorCode == 0)
					std::cout << "Model loaded!\n";
				else {
					std::cout << "Model loading failed!\n";
					if (errorCode == 1)
						std::cout << "Model file is not a .mgr file!\n";
					else if (errorCode == 2)
						std::cout << "Couldn't open file!\n";
					else if (errorCode == 3)
						std::cout << "File is corrupted!\n";
				}
			}
			else {
				errorCode = FBXloader::GetInstance()->GetErrorCode();
				if (errorCode==0)
					std::cout << "Model loaded!\n";
				else std::cout << "Can't load model!\n";
			}
		}
	}
	else std::cout << "Can't read this file! Available extensions: .fbx , .mgr\n";
}

void ShowModels::Process() {
	if (CharacterManager::GetCharactersCount() > 0) {
		for (int i = 0; i < CharacterManager::GetCharactersCount(); ++i) {
			std::cout << i << ". " << CharacterManager::GetCharacter(i)->GetName() << std::endl;
		}
	}
	else std::cout << "No models loaded!\n";
}

void ChooseModel::Process() {
	std::string in;

	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	std::cout << "Model idx: ";
	std::cin >> in;
	if (CheckIfStringIsInt(in)) {
		int inVal = atoi(in.c_str());
		if (inVal < 0 || inVal >= CharacterManager::GetCharactersCount())
			std::cout << "Wrong index!\n";
		else Graphics::SetCurrModelIdx(inVal);
	}
	else std::cout << "Wrong value!\n";
}

void ShowCurrentModel::Process() {
	int idx = Graphics::GetCurrModelIdx();
	if (idx < 0) std::cout << "No model available!\n";
	else std::cout << "Current model: " << idx << ". " << CharacterManager::GetCharacter(idx)->GetName() << std::endl;
}

void ShowHitboxesData::Process() {
	if (CharacterManager::GetCharactersCount() > 0) {
		if (Graphics::GetCurrModelIdx() >= 0 && Graphics::GetCurrModelIdx() < CharacterManager::GetCharactersCount()) {
			int modelIdx = Graphics::GetCurrModelIdx();
			Model* model = CharacterManager::GetCharacter(modelIdx)->GetModel();
			for (int i = 0; i < model->GetObjectsCount(); ++i) {
				Object* obj = model->GetObject_(i);
				BasicObject* basicObj = obj->GetBasicObject();
				std::cout << "Object: " << i << ". " << basicObj->name << std::endl;
				for (int j = 0; j < basicObj->skeleton.joints.size(); ++j) {
					if (obj->GetHitbox(j) != nullptr) 
						std::cout << CON::identation << j << ". " << basicObj->skeleton.joints[j]->name << std::endl;
				}
			}
		}
		else std::cout << "No model choosen!\n";
	}
	else std::cout << "No models loaded!\n";
}

void ChooseHitbox::Process() {
	int modelIdx = Graphics::GetCurrModelIdx();
	int objectIdx = Graphics::GetCurrObjectIdx();
	if (modelIdx < 0)
		std::cout << "No model chosen! Use choose_model function first.\n";
	else if (objectIdx < 0)
		std::cout << "No object chosen! Use choose_object function first!\n";
	else {
		std::string in;
		int idx1,idx2;

		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::cout << "Hitbox index: ";
		std::cin >> in;
		if (CheckIfStringIsInt(in)) {
			//idx1 = atoi(in.c_str());
			//if (idx1 < 0 || idx1 >= CharacterManager::GetCharacter(modelIdx)->GetObjectsCount()) {
			//	std::cout << "Wrong object index!\n";
			//	return;
			//}
			//Graphics::SetCurrObjectIdx(idx1);
			//std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			//std::cout << "Hitbox index: ";
			//std::cin >> in;

			//if(CheckIfStringIsInt(in)) {
				idx2 = atoi(in.c_str());
				if (CharacterManager::GetCharacter(modelIdx)->GetModel()->GetObject_(objectIdx)->GetHitbox(idx2) == nullptr) {
					std::cout << "Wrong hitbox index!\n";
					return;
				}
				Graphics::SetCurrHitboxJointIdx(idx2);
				CharacterManager::GetCharacter(modelIdx)->SetCurrentJointIdx(objectIdx, idx2);
			//}
			//else std::cout << "Wrong value!\n";
		}
		else std::cout << "Wrong value!\n";
	}
}

void ChooseMainHitbox::Process() {
	if (CheckCurrModelIdx()) {
		if(CheckCurrObjectIdx())
			Graphics::SetCurrMainHitbox();
	}
}

void ShowCurrentHitbox::Process() {
	int objIdx = Graphics::GetCurrObjectIdx();
	int hitboxIdx = Graphics::GetCurrHitboxJointIdx();
	int modelIdx = Graphics::GetCurrModelIdx();
	if (objIdx< 0 || hitboxIdx< 0 || modelIdx<0)
		std::cout << "No object ,hitbox or model choosen!\n";
	else {
		std::cout << "Object: " << objIdx << ". " << CharacterManager::GetCharacter(modelIdx)->GetModel()->GetObject_(objIdx)->GetBasicObject()->name << std::endl;
		Hitbox* hitbox= CharacterManager::GetCharacter(modelIdx)->GetModel()->GetObject_(objIdx)->GetHitbox(hitboxIdx);
		if (hitbox == nullptr) 
			std::cout << "Wrong hitbox index!\n";
		else {
			std::cout << CON::identation << "Hitbox:\n";
			std::cout << CON::identation << CON::identation << "Joint index: " << hitboxIdx << std::endl;
			std::cout << CON::identation << CON::identation << "Name: " << hitbox->name << std::endl;
			std::cout << CON::identation << CON::identation << "Damage multiplier: " << hitbox->damageMultiplier << std::endl;
		}
	}
}

void ScaleHitbox::Process() {
	int modelIdx = Graphics::GetCurrModelIdx();
	int objectIdx = Graphics::GetCurrObjectIdx();
	int hitboxJointIdx = Graphics::GetCurrHitboxJointIdx();
	bool mainHitboxChosen = Graphics::GetCurrMainHitboxChosen();

	if (modelIdx < 0 || modelIdx >= CharacterManager::GetCharactersCount())
		std::cout << "Wrong model index!\nchoosen model: " << modelIdx << std::endl;
	else if (objectIdx < 0 || objectIdx >= CharacterManager::GetCharacter(modelIdx)->GetObjectsCount())
		std::cout << "Wrong object index!\nchoosen object: " << objectIdx << std::endl;
	else if (!mainHitboxChosen && (hitboxJointIdx < 0 || CharacterManager::GetCharacter(modelIdx)->GetModel()->GetObject_(objectIdx)->GetHitbox(hitboxJointIdx) == nullptr))
		//std::cout << "Wrong hitbox joint index!\nchoosen hitbox joint: " << hitboxJointIdx << std::endl;
		std::cout << "No hitbox chosen!\n";
	//else if (!mainHitboxChosen)
	//	std::cout << "No hitbox chosen!\n";
	else {
		float scale[3];
		std::string in;
		float eps = 0.001;
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::cout << "Scale in x axis: ";
		std::cin >> in;

		if (CheckIfStringIsFloat(in)) {
			scale[0] = atof(in.c_str());
			if (scale[0] <= eps) scale[0] = 1.0f;
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cout << "Scale in y axis: ";
			std::cin >> in;

			if (CheckIfStringIsFloat(in)) {
				scale[1] = atof(in.c_str());
				if (scale[1] <= eps) scale[1] = 1.0f;
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::cout << "Scale in z axis: ";
				std::cin >> in;

				if (CheckIfStringIsFloat(in)) {
					scale[2] = atof(in.c_str());
					if (scale[2] <= eps) scale[2] = 1.0f;

					CalcNewHitboxCoords(modelIdx, objectIdx, hitboxJointIdx, mainHitboxChosen, scale);

					std::cout << "Done!\n";
				}
				else std::cout << "Wrong value!\n";
			}
			else std::cout << "wrong value!\n";
		}
		else std::cout << "Wrong value!\n";
	}
}

void ScaleHitbox::CalcNewHitboxCoords(int modelIdx, int objectIdx, int hitboxJointIdx, bool mainHitboxChosen, float scale[]) {
	glm::vec4* basicHitboxCoords;
	Hitbox::Axis hitboxAxis;
	if (!mainHitboxChosen) {
		basicHitboxCoords = CharacterManager::GetCharacter(modelIdx)->GetModel()->GetObject_(objectIdx)->GetHitbox(hitboxJointIdx)->basicVertices;
		hitboxAxis = CharacterManager::GetCharacter(modelIdx)->GetModel()->GetObject_(objectIdx)->GetHitbox(hitboxJointIdx)->localAxis;
	}
	else {
		basicHitboxCoords = CharacterManager::GetCharacter(modelIdx)->GetModel()->GetObject_(objectIdx)->GetMainHitbox()->basicVertices;
		hitboxAxis = CharacterManager::GetCharacter(modelIdx)->GetModel()->GetObject_(objectIdx)->GetMainHitbox()->localAxis;
	}
	glm::vec4 hitboxAxisTab[3];
	hitboxAxisTab[0] = glm::vec4(hitboxAxis.x, 0.0f);
	hitboxAxisTab[1] = glm::vec4(hitboxAxis.y, 0.0f);
	hitboxAxisTab[2] = glm::vec4(hitboxAxis.z, 0.0f);

	glm::vec4 center(0.0f);

	for (int i = 0; i < 8; ++i)
		center += basicHitboxCoords[i];
	center /= 8.0f;

	float dirVal[3];
	float dotVal;
	glm::vec4 dirVec;

	dirVec = basicHitboxCoords[0] - center;
	dirVal[0] = glm::dot(dirVec, hitboxAxisTab[0]);
	dirVal[1] = glm::dot(dirVec, hitboxAxisTab[1]);
	dirVal[2] = glm::dot(dirVec, hitboxAxisTab[2]);

	for (int i = 1; i < 8; ++i) {
		dirVec = basicHitboxCoords[i] - center;
		dotVal = glm::dot(dirVec, hitboxAxisTab[0]);
		if (dotVal > dirVal[0]) dirVal[0] = dotVal;
		dotVal = glm::dot(dirVec, hitboxAxisTab[1]);
		if (dotVal > dirVal[1]) dirVal[1] = dotVal;
		dotVal = glm::dot(dirVec, hitboxAxisTab[2]);
		if (dotVal > dirVal[2]) dirVal[2] = dotVal;
	}

	for (int i = 0; i < 3; ++i)
		dirVal[i] *= scale[i];

	glm::vec4 newCoords[8];

	newCoords[0] = dirVal[0] * hitboxAxisTab[0] + dirVal[1] * hitboxAxisTab[1] + dirVal[2] * hitboxAxisTab[2];
	newCoords[1] = dirVal[0] * (-hitboxAxisTab[0]) + dirVal[1] * hitboxAxisTab[1] + dirVal[2] * hitboxAxisTab[2];
	newCoords[2] = dirVal[0] * (-hitboxAxisTab[0]) + dirVal[1] * (-hitboxAxisTab[1]) + dirVal[2] * hitboxAxisTab[2];
	newCoords[3] = dirVal[0] * hitboxAxisTab[0] + dirVal[1] * (-hitboxAxisTab[1]) + dirVal[2] * hitboxAxisTab[2];

	newCoords[4] = dirVal[0] * hitboxAxisTab[0] + dirVal[1] * hitboxAxisTab[1] + dirVal[2] * (-hitboxAxisTab[2]);
	newCoords[5] = dirVal[0] * (-hitboxAxisTab[0]) + dirVal[1] * hitboxAxisTab[1] + dirVal[2] * (-hitboxAxisTab[2]);
	newCoords[6] = dirVal[0] * (-hitboxAxisTab[0]) + dirVal[1] * (-hitboxAxisTab[1]) + dirVal[2] * (-hitboxAxisTab[2]);
	newCoords[7] = dirVal[0] * hitboxAxisTab[0] + dirVal[1] * (-hitboxAxisTab[1]) + dirVal[2] * (-hitboxAxisTab[2]);

	for (int i = 0; i < 8; ++i)
		newCoords[i] += center;

	if(!mainHitboxChosen)
		CharacterManager::GetCharacter(modelIdx)->GetModel()->GetObject_(objectIdx)->ChangeHitboxCoords(hitboxJointIdx, newCoords);
	else CharacterManager::GetCharacter(modelIdx)->GetModel()->GetObject_(objectIdx)->ChangeMainHitboxCoords(newCoords);
}

void MoveHitbox::Process() {
	int modelIdx = Graphics::GetCurrModelIdx();
	int objectIdx = Graphics::GetCurrObjectIdx();
	int hitboxJointIdx = Graphics::GetCurrHitboxJointIdx();
	bool mainHitboxChosen = Graphics::GetCurrMainHitboxChosen();

	if (modelIdx < 0 || modelIdx >= CharacterManager::GetCharactersCount())
		std::cout << "Wrong model index!\nchoosen model: " << modelIdx << std::endl;
	else if (objectIdx < 0 || objectIdx >= CharacterManager::GetCharacter(modelIdx)->GetObjectsCount())
		std::cout << "Wrong object index!\nchoosen object: " << objectIdx << std::endl;
	else if (!mainHitboxChosen && (hitboxJointIdx < 0 || CharacterManager::GetCharacter(modelIdx)->GetModel()->GetObject_(objectIdx)->GetHitbox(hitboxJointIdx) == nullptr))
		std::cout << "No hitbox chosen!\n";
	else {
		glm::vec3 move;
		std::string in;
		bool globalCoordSys = false;
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::cout << "Choose coordination system [l - local, g - global]: ";
		std::cin >> in;
		if(in=="l" || in=="g"){
			if (in == "g") globalCoordSys = true;
			else globalCoordSys = false;

			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cout << "Scale in x axis: ";
			std::cin >> in;

			if (CheckIfStringIsFloat(in)) {
				move[0] = atof(in.c_str());
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::cout << "Scale in y axis: ";
				std::cin >> in;

				if (CheckIfStringIsFloat(in)) {
					move[1] = atof(in.c_str());
					std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					std::cout << "Scale in z axis: ";
					std::cin >> in;

					if (CheckIfStringIsFloat(in)) {
						move[2] = atof(in.c_str());

						CalcNewHitboxCoords(modelIdx, objectIdx, hitboxJointIdx, mainHitboxChosen, move, globalCoordSys);

						std::cout << "Done!\n";
					}
					else std::cout << "Wrong value!\n";
				}
				else std::cout << "wrong value!\n";
			}
			else std::cout << "Wrong value!\n";
		}
		else std::cout << "Wrong value!\n";
	}
}

void MoveHitbox::CalcNewHitboxCoords(int modelIdx, int objectIdx, int hitboxIdx, bool mainHitboxChosen, glm::vec3 move, bool ifGlobal) {
	glm::mat4 mat;	
	Hitbox* hitbox;
	Hitbox::Axis axis;
	glm::vec4 *basicHitboxCoords;
	glm::vec4 newCoords[8];
	glm::vec3 moveVec(0.0f);
	if (!mainHitboxChosen) 
		hitbox = CharacterManager::GetCharacter(modelIdx)->GetModel()->GetObject_(objectIdx)->GetHitbox(hitboxIdx);
	else hitbox = CharacterManager::GetCharacter(modelIdx)->GetModel()->GetObject_(objectIdx)->GetMainHitbox();

	mat = CharacterManager::GetCharacter(modelIdx)->GetModel()->GetObject_(objectIdx)->GetBasicObject()->globalTransform;
	basicHitboxCoords = hitbox->basicVertices;
	axis = hitbox->localAxis;

	if(!ifGlobal) moveVec = axis.x*move.x + axis.y*move.y + axis.z*move.z;
	else moveVec = glm::inverse(mat)*glm::vec4(move, 0.0f);

	for (int i = 0; i < 8; ++i)
		newCoords[i] = basicHitboxCoords[i] + glm::vec4(moveVec, 0.0f);

	if (!mainHitboxChosen)
		CharacterManager::GetCharacter(modelIdx)->GetModel()->GetObject_(objectIdx)->ChangeHitboxCoords(hitboxIdx, newCoords);
	else CharacterManager::GetCharacter(modelIdx)->GetModel()->GetObject_(objectIdx)->ChangeMainHitboxCoords(newCoords);
}

void ResetHitboxVertices::Process() {
	int modelIdx = Graphics::GetCurrModelIdx();
	int objectIdx = Graphics::GetCurrObjectIdx();
	int hitboxJointIdx = Graphics::GetCurrHitboxJointIdx();
	bool mainHitboxChosen = Graphics::GetCurrMainHitboxChosen();

	if (modelIdx < 0 || modelIdx >= CharacterManager::GetCharactersCount())
		std::cout << "Wrong model index!\nchoosen model: " << modelIdx << std::endl;
	else if (objectIdx < 0 || objectIdx >= CharacterManager::GetCharacter(modelIdx)->GetObjectsCount())
		std::cout << "Wrong object index!\nchoosen object: " << objectIdx << std::endl;
	else if (!mainHitboxChosen && (hitboxJointIdx < 0 || CharacterManager::GetCharacter(modelIdx)->GetModel()->GetObject_(objectIdx)->GetHitbox(hitboxJointIdx) == nullptr))
		std::cout << "No hitbox chosen!\n";
		//std::cout << "Wrong hitbox joint index!\nchoosen hitbox joint: " << hitboxJointIdx << std::endl;
	else {
		if(!mainHitboxChosen) CharacterManager::GetCharacter(modelIdx)->GetModel()->GetObject_(objectIdx)->ResetHitboxVertices(hitboxJointIdx);
		else CharacterManager::GetCharacter(modelIdx)->GetModel()->GetObject_(objectIdx)->ResetMainHitboxVertices();
		std::cout << "Done!\n";
	}
}

void LinkHitboxes::Process() {
	int modelIdx = Graphics::GetCurrModelIdx();
	int objectIdx = Graphics::GetCurrObjectIdx();
	int hitboxJointIdx = Graphics::GetCurrHitboxJointIdx();
	bool mainHitboxChosen = Graphics::GetCurrMainHitboxChosen();

	if (modelIdx < 0 || modelIdx >= CharacterManager::GetCharactersCount())
		std::cout << "Wrong model index!\nchoosen model: " << modelIdx << std::endl;
	else if (objectIdx < 0 || objectIdx >= CharacterManager::GetCharacter(modelIdx)->GetObjectsCount())
		std::cout << "Wrong object index!\nchoosen object: " << objectIdx << std::endl;
	else if (mainHitboxChosen)
		std::cout << "Cannot link main hitbox!\n";
	else if (hitboxJointIdx < 0 || CharacterManager::GetCharacter(modelIdx)->GetModel()->GetObject_(objectIdx)->GetHitbox(hitboxJointIdx) == nullptr)
		std::cout << "Wrong hitbox joint index!\nchoosen hitbox joint: " << hitboxJointIdx << std::endl;
	else {
		std::vector<int> hitboxesIdx;
		std::string in("");
		std::cout << "Available hitboxes:\n";

		Model* model = CharacterManager::GetCharacter(modelIdx)->GetModel();
		Object* obj = model->GetObject_(objectIdx);
		BasicObject* basicObj = obj->GetBasicObject();
		std::cout << "Object: " << objectIdx << ". " << basicObj->name << std::endl;
		for (int j = 0; j < basicObj->skeleton.joints.size(); ++j) {
			if (obj->GetHitbox(j) != nullptr && j!=hitboxJointIdx) std::cout << CON::identation << j << ". " << basicObj->skeleton.joints[j]->name << std::endl;
		}

		std::cout << "Select hitbox childs to link [end - to end selection mode]:\n";
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		bool endFlag = false;
		int idx;
		while (!endFlag) {
			std::cout << ">> ";
			std::cin >> in;
			if (in == "end")
				endFlag = true;
			else {
				if (CheckIfStringIsInt(in)) {
					idx = atoi(in.c_str());
					if (obj->GetHitbox(idx) != nullptr && idx != hitboxJointIdx)
						hitboxesIdx.push_back(idx);
					else std::cout << "Wrong hitbox index!\n";
				}
				else std::cout << "Wrong value!\n";
			}
		}

		obj->LinkHitboxes(hitboxJointIdx, hitboxesIdx);
		std::cout << "Done!\n";
	}
}

void UnlinkHitboxes::Process() {
	int modelIdx = Graphics::GetCurrModelIdx();
	int objectIdx = Graphics::GetCurrObjectIdx();
	int hitboxJointIdx = Graphics::GetCurrHitboxJointIdx();
	bool mainHitboxchosen = Graphics::GetCurrMainHitboxChosen();

	if (modelIdx < 0 || modelIdx >= CharacterManager::GetCharactersCount())
		std::cout << "Wrong model index!\nchoosen model: " << modelIdx << std::endl;
	else if (objectIdx < 0 || objectIdx >= CharacterManager::GetCharacter(modelIdx)->GetObjectsCount())
		std::cout << "Wrong object index!\nchoosen object: " << objectIdx << std::endl;
	else if (mainHitboxchosen)
		std::cout << "Cannot unlink main hitbox!\n";
	else if (hitboxJointIdx < 0 || CharacterManager::GetCharacter(modelIdx)->GetModel()->GetObject_(objectIdx)->GetHitbox(hitboxJointIdx) == nullptr)
		std::cout << "Wrong hitbox joint index!\nchoosen hitbox joint: " << hitboxJointIdx << std::endl;
	else {
		CharacterManager::GetCharacter(modelIdx)->GetModel()->GetObject_(objectIdx)->UnlinkHitboxes(hitboxJointIdx);
		std::cout << "Done!\n";
	}
}

void ChangeHitboxName::Process() {
	int modelIdx = Graphics::GetCurrModelIdx();
	int objectIdx = Graphics::GetCurrObjectIdx();
	int hitboxJointIdx = Graphics::GetCurrHitboxJointIdx();
	bool mainHitboxChosen = Graphics::GetCurrMainHitboxChosen();

	if (modelIdx < 0 || modelIdx >= CharacterManager::GetCharactersCount())
		std::cout << "Wrong model index!\nchoosen model: " << modelIdx << std::endl;
	else if (objectIdx < 0 || objectIdx >= CharacterManager::GetCharacter(modelIdx)->GetObjectsCount())
		std::cout << "Wrong object index!\nchoosen object: " << objectIdx << std::endl;
	else if (!mainHitboxChosen && (hitboxJointIdx < 0 || CharacterManager::GetCharacter(modelIdx)->GetModel()->GetObject_(objectIdx)->GetHitbox(hitboxJointIdx) == nullptr))
		std::cout << "No hitbox chosen!\n";
		//std::cout << "Wrong hitbox joint index!\nchoosen hitbox joint: " << hitboxJointIdx << std::endl;
	else {
		std::string in;
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		std::cout << "New name: ";
		std::cin >> in;
		if (in != "") {
			if(!mainHitboxChosen) CharacterManager::GetCharacter(modelIdx)->GetModel()->GetObject_(objectIdx)->GetHitbox(hitboxJointIdx)->name = in;
			else CharacterManager::GetCharacter(modelIdx)->GetModel()->GetObject_(objectIdx)->GetMainHitbox()->name = in;
			std::cout << "Done!\n";
		}
		else std::cout << "Wrong name!\n";
	}
}

void ChangeHitboxDamageMultiplier::Process() {
	int modelIdx = Graphics::GetCurrModelIdx();
	int objectIdx = Graphics::GetCurrObjectIdx();
	int hitboxJointIdx = Graphics::GetCurrHitboxJointIdx();
	bool mainHitboxChosen = Graphics::GetCurrMainHitboxChosen();

	if (modelIdx < 0 || modelIdx >= CharacterManager::GetCharactersCount())
		std::cout << "Wrong model index!\nchoosen model: " << modelIdx << std::endl;
	else if (objectIdx < 0 || objectIdx >= CharacterManager::GetCharacter(modelIdx)->GetObjectsCount())
		std::cout << "Wrong object index!\nchoosen object: " << objectIdx << std::endl;
	else if (!mainHitboxChosen && (hitboxJointIdx < 0 || CharacterManager::GetCharacter(modelIdx)->GetModel()->GetObject_(objectIdx)->GetHitbox(hitboxJointIdx) == nullptr))
		std::cout << "No hitbox chosen!\n";
		//std::cout << "Wrong hitbox joint index!\nchoosen hitbox joint: " << hitboxJointIdx << std::endl;
	else {
		std::string in;
		float val;
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		std::cout << "New damage multiplier: ";
		std::cin >> in;

		if(CheckIfStringIsFloat(in)){
			val = atof(in.c_str());

			if(!mainHitboxChosen) CharacterManager::GetCharacter(modelIdx)->GetModel()->GetObject_(objectIdx)->GetHitbox(hitboxJointIdx)->damageMultiplier = val;
			else CharacterManager::GetCharacter(modelIdx)->GetModel()->GetObject_(objectIdx)->GetMainHitbox()->damageMultiplier = val;

			std::cout << "Done!\n";
		}
		else std::cout << "Wrong value!\n";
	}
}

void ShowObjects::Process() {
	int modelIdx = Graphics::GetCurrModelIdx();

	if(CheckCurrModelIdx()){
		std::cout << "Available objects:\n";
		for (int i = 0; i < CharacterManager::GetCharacter(modelIdx)->GetModel()->GetObjectsCount(); ++i)
			std::cout << CON::identation << i << ". " << CharacterManager::GetCharacter(modelIdx)->GetModel()->GetObject_(i)->GetBasicObject()->name << std::endl;
	}
}

void ChooseObject::Process() {
	int modelIdx = Graphics::GetCurrModelIdx();

	if(CheckCurrModelIdx()){
		std::string in;
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::cout << "Object index: ";
		std::cin >> in;
		if (CheckIfStringIsInt(in)) {
			int inVal = atoi(in.c_str());
			if (inVal<0 || inVal>CharacterManager::GetCharacter(modelIdx)->GetObjectsCount() - 1)
				std::cout << "Wrong index!\n";
			else Graphics::SetCurrObjectIdx(inVal);
		}
		else std::cout << "Wrong value!\n";
	}
}

void ShowCurrentObject::Process() {
	int modelIdx = Graphics::GetCurrModelIdx();
	int objectIdx = Graphics::GetCurrObjectIdx();

	if (CheckCurrObjectIdx())
		std::cout << objectIdx << ". " << CharacterManager::GetCharacter(modelIdx)->GetModel()->GetObject_(objectIdx)->GetBasicObject()->name << std::endl;
}

void ShowAllAnimationsData::Process() {
	int modelIdx = Graphics::GetCurrModelIdx();
	int objectIdx = Graphics::GetCurrObjectIdx();

	if (CheckCurrObjectIdx()){
		if (CharacterManager::GetCharacter(modelIdx)->GetModel()->GetObject_(objectIdx)->GetAnimationManager() != nullptr) {
			std::cout << "Available animations:\n";
			for (int i = 0; i < CharacterManager::GetCharacter(modelIdx)->GetModel()->GetObject_(objectIdx)->GetAnimationManager()->GetAnimationsCount(); ++i)
				std::cout << CON::identation << i << ". " << CharacterManager::GetCharacter(modelIdx)->GetModel()->GetObject_(objectIdx)->GetBasicObject()->animationsInfo[i].name << std::endl;
		}
		else std::cout << "No animations available!\n";
	}
}

void ShowAnimationData::Process() {
	int modelIdx = Graphics::GetCurrModelIdx();
	int objectIdx = Graphics::GetCurrObjectIdx();

	if (CharacterManager::GetCharacter(modelIdx)->GetModel()->GetObject_(objectIdx)->GetAnimationManager() != nullptr) {
		int animationIdx = Graphics::GetCurrAnimationIdx();

		if (CheckCurrAnimationIdx()) {
			std::cout << animationIdx << ". " << CharacterManager::GetCharacter(modelIdx)->GetModel()->GetObject_(objectIdx)->GetBasicObject()->animationsInfo[animationIdx].name << std::endl;
			std::cout << CON::identation << "Frames count: " << CharacterManager::GetCharacter(modelIdx)->GetModel()->GetObject_(objectIdx)->GetBasicObject()->animationsInfo[animationIdx].frameCount << std::endl;
			std::cout << CON::identation << "FPS: " << CharacterManager::GetCharacter(modelIdx)->GetModel()->GetObject_(objectIdx)->GetAnimationManager()->GetBaseAnimtionFPS() << std::endl;
		}
	}
	else std::cout << "No animations available!\n";
}

void StartAnimation::Process() {
	int modelIdx = Graphics::GetCurrModelIdx();
	int objectIdx = Graphics::GetCurrObjectIdx();

	if (CheckCurrObjectIdx()) {
		if (CharacterManager::GetCharacter(modelIdx)->GetModel()->GetObject_(objectIdx)->GetAnimationManager() != nullptr) {
			std::string in;
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cout << "Animation index: ";
			std::cin >> in;

			if (CheckIfStringIsInt(in)) {
				int animIdx = atoi(in.c_str());

				Object* obj = CharacterManager::GetCharacter(modelIdx)->GetModel()->GetObject_(objectIdx);
				if (animIdx >= 0 && animIdx < obj->GetBasicObject()->animationsInfo.size()) {
					obj->StartAnimation(animIdx);
				}
				else {
					std::cout << "Wrong animation index!\n";
				}
			}
		}
		else std::cout << "No animations available!\n";
	}
}

void StopAnimation::Process() {
	int modelIdx = Graphics::GetCurrModelIdx();
	int objectIdx = Graphics::GetCurrObjectIdx();

	if (CheckCurrObjectIdx()) {
		if (CharacterManager::GetCharacter(modelIdx)->GetModel()->GetObject_(objectIdx)->GetAnimationManager() != nullptr) {
			std::string in;
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cout << "Animation index: ";
			std::cin >> in;

			if (CheckIfStringIsInt(in)) {
				int animIdx = atoi(in.c_str());

				Object* obj = CharacterManager::GetCharacter(modelIdx)->GetModel()->GetObject_(objectIdx);
				if (animIdx >= 0 && animIdx < obj->GetBasicObject()->animationsInfo.size()) {
					obj->StopAnimation(animIdx);
				}
				else {
					std::cout << "Wrong animation index!\n";
				}
			}
		}
		else std::cout << "No animations available!\n";
	}
}

void ShowActiveAnimations::Process() {
	int modelIdx = Graphics::GetCurrModelIdx();
	int objectIdx = Graphics::GetCurrObjectIdx();

	if (CheckCurrObjectIdx()) {
		if (CharacterManager::GetCharacter(modelIdx)->GetModel()->GetObject_(objectIdx)->GetAnimationManager() != nullptr) {
			AnimationManager* animManager = CharacterManager::GetCharacter(modelIdx)->GetModel()->GetObject_(objectIdx)->GetAnimationManager();
			if (animManager != nullptr) {
				for (int i = 0; i < animManager->GetAnimationsCount(); ++i) {
					if (animManager->GetAnimation(i)->active == true) {
						std::cout << i << ". " << animManager->GetAnimation(i)->name << std::endl;
					}
				}
			}
		}
		else std::cout << "No animations available!\n";
	}
}

void ResetAnimations::Process() {
	int modelIdx = Graphics::GetCurrModelIdx();
	int objectIdx = Graphics::GetCurrObjectIdx();

	if (CheckCurrObjectIdx()) {
		if (CharacterManager::GetCharacter(modelIdx)->GetModel()->GetObject_(objectIdx)->GetAnimationManager() != nullptr) {
			Object* obj = CharacterManager::GetCharacter(modelIdx)->GetModel()->GetObject_(objectIdx);
			for (int i = 0; i < obj->GetBasicObject()->animationsInfo.size(); ++i)
				obj->StopAnimation(i);
		}
		else std::cout << "No animations available!\n";
	}
}

void ExportModel::Process() {
	if (Graphics::GetCurrModelIdx() >= 0) {
		if (Graphics::GetCurrModelIdx() < CharacterManager::GetCharactersCount()) {
			std::string filename;
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cout << "Filename: ";
			std::cin >> filename;

			size_t pos = filename.std::string::find(".", 0);

			if (pos != std::string::npos) {
				if (strcmp(filename.c_str() + pos, ".mgr") != 0) {
					std::cout << "Wrong file extension!\nDefault extension: .mgr\n";
					return;
				}
			}
			else filename += ".mgr";

			std::cout << "Exporting file!\n";
			ExportFile::Export(filename.c_str(), CharacterManager::GetCharacter(Graphics::GetCurrModelIdx())->GetModel(), 
				CharacterManager::GetCharacter(Graphics::GetCurrModelIdx())->GetScaleMat());
			std::cout << "File successfully exported!\n";
		}
		else std::cout << "Model " << Graphics::GetCurrModelIdx() << " does not exist!\n";
	}
	else std::cout << "No model chosen!\n";
}

void DeleteModel::Process() {
	int modelIdx = Graphics::GetCurrModelIdx();

	if (modelIdx < 0)
		std::cout << "No model chosen!\n";
	else {
		std::cout << "Deleting model..\n";
		Graphics::DeleteModel();
		while (Graphics::GetModelSemafor() > 0);
		std::cout << "Model deleted!\n";
	}
}

void ScaleModel::Process() {
	if (CheckCurrModelIdx) {
		std::string in;
		float val;
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::cout << "Scale value: ";
		std::cin >> in;
		if (CheckIfStringIsFloat(in)) {
			val = atof(in.c_str());

			CharacterManager::GetCharacter(Graphics::GetCurrModelIdx())->SetScaleMat(val);
		}
		else std::cout << "Wrong value!\n";
	}
	else std::cout << "No model chosen!\n";
}

void SetModelHeight::Process() {
	if (CheckCurrModelIdx) {
		std::string in;
		float val;
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::cout << "Character height: ";
		std::cin >> in;
		if (CheckIfStringIsFloat(in)) {
			val = atof(in.c_str());

			float maxHeight=0.0f;

			glm::vec4 minVec, maxVec, tmpVec;

			Character* character = CharacterManager::GetCharacter(Graphics::GetCurrModelIdx());
			Object* obj;
			glm::mat4 transform;
			for (int i = 0; i < character->GetModel()->GetObjectsCount(); ++i) {
				obj = character->GetModel()->GetObject_(i);
				transform = obj->GetBasicObject()->globalTransform;
				minVec = maxVec = transform*obj->GetMainHitbox()->basicVertices[0];

				for (int j = 1; j < 8; ++j) {
					tmpVec = transform * obj->GetMainHitbox()->basicVertices[j];
					if (tmpVec.y > maxVec.y) maxVec = tmpVec;
					if (tmpVec.y < minVec.y) minVec = tmpVec;
				}

				if (maxVec.y - minVec.y > maxHeight) maxHeight = maxVec.y - minVec.y;
			}

			float scaleVal = val / maxHeight;

			character->SetScaleMat(scaleVal);
		}
	}
	else std::cout << "No model chosen!\n";
}
