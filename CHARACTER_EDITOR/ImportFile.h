#pragma once
#include <fstream>
#include "ExportImportStruct.h"
#include "BasicModel.h"
#include "Object.h"

class ImportFile {
	static const int blockSize = 32;

	static int errorFlag;

	static int CheckFile(const char* filename);

	static int LoadModel(BasicModel* model, std::fstream& file);
	static int LoadObject(BasicModel* model, std::fstream& file);
	static int LoadControlPt(BasicObject* object, std::fstream& file);
	static int LoadFaces(BasicObject* object, std::fstream& file);
	static int LoadSkeleton(BasicObject* object, std::fstream& file);
	static int LoadJoint(BasicObject* object, std::fstream& file);
	static int LoadAnimation(BasicObject::Joint* joint, std::fstream& file);
	static int LoadFrame(BasicObject::AnimationJoint* animationJoint, std::fstream& file);

	static int LoadHitbox(std::fstream& file,int objectIdx);

	typedef std::map<int, std::vector<Hitbox*> > HitboxMap;
	static HitboxMap importedHitboxes;
	typedef std::map<int, Hitbox*> MainHitboxMap;
	static MainHitboxMap importedMainHitboxes;

public:
	static int Import(const char* filename,BasicModel* model);

	static bool ImportedHitboxesAvailable() { if (importedHitboxes.size() > 0) return true; else return false; }

	static bool ImportedHitboxesAvailableForObject(int objectIdx) {
		HitboxMap::iterator it = importedHitboxes.find(objectIdx);
		if (it != importedHitboxes.end())
			return true;
		else return false;
	}

	static int GetHitboxCount(int objectIdx) {
		HitboxMap::iterator it = importedHitboxes.find(objectIdx);
		if (it != importedHitboxes.end())
			return it->second.size();
		else return 0;
	}

	static Hitbox* GetMainHitbox(int objectIdx) {
		MainHitboxMap::iterator it = importedMainHitboxes.find(objectIdx);
		if (it != importedMainHitboxes.end()) {
			return it->second;
		}
		else return nullptr;
	}

	static Hitbox* GetHitbox(int objectIdx, int hitboxIdx) {
		HitboxMap::iterator it = importedHitboxes.find(objectIdx);
		if (it != importedHitboxes.end()) {
			if (hitboxIdx >= 0 && hitboxIdx < it->second.size())
				return it->second[hitboxIdx];
			else return nullptr;
		}
		else return nullptr;
	}

	static int GetErrorFlag() { return errorFlag; }
};