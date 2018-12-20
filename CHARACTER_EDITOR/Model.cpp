#include "Model.h"

void Model::Init(BasicModel* basicModel_) {
	for (unsigned i = 0; i < basicModel_->objects.size(); ++i) {
		if (basicModel_->objects[i]->animationsInfo.size() > 0) {
			objects.push_back(new DynamicObject());
		}
		else objects.push_back(new StaticObject());
		objects[i]->SetBasicObject(basicModel_->objects[i]);
	}
}

void Model::Update() {
	for (Object* object : objects)
		object->Update();
}

void Model::Draw() {
	for (Object* object : objects)
		object->Draw();
}