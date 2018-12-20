#pragma once
#include <vector>
#include "FBXloader.h"
#include "ImportFile.h"

class BasicModelManager {
	BasicModelManager() {}
	BasicModelManager(const BasicModelManager&){}
	BasicModelManager& operator=(const BasicModelManager&) {}

	static std::vector<BasicModel*> models;

public:
	static BasicModel* RegisterModel(const char* filename);

	static unsigned GetModelsCount() { return models.size(); }
	static BasicModel* GetModel(unsigned index) { if (index < models.size()) return models[index]; }
	static void DestroyModel(int idx);

};