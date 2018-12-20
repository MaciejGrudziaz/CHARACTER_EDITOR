#pragma once
#include <vector>
#include "Object.h"
#include "BasicModel.h"

class Model {
	std::vector<Object*> objects;

public:
	Model() {}

	void Init(BasicModel* basicModel_);

	void Update();

	void Draw();

	unsigned GetObjectsCount()const { return objects.size(); }
	Object* GetObject_(unsigned index) { if (index < objects.size()) { return objects[index]; } else return nullptr; }

	~Model() {
		for (Object* obj : objects)
			delete obj;
	}
};
