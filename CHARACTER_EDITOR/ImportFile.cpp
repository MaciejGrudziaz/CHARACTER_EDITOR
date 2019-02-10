#include "ImportFile.h"
int ImportFile::errorFlag = 0;
std::map<int, std::vector<Hitbox*> > ImportFile::importedHitboxes;
std::map<int, Hitbox*> ImportFile::importedMainHitboxes;

int ImportFile::Import(const char* filename,BasicModel* model) {
	importedHitboxes.clear();
	importedMainHitboxes.clear();

	errorFlag = CheckFile(filename);

	if (errorFlag != 0) return errorFlag;

	char block[blockSize] = { 0 };
	//BasicModel* model = new BasicModel();

	std::fstream file(filename, std::ios::in | std::ios::binary);

	do {
		file.read(block, blockSize * sizeof(char));

		if (strcmp(block, FILEHDR::modelHeader) == 0)
			if (LoadModel(model, file) != 0) {
				errorFlag = 3;
				file.close();
				delete model;
				for (HitboxMap::iterator it = importedHitboxes.begin(); it != importedHitboxes.end();++it) {
					for (Hitbox* hitbox : it->second)
						delete hitbox;
				}

				return errorFlag;
			}

	} while ((file.rdstate() & std::fstream::eofbit) == 0);

	if (errorFlag == 0) {
		for (int i = 0; i < model->objects.size(); ++i) {
			if (model->objects[i]->skeleton.joints.size() > 0) {
				for (int j = 0; j < model->objects[i]->skeleton.joints[0]->animations.size(); ++j) {
					BasicObject::AnimationInfo animInfo;
					animInfo.frameCount = model->objects[i]->skeleton.joints[0]->animations[j]->frameCount;
					animInfo.name = model->objects[i]->skeleton.joints[0]->animations[j]->name;
					model->objects[i]->animationsInfo.push_back(animInfo);
				}
			}

			HitboxMap::iterator it = importedHitboxes.find(i);
			if (it != importedHitboxes.end()) {
				for (Hitbox* hitbox : it->second)
					hitbox->name = model->objects[i]->skeleton.joints[hitbox->jointIdx]->name;
			}
		}
	}

	return errorFlag;
}

//-----------------------------------------------------------------------------
//-----------------------------------PRIVATE-----------------------------------
//-----------------------------------------------------------------------------

int ImportFile::CheckFile(const char* filename) {
	char block[blockSize] = { 0 };

	std::fstream file(filename, std::ios::in | std::ios::binary);
	
	if (!file.is_open())
		return 2;

	file.read(block, blockSize * sizeof(char));

	if ((file.rdstate() & std::fstream::eofbit) != 0) {
		file.close();
		return 1;
	}
	else {
		if (strcmp(block, FILEHDR::header) != 0) {
			file.close();
			return 1;
		}
	}

	do {
		file.read(block, blockSize * sizeof(char));
	} while ((file.rdstate()&std::fstream::eofbit) == 0);

	if (strncmp(block + (blockSize - (sizeof(FILEHDR::footer) / sizeof(char))), FILEHDR::footer,sizeof(FILEHDR::footer)/sizeof(char)) != 0) {
		file.close();
		return 1;
	}

	file.close();
	return 0;
}

int ImportFile::LoadModel(BasicModel* model, std::fstream& file) {
	char block[blockSize] = { 0 };

	bool modelEnd = false;
	int objIdx = -1;

	do {
		file.read(block, blockSize * sizeof(char));

		if (strcmp(block, FILEHDR::objectHeader) == 0) {
			if (LoadObject(model, file) != 0) {
				return 1;
			}
			++objIdx;
		}
		else if (strcmp(block, FILEHDR::hitboxHeader) == 0) {
			if (LoadHitbox(file, objIdx) != 0)
				return 1;
		}
		else if (strcmp(block, FILEHDR::modelFooter) == 0)
			modelEnd = true;
		else if ((file.rdstate()&std::fstream::eofbit) != 0) {
			modelEnd = true;
			return 1;
		}
	
	} while (!modelEnd);

	return 0;
}

int ImportFile::LoadObject(BasicModel* model, std::fstream& file) {
	char block[blockSize] = { 0 };

	BasicObject* object = new BasicObject();

	file.read(block, blockSize * sizeof(char));
	if ((file.rdstate()&std::fstream::eofbit) != 0)
		return 1;

	object->name = std::string(block);
	file.read(block, blockSize * sizeof(char));
	if ((file.rdstate()&std::fstream::eofbit) != 0)
		return 1;

	object->textureName = std::string(block);

	Mat4Struct globalTransform;
	file.read((char*)(&globalTransform), sizeof(Mat4Struct));
	if ((file.rdstate()&std::fstream::eofbit) != 0)
		return 1;

	object->globalTransform = globalTransform.GetGLM_Mat();

	bool objectEnd = false;

	do{
		file.read(block, blockSize * sizeof(char));

		if (strcmp(block, FILEHDR::controlPtHeader) == 0) {
			if (LoadControlPt(object, file) != 0) {
				objectEnd = false;
				return 1;
			}
		}
		else if (strcmp(block, FILEHDR::facesHeader) == 0) {
			if (LoadFaces(object, file) != 0) {
				objectEnd = false;
				return 1;
			}
		}
		else if (strcmp(block, FILEHDR::skeletonHeader) == 0) {
			if (LoadSkeleton(object, file) != 0) {
				objectEnd = false;
				return 1;
			}
		}
		else if (strcmp(block, FILEHDR::objectFooter) == 0) {
			model->objects.push_back(object);
			objectEnd = true;
		}
		else if ((file.rdstate()&std::fstream::eofbit) != 0) {
			objectEnd = true;
			return 1;
		}

	} while (!objectEnd);

	return 0;
}

int ImportFile::LoadControlPt(BasicObject* object, std::fstream& file) {
	char block[blockSize] = { 0 };

	int controlPtCount = 0;
	int bytesRead = 0;

	file.read((char*)(&controlPtCount), sizeof(int));
	if ((file.rdstate()&std::fstream::eofbit) != 0)
		return 1;
	bytesRead += sizeof(int);

	ControlPtStruct* controlPtIn=new ControlPtStruct[controlPtCount];
	file.read((char*)controlPtIn, controlPtCount * sizeof(ControlPtStruct));
	if ((file.rdstate()&std::fstream::eofbit) != 0) {
		delete[]controlPtIn;
		return 1;
	}
	bytesRead += controlPtCount * sizeof(ControlPtStruct);

	BasicObject::ControlPoint controlPt;

	for (int i = 0; i < controlPtCount; ++i) {
		controlPt.coords.x = controlPtIn[i].coords[0];
		controlPt.coords.y = controlPtIn[i].coords[1];
		controlPt.coords.z = controlPtIn[i].coords[2];
		controlPt.coords.w = 0.0;

		for (int j = 0; j < 4; ++j) {
			BasicObject::BlendingIndexWeightPair blendAndWeight;
			blendAndWeight.jointIndex = controlPtIn[i].blendingJointIdx[j];
			blendAndWeight.weight = controlPtIn[i].blendingWeight[j];
			controlPt.blendingInfo.push_back(blendAndWeight);
		}
		
		object->controlPoints.push_back(controlPt);

		controlPt.blendingInfo.clear();
	}
	delete[]controlPtIn;

	int blockAlignement = blockSize - (bytesRead%blockSize);
	file.read(block, blockAlignement * sizeof(char));
	if ((file.rdstate()&std::fstream::eofbit) != 0)
		return 1;

	file.read(block, blockSize * sizeof(char));
	if ((file.rdstate()&std::fstream::eofbit) != 0)
		return 1;
	if (strcmp(block, FILEHDR::controlPtFooter) != 0)
		return 1;

	return 0;
}

int ImportFile::LoadFaces(BasicObject* object, std::fstream& file) {
	char block[blockSize] = { 0 };
	int bytesRead = 0;

	int faceCount = 0;

	file.read((char*)(&faceCount), sizeof(int));
	if ((file.rdstate()&std::fstream::eofbit) != 0)
		return 1;
	bytesRead += sizeof(int);

	FaceStruct* faceIn = new FaceStruct[faceCount];
	file.read((char*)faceIn, faceCount * sizeof(FaceStruct));
	if ((file.rdstate()&std::fstream::eofbit) != 0){
		delete[]faceIn;
		return 1;
	}
	bytesRead += faceCount * sizeof(FaceStruct);

	BasicObject::Face face;

	for (int i = 0; i < faceCount; ++i) {
		//file.read((char*)(&faceIn), sizeof(FaceStruct));
		//if ((file.rdstate()&std::fstream::eofbit) != 0)
		//	return 1;

		for (int j = 0; j < 3; ++j) {
			face.indices[j] = faceIn[i].indices[j];

			face.normals[j].x = faceIn[i].normal[j][0];
			face.normals[j].y = faceIn[i].normal[j][1];
			face.normals[j].z = faceIn[i].normal[j][2];
			face.normals[j].w = 0.0f;

			face.uv[j].x = faceIn[i].uv[j][0];
			face.uv[j].y = faceIn[i].uv[j][1];
		}

		object->faces.push_back(face);
	}
	delete[]faceIn;

	int blockAlignement = blockSize - (bytesRead%blockSize);
	file.read(block, blockAlignement * sizeof(char));
	if ((file.rdstate()&std::fstream::eofbit) != 0)
		return 1;

	file.read(block, blockSize * sizeof(char));
	if ((file.rdstate()&std::fstream::eofbit) != 0)
		return 1;
	if (strcmp(block, FILEHDR::facesFooter) != 0)
		return 1;

	return 0;
}

int ImportFile::LoadSkeleton(BasicObject* object, std::fstream& file) {
	char block[blockSize] = { 0 };

	bool skeletonEnd = false;

	do {
		file.read(block, blockSize * sizeof(char));

		if (strcmp(block, FILEHDR::jointHeader) == 0) {
			if (LoadJoint(object, file) != 0) {
				skeletonEnd = true;
				return 1;
			}
		}
		else if (strcmp(block, FILEHDR::skeletonFooter)==0)
			skeletonEnd = true;
		else if ((file.rdstate()&std::fstream::eofbit) != 0) {
			skeletonEnd = true;
			return 1;
		}
	} while (!skeletonEnd);

	return 0;
}

int ImportFile::LoadJoint(BasicObject* object, std::fstream& file) {
	char block[blockSize] = { 0 };

	BasicObject::Joint* joint=new BasicObject::Joint;

	file.read(block, blockSize * sizeof(char));
	if ((file.rdstate()&std::fstream::eofbit) != 0)
		return 1;

	joint->name = std::string(block);

	file.read(block, blockSize * sizeof(char));
	if ((file.rdstate()&std::fstream::eofbit) != 0)
		return 1;

	joint->parentIndex = *((int*)block);

	Mat4Struct globalBindposeInverse;

	file.read((char*)(&globalBindposeInverse), sizeof(Mat4Struct));
	if ((file.rdstate()&std::fstream::eofbit) != 0)
		return 1;

	joint->globalBindposeInverse = globalBindposeInverse.GetGLM_Mat();

	bool jointEnd = false;

	do {
		file.read(block, blockSize * sizeof(char));

		if (strcmp(block, FILEHDR::animationsHeader) == 0) {
			if (LoadAnimation(joint, file) != 0) {
				jointEnd = true;
				return 1;
			}
		}
		else if (strcmp(block, FILEHDR::jointFooter)==0) {
			object->skeleton.joints.push_back(joint);
			jointEnd = true;
		}
		else if ((file.rdstate()&std::fstream::eofbit) != 0) {
			jointEnd = true;
			return 1;
		}
	} while (!jointEnd);

	return 0;
}

int ImportFile::LoadAnimation(BasicObject::Joint* joint, std::fstream& file) {
	char block[blockSize] = { 0 };

	BasicObject::AnimationJoint* animationJoint=new BasicObject::AnimationJoint;
	
	file.read(block, blockSize * sizeof(char));
	if ((file.rdstate()&std::fstream::eofbit) != 0)
		return 1;

	animationJoint->name = std::string(block);

	file.read(block, blockSize * sizeof(char));
	if ((file.rdstate()&std::fstream::eofbit) != 0)
		return 1;

	animationJoint->frameCount = *((int*)block);

	bool animationEnd = false;

	do {
		file.read(block, blockSize * sizeof(char));

		if (strcmp(block, FILEHDR::frameHeader) == 0) {
			if (LoadFrame(animationJoint, file) != 0) {
				animationEnd = true;
				return 1;
			}
		}
		else if (strncmp(block, FILEHDR::animationsFooter,sizeof(FILEHDR::animationsFooter)/sizeof(char)) == 0) {
			joint->animations.push_back(animationJoint);
			animationEnd = true;
		}
		else if ((file.rdstate()&std::fstream::eofbit) != 0) {
			animationEnd = true;
			return 1;
		}
	} while (!animationEnd);

	return 0;
}

int ImportFile::LoadFrame(BasicObject::AnimationJoint* animationJoint, std::fstream& file) {
	char block[blockSize];

	BasicObject::Keyframe* frame = new BasicObject::Keyframe();

	file.read(block, blockSize * sizeof(char));
	if ((file.rdstate()&std::fstream::eofbit) != 0) {
		delete frame;
		return 1;
	}

	frame->frameNum = *((int*)block);

	Mat4Struct globalTransform;

	file.read((char*)(&globalTransform), sizeof(Mat4Struct));
	if ((file.rdstate()&std::fstream::eofbit) != 0) {
		delete frame;
		return 1;
	}

	frame->globalTransform = globalTransform.GetGLM_Mat();

	if (animationJoint->frames.size() > 0)
		animationJoint->frames[animationJoint->frames.size() - 1]->nextFrame = frame;

	animationJoint->frames.push_back(frame);

	file.read(block, blockSize * sizeof(char));
	if ((file.rdstate()&std::fstream::eofbit) != 0)
		return 1;
	if (strcmp(block, FILEHDR::frameFooter) != 0)
		return 1;

	return 0;
}

int ImportFile::LoadHitbox(std::fstream& file, int objectIdx) {
	char block[blockSize];

	Hitbox* hitbox = new Hitbox;

	MainHitboxMap::iterator it1 = importedMainHitboxes.find(objectIdx);
	HitboxMap::iterator it2 = importedHitboxes.find(objectIdx);
	if (it1 == importedMainHitboxes.end())
		importedMainHitboxes.insert(MainHitboxMap::value_type(objectIdx, hitbox));
	else {
		if (it2 == importedHitboxes.end()) {
			std::pair<HitboxMap::iterator, bool> p;
			p = importedHitboxes.insert(HitboxMap::value_type(objectIdx, std::vector<Hitbox*>()));
			it2 = p.first;
		}

		it2->second.push_back(hitbox);
	}

	int bytesRead = 0;

	file.read((char*)(&hitbox->jointIdx), sizeof(int));
	if ((file.rdstate()&std::fstream::eofbit) != 0)
		return 1;
	bytesRead += sizeof(int);
	file.read((char*)(&hitbox->damageMultiplier), sizeof(double));
	if ((file.rdstate()&std::fstream::eofbit) != 0)
		return 1;
	bytesRead += sizeof(double);

	AxisStruct axis;

	file.read((char*)(&axis), sizeof(AxisStruct));
	if ((file.rdstate()&std::fstream::eofbit) != 0)
		return 1;
	bytesRead += sizeof(AxisStruct);

	hitbox->localAxis.x = axis.GetX_axis();
	hitbox->localAxis.y = axis.GetY_axis();
	hitbox->localAxis.z = axis.GetZ_axis();

	float basicVertices[8 * 3];

	file.read((char*)basicVertices, 24 * sizeof(float));
	if ((file.rdstate()&std::fstream::eofbit) != 0)
		return 1;
	bytesRead += 24 * sizeof(float);

	for (int i = 0; i < 8; ++i) {
		hitbox->basicVertices[i].x = basicVertices[i * 3 + 0];
		hitbox->basicVertices[i].y = basicVertices[i * 3 + 1];
		hitbox->basicVertices[i].z = basicVertices[i * 3 + 2];
		hitbox->basicVertices[i].w = 0.0f;
	}

	float basicNormals[6 * 3];

	file.read((char*)basicNormals, 18 * sizeof(float));
	if ((file.rdstate()&std::fstream::eofbit) != 0)
		return 1;
	bytesRead += 18 * sizeof(float);

	for (int i = 0; i < 6; ++i) {
		hitbox->basicNormals[i].x = basicNormals[i * 3 + 0];
		hitbox->basicNormals[i].y = basicNormals[i * 3 + 1];
		hitbox->basicNormals[i].z = basicNormals[i * 3 + 2];
	}

	int blockComplement = blockSize - (bytesRead%blockSize);

	file.read(block, blockComplement * sizeof(char));
	if ((file.rdstate()&std::fstream::eofbit) != 0)
		return 1;

	file.read(block, blockSize * sizeof(char));
	if ((file.rdstate()&std::fstream::eofbit) != 0)
		return 1;

	if (strncmp(block, FILEHDR::hitboxFooter, sizeof(FILEHDR::hitboxFooter) / sizeof(char)) != 0) {
		delete hitbox;
		return 1;
	}

	return 0;
}
