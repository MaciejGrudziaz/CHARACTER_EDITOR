#include "BasicModelManager.h"

std::vector<BasicModel*> BasicModelManager::models;

BasicModel* BasicModelManager::RegisterModel(const char* filename) {
	BasicModel* model = new BasicModel;
	std::string filenameStr(filename);
	int errorCode;

	if (strcmp(filename + (filenameStr.size() - 4), ".fbx") == 0) {
		errorCode=FBXloader::GetInstance()->LoadModel(filename, model);
		if (errorCode == 0) {
			models.push_back(model);
			return model;
		}
		else {
			delete model;
			return nullptr;
		}
	}
	else if (strcmp(filename + (filenameStr.size() - 4), ".mgr") == 0) {
		errorCode=ImportFile::Import(filename, model);
		if (errorCode == 0) {
			models.push_back(model);
			return model;
		}
		else{ 
			delete model;
			return nullptr;
		}
	}
}

void BasicModelManager::DestroyModel(int idx) {
	if (idx >= 0 && idx < models.size()) {
		delete models[idx];
		models.erase(models.begin() + idx);
	}
}
