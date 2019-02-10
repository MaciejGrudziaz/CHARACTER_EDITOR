#include "ExportFile.h"

int ExportFile::Export(const char* filename, Model* model){
	std::fstream file(filename, std::ios::out | std::ios::binary | std::ios::trunc);

	char block[blockSize] = { 0x00 };

	for (int i = 0; i < sizeof(FILEHDR::header) / sizeof(char); ++i)
		block[i] = FILEHDR::header[i];

	file.write(block, blockSize * sizeof(char));
	memset(block, 0x00, blockSize * sizeof(char));

	for (int i = 0; i < sizeof(FILEHDR::modelHeader) / sizeof(char); ++i)
		block[i] = FILEHDR::modelHeader[i];

	file.write(block, blockSize * sizeof(char));
	memset(block, 0x00, blockSize * sizeof(char));

	for (int i = 0; i < model->GetObjectsCount(); ++i) {
		WriteObject(model->GetObject_(i)->GetBasicObject(), file);

		//if (model->GetObject_(i)->GetHitboxCount() > 0)
			WriteObjectHitboxes(model->GetObject_(i), file);
	}

	for (int i = 0; i < sizeof(FILEHDR::modelFooter) / sizeof(char); ++i)
		block[i] = FILEHDR::modelFooter[i];

	file.write(block, blockSize * sizeof(char));
	memset(block, 0x00, blockSize * sizeof(char));

	int footerOffset = blockSize - sizeof(FILEHDR::footer) / sizeof(char);
	for (int i = 0; i < sizeof(FILEHDR::footer) / sizeof(char); ++i)
		block[footerOffset+i] = FILEHDR::footer[i];

	file.write(block, blockSize * sizeof(char));

	file.close();

	return 0;
}

void ExportFile::WriteObject(BasicObject* object, std::fstream& file) {
	char block[blockSize] = { 0x00 };

	for (int j = 0; j < sizeof(FILEHDR::objectHeader) / sizeof(char); ++j)
		block[j] = FILEHDR::objectHeader[j];

	file.write(block, blockSize * sizeof(char));
	memset(block, 0x00, blockSize * sizeof(char));

	for (int j = 0; j < object->name.size(); ++j)
		block[j] = object->name[j];

	file.write(block, blockSize * sizeof(char));
	memset(block, 0x00, blockSize * sizeof(char));

	for (int j = 0; j < object->textureName.size(); ++j)
		block[j] = object->textureName[j];

	file.write(block, blockSize * sizeof(char));
	memset(block, 0x00, blockSize * sizeof(char));

	Mat4Struct globalTransform(object->globalTransform);

	file.write((char*)(globalTransform.mat), sizeof(Mat4Struct));

	WriteControlPoints(object, file);

	WriteFaces(object, file);

	WriteSkeleton(object, file);

	for (int i = 0; i < sizeof(FILEHDR::objectFooter) / sizeof(char); ++i)
		block[i] = FILEHDR::objectFooter[i];

	file.write(block, blockSize * sizeof(char));
}

void ExportFile::WriteControlPoints(BasicObject* object, std::fstream& file) {
	char block[blockSize] = { 0x00 };

	for (int j = 0; j < sizeof(FILEHDR::controlPtHeader) / sizeof(char); ++j)
		block[j] = FILEHDR::controlPtHeader[j];

	file.write(block, blockSize * sizeof(char));
	memset(block, 0x00, blockSize * sizeof(char));

	int controlPtCount = object->controlPoints.size();

	file.write((char*)(&controlPtCount), sizeof(int));

	ControlPtStruct* controlPoints = new ControlPtStruct[controlPtCount];

	for (int j = 0; j < controlPtCount; ++j) {
		controlPoints[j].coords[0] = object->controlPoints[j].coords.x;
		controlPoints[j].coords[1] = object->controlPoints[j].coords.y;
		controlPoints[j].coords[2] = object->controlPoints[j].coords.z;

		for (int k = 0; k < 4; ++k) {
			controlPoints[j].blendingJointIdx[k] = object->controlPoints[j].blendingInfo[k].jointIndex;
			controlPoints[j].blendingWeight[k] = object->controlPoints[j].blendingInfo[k].weight;
		}
	}

	file.write((char*)controlPoints, controlPtCount * sizeof(ControlPtStruct));
	delete[]controlPoints;

	int addBlock = blockSize - ((sizeof(int) + controlPtCount * sizeof(ControlPtStruct)) % blockSize);
	file.write(block, addBlock * sizeof(char));

	for (int i = 0; i < sizeof(FILEHDR::controlPtFooter) / sizeof(char); ++i)
		block[i] = FILEHDR::controlPtFooter[i];

	file.write(block, blockSize * sizeof(char));
}

void ExportFile::WriteFaces(BasicObject* object, std::fstream& file) {
	char block[blockSize] = { 0x00 };

	for (int i = 0; i < sizeof(FILEHDR::facesHeader) / sizeof(char); ++i)
		block[i] = FILEHDR::facesHeader[i];

	file.write(block, blockSize * sizeof(char));
	memset(block, 0x00, blockSize * sizeof(char));

	int facesCount = object->faces.size();

	file.write((char*)(&facesCount), sizeof(int));

	FaceStruct* faces = new FaceStruct[facesCount];

	for (int i = 0; i < facesCount; ++i) {
		for (int j = 0; j < 3; ++j) {
			faces[i].indices[j] = object->faces[i].indices[j];

			faces[i].normal[j][0] = object->faces[i].normals[j].x;
			faces[i].normal[j][1] = object->faces[i].normals[j].y;
			faces[i].normal[j][2] = object->faces[i].normals[j].z;

			faces[i].uv[j][0] = object->faces[i].uv[j].x;
			faces[i].uv[j][1] = object->faces[i].uv[j].y;
		}
	}

	file.write((char*)faces, facesCount * sizeof(FaceStruct));
	delete[]faces;

	int addBlock = blockSize - ((sizeof(int) + facesCount * sizeof(FaceStruct)) % blockSize);
	file.write(block, addBlock * sizeof(char));

	for (int i = 0; i < sizeof(FILEHDR::facesFooter) / sizeof(char); ++i)
		block[i] = FILEHDR::facesFooter[i];

	file.write(block, blockSize * sizeof(char));
}

void ExportFile::WriteSkeleton(BasicObject* object, std::fstream& file) {
	char block[blockSize] = { 0x00 };

	for (int i = 0; i < sizeof(FILEHDR::skeletonHeader) / sizeof(char); ++i)
		block[i] = FILEHDR::skeletonHeader[i];

	file.write(block, blockSize * sizeof(char));
	memset(block, 0x00, blockSize * sizeof(char));

	for (int i = 0; i < object->skeleton.joints.size(); ++i)
		WriteJoint(object->skeleton.joints[i], file);

	for (int i = 0; i < sizeof(FILEHDR::skeletonFooter)/sizeof(char);++i)
		block[i] = FILEHDR::skeletonFooter[i];

	file.write(block, blockSize * sizeof(char));
}

void ExportFile::WriteJoint(BasicObject::Joint* joint, std::fstream& file) {
	char block[blockSize] = { 0x00 };

	for (int i = 0; i < sizeof(FILEHDR::jointHeader) / sizeof(char); ++i)
		block[i] = FILEHDR::jointHeader[i];

	file.write(block, blockSize * sizeof(char));
	memset(block, 0x00, blockSize * sizeof(char));

	for (int i = 0; i < joint->name.size(); ++i)
		block[i] = joint->name[i];

	file.write(block, blockSize * sizeof(char));
	memset(block, 0x00, blockSize * sizeof(char));

	file.write((char*)(&joint->parentIndex), sizeof(int));

	int blockAdd = blockSize - (sizeof(int));
	file.write(block, blockAdd * sizeof(char));

	Mat4Struct globalBindposeInverse(joint->globalBindposeInverse);

	file.write((char*)(globalBindposeInverse.mat), sizeof(Mat4Struct));

	for (BasicObject::AnimationJoint* anim : joint->animations)
		WriteAnimation(anim, file);

	for (int i = 0; i < sizeof(FILEHDR::jointFooter) / sizeof(char); ++i)
		block[i] = FILEHDR::jointFooter[i];

	file.write(block, blockSize * sizeof(char));
}

void ExportFile::WriteAnimation(BasicObject::AnimationJoint* animation, std::fstream& file) {
	char block[blockSize] = { 0x00 };

	for (int i = 0; i < sizeof(FILEHDR::animationsHeader) / sizeof(char); ++i)
		block[i] = FILEHDR::animationsHeader[i];

	file.write(block, blockSize * sizeof(char));
	memset(block, 0x00, blockSize * sizeof(char));

	for (int i = 0; i < animation->name.size(); ++i)
		block[i] = animation->name[i];

	file.write(block, blockSize * sizeof(char));
	memset(block, 0x00, blockSize * sizeof(char));

	file.write((char*)(&animation->frameCount), sizeof(int));

	int blockAdd = blockSize - sizeof(int);
	file.write(block, blockAdd * sizeof(char));

	for (BasicObject::Keyframe* frame : animation->frames)
		WriteFrame(frame, file);

	for (int i = 0; i < sizeof(FILEHDR::animationsFooter) / sizeof(char); ++i)
		block[i] = FILEHDR::animationsFooter[i];

	file.write(block, blockSize * sizeof(char));
}

void ExportFile::WriteFrame(BasicObject::Keyframe* frame, std::fstream& file) {
	char block[blockSize] = { 0x00 };

	for (int i = 0; i < sizeof(FILEHDR::frameHeader) / sizeof(char); ++i)
		block[i] = FILEHDR::frameHeader[i];

	file.write(block, blockSize * sizeof(char));
	memset(block, 0x00, blockSize * sizeof(char));

	file.write((char*)(&frame->frameNum), sizeof(int));

	int blockAdd = blockSize - sizeof(int);
	file.write(block, blockAdd * sizeof(char));

	Mat4Struct globalTransform(frame->globalTransform);

	file.write((char*)globalTransform.mat, sizeof(Mat4Struct));
	
	for (int i = 0; i < sizeof(FILEHDR::frameFooter) / sizeof(char); ++i)
		block[i] = FILEHDR::frameFooter[i];

	file.write(block, blockSize * sizeof(char));
}

void ExportFile::WriteObjectHitboxes(Object* object, std::fstream& file) {
	WriteHitbox(object->GetMainHitbox(), file);

	for (int i = 0; i < object->GetBasicObject()->skeleton.joints.size(); ++i) {
		Hitbox* hitbox = object->GetHitbox(i);
		if (hitbox != nullptr)
			WriteHitbox(hitbox, file);
	}
}

void ExportFile::WriteHitbox(Hitbox* hitbox, std::fstream& file) {
	char block[blockSize] = { 0x00 };

	for (int i = 0; i < sizeof(FILEHDR::hitboxHeader) / sizeof(char); ++i)
		block[i] = FILEHDR::hitboxHeader[i];

	file.write(block, blockSize * sizeof(char));
	memset(block, 0x00, blockSize * sizeof(char));

	int bytesWrite = 0;

	file.write((char*)(&hitbox->jointIdx), sizeof(int));
	bytesWrite += sizeof(int);
	file.write((char*)(&hitbox->damageMultiplier), sizeof(double));
	bytesWrite += sizeof(double);

	AxisStruct axis(hitbox->localAxis.x, hitbox->localAxis.y, hitbox->localAxis.z);

	file.write((char*)(&axis), sizeof(AxisStruct));
	bytesWrite += sizeof(AxisStruct);

	float basicVert[8 * 3];
	for (int i = 0; i < 8; ++i) {
		basicVert[i * 3 + 0] = hitbox->basicVertices[i].x;
		basicVert[i * 3 + 1] = hitbox->basicVertices[i].y;
		basicVert[i * 3 + 2] = hitbox->basicVertices[i].z;
	}

	file.write((char*)basicVert, 24 * sizeof(float));
	bytesWrite += 24 * sizeof(float);

	float basicNormals[6 * 3];
	for (int i = 0; i < 6; ++i) {
		basicNormals[i * 3 + 0] = hitbox->basicNormals[i].x;
		basicNormals[i * 3 + 1] = hitbox->basicNormals[i].y;
		basicNormals[i * 3 + 2] = hitbox->basicNormals[i].z;
	}

	file.write((char*)basicNormals, 18 * sizeof(float));
	bytesWrite += 18 * sizeof(float);

	int addBlock = blockSize - (bytesWrite%blockSize);

	file.write(block, addBlock * sizeof(char));

	for (int i = 0; i < sizeof(FILEHDR::hitboxFooter) / sizeof(char); ++i)
		block[i] = FILEHDR::hitboxFooter[i];

	file.write(block, blockSize * sizeof(char));

}
