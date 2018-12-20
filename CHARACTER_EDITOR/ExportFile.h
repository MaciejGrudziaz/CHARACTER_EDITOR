#pragma once
#include <fstream>
#include "ExportImportStruct.h"
#include "Model.h"

class ExportFile {
	static const int blockSize = 32;

	static void WriteObject(BasicObject* object, std::fstream& file);
	static void WriteControlPoints(BasicObject* object, std::fstream& file);
	static void WriteFaces(BasicObject* object, std::fstream& file);
	static void WriteSkeleton(BasicObject* object, std::fstream& file);
	static void WriteJoint(BasicObject::Joint* joint, std::fstream& file);
	static void WriteAnimation(BasicObject::AnimationJoint* animation, std::fstream& file);
	static void WriteFrame(BasicObject::Keyframe* frame, std::fstream& file);

	static void WriteObjectHitboxes(Object* object, std::fstream& file);
	static void WriteHitbox(Hitbox* hitbox, std::fstream& file);

public:
	static int Export(const char* filename,Model* model);
};