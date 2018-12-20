#include "Object.h"

//-----------------------------------------------------------------------------
//-----------------------------------Object------------------------------------
//-----------------------------------------------------------------------------

Object::Object() :vertices(nullptr), hitboxVertices(nullptr), basicObject(nullptr), modelUpdate(false), viewUpdate(false), projectionUpdate(false), objectBufferVertexAttribCount(13),hitboxObjectBufferVertexAttribCount(4) ,
currentHitboxJointIdx(-2)
{ }

void Object::LoadShader(Shader* shaderProgram_) { 
	if (shaderProgram_ != nullptr)
		shaderManager->LoadMainShaderProgram(shaderProgram_);
}

void Object::LoadHitboxShader(Shader* hitboxBasicShaderProgram_) {
	if (hitboxBasicShaderProgram_ != nullptr)
		shaderManager->LoadHitboxVisualizationShaderProgram(hitboxBasicShaderProgram_);
	else throw std::exception();
	
}

void Object::LoadHitboxComputeShader(Shader* hitboxComputeShaderProgram) {
	if (hitboxComputeShaderProgram != nullptr)
		shaderManager->LoadHitboxComputeShaderProgram(hitboxComputeShaderProgram);
	else throw std::exception();
}

int Object::LoadTexture(std::string filename) {
	if (filename == g_modelsDirectory) {
		textureAvailable = 0;
		shaderManager->LoadTexture("");
		return 1;
	}
	else {
		textureAvailable = 1;
		shaderManager->LoadTexture(filename.c_str());
		return 1;
	}
}

int Object::StartAnimation(int animationIndex_) {
	if (animationManager == nullptr) return -1;
	else if (animationIndex_<0 || animationIndex_ > animationManager->GetAnimationsCount() - 1)
		return 0;
	else {
		animationManager->Start(animationIndex_);
		return 1;
	}
}

int Object::StopAnimation(int animationIndex_) {
	if (animationManager == nullptr) return -1;
	else if (animationIndex_<0 || animationIndex_> animationManager->GetAnimationsCount() - 1)
		return 0;
	else {
		animationManager->Stop(animationIndex_);
		return 1;
	}
}

void Object::ChangeHitboxCoords(int hitboxJointIdx, glm::vec4 newVerticesCoords[]) {
	HitboxMap::iterator it = hitboxes.find(hitboxJointIdx);
	if (it != hitboxes.end()) {
		memcpy(it->second->basicVertices, newVerticesCoords, 8 * sizeof(glm::vec4));

		int startPosIdx = -1;

		for (int i = 3; i < hitboxVerticesCount; i += 32) {
			if (hitboxVertices[i] == hitboxJointIdx) {
				startPosIdx = i - 3;
				break;
			}
		}

		if (startPosIdx != -1) {
			float f_newVerticesCoords[32];
			for (int i = 0; i < 8; ++i) {
				f_newVerticesCoords[4 * i + 0] = newVerticesCoords[i].x;
				f_newVerticesCoords[4 * i + 1] = newVerticesCoords[i].y;
				f_newVerticesCoords[4 * i + 2] = newVerticesCoords[i].z;
				f_newVerticesCoords[4 * i + 3] = hitboxJointIdx;
			}

			memcpy(hitboxVertices + startPosIdx, f_newVerticesCoords, 32 * sizeof(float));
			++updateHitboxVerticesInBuffer;
		}
	}
}

void Object::LinkHitboxes(int parentHitboxJointIdx, std::vector<int>& childHitboxJointIdx) {
	HitboxMap::iterator it = hitboxes.find(parentHitboxJointIdx);

	if (it != hitboxes.end()) {
		Hitbox::Axis parentAxis = it->second->localAxis;
		
		glm::vec4 center(0.0f);

		for (int i = 0; i < 8; ++i)
			center += it->second->basicVertices[i];
		center /= 8.0f;

		std::vector<glm::vec4> childCenter;
		glm::vec4 tmpCenter(0.0f);

		for (int idx : childHitboxJointIdx) {
			it = hitboxes.find(idx);

			if (it != hitboxes.end()) {
				tmpCenter = glm::vec4(0.0f);

				for (int i = 0; i < 8; ++i)
					tmpCenter += it->second->basicVertices[i];
				tmpCenter /= 8.0f;

				childCenter.push_back(tmpCenter);
			}
		}

		for (glm::vec4 c : childCenter)
			center += c;

		center /= (childCenter.size() + 1.0f);

		it = hitboxes.find(parentHitboxJointIdx);

		glm::vec4 dir[6];
		float dirVal[6];
		glm::vec4 parentAxisTab[6];
		glm::vec4 dirVec= it->second->basicVertices[0] - center;
		parentAxisTab[0] = glm::vec4(parentAxis.x, 0.0f);
		parentAxisTab[1] = glm::vec4(-parentAxis.x, 0.0f);
		parentAxisTab[2] = glm::vec4(parentAxis.y, 0.0f);
		parentAxisTab[3] = glm::vec4(-parentAxis.y, 0.0f);
		parentAxisTab[4] = glm::vec4(parentAxis.z, 0.0f);
		parentAxisTab[5] = glm::vec4(-parentAxis.z, 0.0f);

		for (int i = 0; i < 6; ++i) {
			dir[i] = dirVec;
			dirVal[i] = glm::dot(dirVec, parentAxisTab[i]);
		}
		
		float dotVal;
		for (int i = 1; i < 7; ++i) {
			dirVec = it->second->basicVertices[i] - center;
			
			for (int j = 0; j < 6; ++j) {
				dotVal = glm::dot(dirVec, parentAxisTab[j]);
				if (dotVal > dirVal[j]) {
					dirVal[j] = dotVal;
					dir[j] = dirVec;
				}
			}
		}

		LinkedHitboxesMap::iterator linkIt = linkedHitboxes.find(parentHitboxJointIdx);
		if (linkIt == linkedHitboxes.end()) {
			std::pair<LinkedHitboxesMap::iterator, bool> p = linkedHitboxes.insert(LinkedHitboxesMap::value_type(parentHitboxJointIdx, std::vector<Hitbox*>()));
			linkIt = p.first;
		}

		for (int idx : childHitboxJointIdx) {
			it = hitboxes.find(idx);

			if (it != hitboxes.end()) {
				for (int i = 0; i < 7; ++i) {
					dirVec = it->second->basicVertices[i] - center;

					for (int j = 0; j < 6; ++j) {
						dotVal = glm::dot(dirVec, parentAxisTab[j]);
						if (dotVal > dirVal[j]) {
							dirVal[j] = dotVal;
							dir[j] = dirVec;
						}
					}
				}
				linkIt->second.push_back(it->second);
				hitboxes.erase(it);
			}
		}

		glm::vec4 newParentHitboxCoords[8];

		newParentHitboxCoords[0] = parentAxisTab[0] * dirVal[0] + parentAxisTab[2] * dirVal[2] + parentAxisTab[4] * dirVal[4];
		newParentHitboxCoords[1] = parentAxisTab[1] * dirVal[1] + parentAxisTab[2] * dirVal[2] + parentAxisTab[4] * dirVal[4];
		newParentHitboxCoords[2] = parentAxisTab[1] * dirVal[1] + parentAxisTab[3] * dirVal[3] + parentAxisTab[4] * dirVal[4];
		newParentHitboxCoords[3] = parentAxisTab[0] * dirVal[0] + parentAxisTab[3] * dirVal[3] + parentAxisTab[4] * dirVal[4];

		newParentHitboxCoords[4] = parentAxisTab[0] * dirVal[0] + parentAxisTab[2] * dirVal[2] + parentAxisTab[5] * dirVal[5];
		newParentHitboxCoords[5] = parentAxisTab[1] * dirVal[1] + parentAxisTab[2] * dirVal[2] + parentAxisTab[5] * dirVal[5];
		newParentHitboxCoords[6] = parentAxisTab[1] * dirVal[1] + parentAxisTab[3] * dirVal[3] + parentAxisTab[5] * dirVal[5];
		newParentHitboxCoords[7] = parentAxisTab[0] * dirVal[0] + parentAxisTab[3] * dirVal[3] + parentAxisTab[5] * dirVal[5];

		for (int i = 0; i < 8; ++i)
			newParentHitboxCoords[i] += center;

		it = hitboxes.find(parentHitboxJointIdx);
		memcpy(it->second->basicVertices, newParentHitboxCoords, 8 * sizeof(glm::vec4));

		delete[]hitboxVertices;
		delete[]hitboxOutVertices;

		LoadHitboxBufferData();
		++updateHitboxVerticesInBuffer;
	}
}

void Object::ResetHitboxVertices(int hitboxJointIdx) {
	HitboxMap::iterator it = hitboxes.find(hitboxJointIdx);

	if (it != hitboxes.end())
		ChangeHitboxCoords(hitboxJointIdx, it->second->initVertices);
}

void Object::UnlinkHitboxes(int parentHitboxJointIdx) {
	LinkedHitboxesMap::iterator linkedIt = linkedHitboxes.find(parentHitboxJointIdx);

	if (linkedIt != linkedHitboxes.end()) {
		for (std::vector<Hitbox*>::iterator hIt = linkedIt->second.begin(); hIt != linkedIt->second.end(); ++hIt) {
			hitboxes.insert(HitboxMap::value_type((*hIt)->jointIdx, (*hIt)));
		}
	}

	linkedHitboxes.erase(linkedIt);

	LoadHitboxBufferData();
	++updateHitboxVerticesInBuffer;
}

//-----------------------------------------------------------------------------
//-------------------------------Object::Private-------------------------------
//-----------------------------------------------------------------------------

void Object::CreateHitboxes() {
	CreateHitboxes_LoadVertices();

	CreateHitboxes_HitboxCreateAlgorithm();

	CreateHitboxes_CalcMainHitbox();

	LoadHitboxBufferData();
}

void Object::CreateHitboxes_LoadVertices() {
	for (unsigned i = 0; i < basicObject->controlPoints.size(); ++i) {
		BasicObject::ControlPoint& pt = basicObject->controlPoints[i];
		int jointIdx;
		for (int j = 0; j < 4; ++j) {
			if (pt.blendingInfo[j].weight >= 0.45) {
				jointIdx= pt.blendingInfo[j].jointIndex;

				HitboxMap::iterator it;
				it=hitboxes.find(jointIdx);
				if (it == hitboxes.end()) {
					std::pair<HitboxMap::iterator, bool> insertVal;
					insertVal = hitboxes.insert(HitboxMap::value_type(jointIdx, new Hitbox()));
					if (insertVal.second == true) {
						Hitbox* hitbox = insertVal.first->second;
						hitbox->jointIdx = jointIdx;
						hitbox->name = basicObject->skeleton.joints[jointIdx]->name;
						hitbox->controlPointsIdx.push_back(i);
					}
				}
				else it->second->controlPointsIdx.push_back(i);

			}
		}
	}
}

void Object::CreateHitboxes_HitboxCreateAlgorithm() {
	Hitbox* hitbox;
	Hitbox::Axis localAxis;
	Hitbox::MoveValues axisMoveVal;
	glm::vec3 center;
	std::vector<glm::vec3> vectors;

	for (HitboxMap::iterator it = hitboxes.begin(); it != hitboxes.end(); ++it) {
		if ((*it).second->controlPointsIdx.size() >= 2) {
			vectors.clear();
			hitbox = it->second;

			center = CreateHitboxes_HitboxCreateAlgorithm_FindCenter(hitbox);
			CreateHitboxes_HitboxCreateAlgorithm_CalcVectorsFromCenter(hitbox, center, vectors);
			CreateHitboxes_HitboxCreateAlgorithm_CalcLocalAxis(hitbox, vectors, localAxis);
			CreateHitboxes_HitboxCreateAlgorithm_CalcMoveInLocalAxis(vectors, localAxis, axisMoveVal);
			CreateHitboxes_HitboxCreateAlgorithm_CalcHitboxVertices(center, localAxis, axisMoveVal, hitbox);
		}
	}
}

glm::vec3 Object::CreateHitboxes_HitboxCreateAlgorithm_FindCenter(const Hitbox* hitbox) {
	glm::vec3 center(0.0f);

	int idx = -1;
	for (unsigned i = 0; i < hitbox->controlPointsIdx.size(); ++i) {
		idx = hitbox->controlPointsIdx[i];
		center += glm::vec3(basicObject->controlPoints[idx].coords);
	}

	center /= static_cast<float>(hitbox->controlPointsIdx.size());

	return center;
}

void Object::CreateHitboxes_HitboxCreateAlgorithm_CalcVectorsFromCenter(const Hitbox* hitbox, glm::vec3 center, std::vector<glm::vec3>& vectors) {
	glm::vec3 vec;

	int idx = -1;
	for(unsigned i=0;i<hitbox->controlPointsIdx.size();++i){
		idx = hitbox->controlPointsIdx[i];
		vec = glm::vec3(basicObject->controlPoints[idx].coords) - center;
		vectors.push_back(vec);
	}
}

void Object::CreateHitboxes_HitboxCreateAlgorithm_CalcLocalAxis(Hitbox* hitbox, const std::vector<glm::vec3>& vectors, Hitbox::Axis& localAxis) {
	typedef std::map<int, Hitbox::IndexGroup> IndexMap;
	IndexMap indexes;
	IndexMap linkIndexes;

	CreateHitboxes_HitboxCreateAlgorithm_CalcLocalAxis_CreateBasicIndexGroups(vectors, indexes);

	CreateHitboxes_HitboxCreateAlgorithm_CalcLocalAxis_LinkIndexGroups(indexes, linkIndexes);

	std::vector<glm::vec3> newVertices, newVectors;
	std::pair<int, int> idxPairAxisY;

	CreateHitboxes_HitboxCreateAlgorithm_CalcLocalAxis_CalcNewCoords(hitbox, linkIndexes, newVertices, newVectors);

	CreateHitboxes_HitboxCreateAlgorithm_CalcLocalAxis_CalcAxisY(newVertices, newVectors, idxPairAxisY, localAxis);

	CreateHitboxes_HitboxCreateAlgorithm_CalcLocalAxis_CalcAxisXZ(newVertices, newVectors, idxPairAxisY, localAxis);

	hitbox->localAxis.x = glm::vec4(localAxis.x, 0.0f);
	hitbox->localAxis.y = glm::vec4(localAxis.y, 0.0f);
	hitbox->localAxis.z = glm::vec4(localAxis.z, 0.0f);
}

void Object::CreateHitboxes_HitboxCreateAlgorithm_CalcLocalAxis_CreateBasicIndexGroups(const std::vector<glm::vec3>& vectors, std::map<int, Hitbox::IndexGroup>& indexes) {
	std::vector<float> vecLength;
	std::vector<glm::vec3> unitVec;

	for (unsigned i = 0; i < vectors.size(); ++i) {
		unitVec.push_back(glm::normalize(vectors[i]));
		vecLength.push_back(glm::length(vectors[i]));
	}

	typedef std::map<int, Hitbox::IndexGroup> IndexMap;
	Hitbox::IndexGroup* group;
	const float lengthFactor = 0.5f;
	const float coneAngle = 3.1415629f / 4.0f;
	for (unsigned i = 0; i < unitVec.size(); ++i) {
		std::pair<IndexMap::iterator, bool> insertVal;
		insertVal = indexes.insert(IndexMap::value_type(i, Hitbox::IndexGroup(i)));
		group = &(insertVal.first->second);
		for (unsigned j = 0; j < unitVec.size(); ++j) {
			if (j != i) {
				if (glm::dot(unitVec[i], unitVec[j]) > cos(coneAngle) && (vecLength[j]>(1.0f - lengthFactor)*vecLength[i] && vecLength[j]<(1.0f + lengthFactor)*vecLength[i])) {
					group->idx.insert(j);
				}
			}
		}
	}
}

void Object::CreateHitboxes_HitboxCreateAlgorithm_CalcLocalAxis_LinkIndexGroups(std::map<int, Hitbox::IndexGroup>& indexes, std::map<int, Hitbox::IndexGroup>& linkIndexes) {
	typedef std::map<int, Hitbox::IndexGroup> IndexMap;
	typedef std::set<int> IndexSet;
	Hitbox::IndexGroup* group;
	int idx = 0;
	for (IndexMap::iterator it = indexes.begin(); it != indexes.end(); ++it) {
		group = &(it->second);
		if (group->status == Hitbox::IndexGroup::OPEN) {
			group->status = Hitbox::IndexGroup::IN_USE;
			std::pair<IndexMap::iterator, bool> insertVal;
			insertVal = linkIndexes.insert(IndexMap::value_type(idx, Hitbox::IndexGroup(idx)));
			Hitbox::IndexGroup* currNewIdxGroup = &(insertVal.first->second);
			currNewIdxGroup->idx.insert(group->baseIdx);

			for (IndexSet::iterator setIt = group->idx.begin(); setIt != group->idx.end(); ++setIt) {
				if (CreateHitboxes_HitboxCreateAlgorithm_CalcLocalAxis_CheckIdxGroup(group->baseIdx, (*setIt), indexes, currNewIdxGroup) == true)
					currNewIdxGroup->idx.insert((*setIt));
			}
			group->status = Hitbox::IndexGroup::CLOSED;
			++idx;
		}
	}
}

void Object::CreateHitboxes_HitboxCreateAlgorithm_CalcLocalAxis_CalcNewCoords(const Hitbox* hitbox, std::map<int, Hitbox::IndexGroup>& linkIndexes, std::vector<glm::vec3>& newVertices, std::vector<glm::vec3>& newVectors) {
	typedef std::map<int, Hitbox::IndexGroup> IndexMap;
	typedef std::set<int> IndexSet;
	Hitbox::IndexGroup* group;
	glm::vec3 currVec(0.0f);
	for (IndexMap::iterator it = linkIndexes.begin(); it != linkIndexes.end(); ++it) {
		group = &(it->second);
		currVec = glm::vec3(0.0f);
		for (IndexSet::iterator setIt = group->idx.begin(); setIt != group->idx.end(); ++setIt)
			currVec += glm::vec3(basicObject->controlPoints[hitbox->controlPointsIdx[(*setIt)]].coords);
		currVec /= static_cast<float>(group->idx.size());
		newVertices.push_back(currVec);
	}

	glm::vec3 center(0.0f);
	for (glm::vec3& vec : newVertices)
		center += vec;
	center /= static_cast<float>(newVertices.size());

	for (unsigned i = 0; i < newVertices.size(); ++i)
		newVectors.push_back(newVertices[i] - center);
}

void Object::CreateHitboxes_HitboxCreateAlgorithm_CalcLocalAxis_CalcAxisY(const std::vector<glm::vec3>& newVertices, const std::vector<glm::vec3>& newVectors, std::pair<int, int>& idxPairAxisY, Hitbox::Axis& localAxis) {
	if (newVertices.size() >= 2) {
		float minDotVal = glm::dot(newVectors[0], newVectors[1]);
		idxPairAxisY.first = 0;
		idxPairAxisY.second = 1;
		float dotVal;

		for (int i = 0; i < newVectors.size() - 1; ++i) {
			for (int j = i + 1; j < newVectors.size(); ++j) {
				dotVal = glm::dot(newVectors[i], newVectors[j]);
				if (dotVal < minDotVal) {
					minDotVal = dotVal;
					idxPairAxisY.first = i;
					idxPairAxisY.second = j;
				}
			}
		}

		localAxis.y = (newVertices[idxPairAxisY.first] - newVertices[idxPairAxisY.second]);
		localAxis.y = glm::normalize(localAxis.y);
	}
	else localAxis.y = glm::vec3(0.0f, 1.0f, 0.0f);
}

void Object::CreateHitboxes_HitboxCreateAlgorithm_CalcLocalAxis_CalcAxisXZ(const std::vector<glm::vec3>& newVertices, const std::vector<glm::vec3>& newVectors, std::pair<int, int> idxPairAxisY, Hitbox::Axis& localAxis) {
	if(newVertices.size()>=2){
		std::pair<int, int> idxPairX(-1, -1);
		bool init = false;
		float minDotVal, dotVal;
		for (unsigned i = 0; i < newVectors.size(); ++i) {
			if (i != idxPairAxisY.first && i != idxPairAxisY.second) {
				for (unsigned j = i + 1; j < newVectors.size(); ++j) {
					if (j != idxPairAxisY.first && j != idxPairAxisY.second) {
						dotVal = glm::dot(newVectors[i], newVectors[j]);
						if (dotVal < 0) {
							if (!init) {
								minDotVal = abs(dotVal);
								idxPairX.first = i;
								idxPairX.second = j;
								init = true;
							}
							else {
								if (abs(dotVal) < minDotVal) {
									minDotVal = abs(dotVal);
									idxPairX.first = i;
									idxPairX.second = j;
								}
							}
						}
					}
				}
			}
		}

		glm::vec3 tmpX;
		if (idxPairX.first == -1 && idxPairX.second == -1)
			tmpX = glm::vec3(1.0f, 0.0f, 0.0f);
		else tmpX = (newVertices[idxPairX.first] - newVertices[idxPairX.second]);

		localAxis.z = glm::cross(tmpX, localAxis.y);
		if (localAxis.z == glm::vec3(0.0f, 0.0f, 0.0f)) localAxis.z = glm::vec3(0.0f, 0.0f, 1.0f);
		else localAxis.z = glm::normalize(localAxis.z);
		localAxis.x = glm::normalize(glm::cross(localAxis.y, localAxis.z));
	}
	else {
		localAxis.x = glm::vec3(1.0f, 0.0f, 0.0f);
		localAxis.z = glm::vec3(0.0f, 0.0f, 1.0f);
	}
}

bool Object::CreateHitboxes_HitboxCreateAlgorithm_CalcLocalAxis_CheckIdxGroup(int checkGroup, int currGroup, std::map<int, Hitbox::IndexGroup>& indexes, Hitbox::IndexGroup* currNewIdxGroup) {
	typedef std::map<int, Hitbox::IndexGroup> IndexMap;
	typedef std::set<int> IndexSet;
	IndexMap::iterator it = indexes.find(currGroup);
	Hitbox::IndexGroup* group = &(it->second);
	if (group->status == Hitbox::IndexGroup::OPEN) {
		group->status = Hitbox::IndexGroup::IN_USE;
		bool status = false;
		for (unsigned i = 0; i < group->idx.size(); ++i) {
			IndexSet::iterator setIt = group->idx.find(checkGroup);
			if (setIt != group->idx.end()) {
				status = true;
				break;
			}
		}
		if (status == true) {
			for (IndexSet::iterator setIt = group->idx.begin(); setIt != group->idx.end(); ++setIt) {
				if ((*setIt) != checkGroup) {
					if (CreateHitboxes_HitboxCreateAlgorithm_CalcLocalAxis_CheckIdxGroup(group->baseIdx, (*setIt), indexes, currNewIdxGroup) == true)
						currNewIdxGroup->idx.insert((*setIt));
				}
			}

			group->status = Hitbox::IndexGroup::CLOSED;
			return true;
		}
		else return false;
	}
	else if (group->status == Hitbox::IndexGroup::IN_USE) return true;
	else if (group->status == Hitbox::IndexGroup::CLOSED) return false;
}

void Object::CreateHitboxes_HitboxCreateAlgorithm_CalcMoveInLocalAxis(const std::vector<glm::vec3>& vectors, const Hitbox::Axis& localAxis, Hitbox::MoveValues& axisMoveVal) {
	if (vectors.size() > 0) {
		int xMoveIdx[2], yMoveIdx[2], zMoveIdx[2], crossMove1Idx[2], crossMove2Idx[2], crossMove3Idx[2], crossMove4Idx[2];
		glm::vec3 crossAxis1, crossAxis2, crossAxis3, crossAxis4;
		crossAxis1 = glm::normalize(localAxis.x + localAxis.z + localAxis.y);
		crossAxis2 = glm::normalize(-localAxis.x + localAxis.z + localAxis.y);
		crossAxis3 = glm::normalize(-localAxis.x + -localAxis.z + localAxis.y);
		crossAxis4 = glm::normalize(localAxis.x + -localAxis.z + localAxis.y);

		float xMoveVal[2] = { glm::dot(vectors[0], localAxis.x), glm::dot(vectors[0], -localAxis.x) };
		float yMoveVal[2] = { glm::dot(vectors[0], localAxis.y), glm::dot(vectors[0], -localAxis.y) };
		float zMoveVal[2] = { glm::dot(vectors[0], localAxis.z), glm::dot(vectors[0], -localAxis.z) };
		float crossMove1Val[2] = { glm::dot(vectors[0],crossAxis1),glm::dot(vectors[0],-crossAxis1) };
		float crossMove2Val[2] = { glm::dot(vectors[0],crossAxis2),glm::dot(vectors[0],-crossAxis2) };
		float crossMove3Val[2] = { glm::dot(vectors[0],crossAxis3),glm::dot(vectors[0],-crossAxis3) };
		float crossMove4Val[2] = { glm::dot(vectors[0],crossAxis4),glm::dot(vectors[0],-crossAxis4) };
		xMoveIdx[0] = xMoveIdx[1] = yMoveIdx[0] = yMoveIdx[1] = zMoveIdx[0] = zMoveIdx[1] = 0;
		crossMove1Idx[0] = crossMove1Idx[1] = crossMove2Idx[0] = crossMove2Idx[1] = crossMove3Idx[0] = crossMove3Idx[1] = crossMove4Idx[0] = crossMove4Idx[1] = 0;

		for (unsigned i = 1; i < vectors.size(); ++i) {
			float xDot, yDot, zDot, crossDot1, crossDot2, crossDot3, crossDot4;
			xDot = glm::dot(vectors[i], localAxis.x);
			yDot = glm::dot(vectors[i], localAxis.y);
			zDot = glm::dot(vectors[i], localAxis.z);
			crossDot1 = glm::dot(vectors[i], crossAxis1);
			crossDot2 = glm::dot(vectors[i], crossAxis2);
			crossDot3 = glm::dot(vectors[i], crossAxis3);
			crossDot4 = glm::dot(vectors[i], crossAxis4);
			if (xDot > xMoveVal[0]) { xMoveVal[0] = xDot; xMoveIdx[0] = i; }
			if (-xDot > xMoveVal[1]) { xMoveVal[1] = -xDot; xMoveIdx[1] = i; }
			if (yDot > yMoveVal[0]) { yMoveVal[0] = yDot; yMoveIdx[0] = i; }
			if (-yDot > yMoveVal[1]) { yMoveVal[1] = -yDot; yMoveIdx[1] = i; }
			if (zDot > zMoveVal[0]) { zMoveVal[0] = zDot; zMoveIdx[0] = i; }
			if (-zDot > zMoveVal[1]) { zMoveVal[1] = -zDot; zMoveIdx[1] = i; }

			if (crossDot1 > crossMove1Val[0]) { crossMove1Val[0] = crossDot1; crossMove1Idx[0] = i; }
			if (-crossDot1 > crossMove1Val[1]) { crossMove1Val[1] = crossDot1; crossMove1Idx[1] = i; }
			if (crossDot2 > crossMove2Val[0]) { crossMove2Val[0] = crossDot2; crossMove2Idx[0] = i; }
			if (-crossDot2 > crossMove2Val[1]) { crossMove2Val[1] = crossDot2; crossMove2Idx[1] = i; }
			if (crossDot3 > crossMove3Val[0]) { crossMove3Val[0] = crossDot3; crossMove3Idx[0] = i; }
			if (-crossDot3 > crossMove3Val[1]) { crossMove3Val[1] = crossDot3; crossMove3Idx[1] = i; }
			if (crossDot4 > crossMove4Val[0]) { crossMove4Val[0] = crossDot4; crossMove4Idx[0] = i; }
			if (-crossDot4 > crossMove4Val[1]) { crossMove4Val[1] = crossDot4; crossMove4Idx[1] = i; }
		}

		glm::mat3 rot;
		rot[0][0] = localAxis.x.x; rot[0][1] = localAxis.y.x; rot[0][2] = localAxis.z.x;
		rot[1][0] = localAxis.x.y; rot[1][1] = localAxis.y.y; rot[1][2] = localAxis.z.y;
		rot[2][0] = localAxis.x.z; rot[2][1] = localAxis.y.z; rot[2][2] = localAxis.z.z;

		for (int i = 0; i < 2; ++i) {
			axisMoveVal.xMove[i] = (rot*glm::vec3(vectors[xMoveIdx[i]])).x;
			axisMoveVal.yMove[i] = (rot*glm::vec3(vectors[yMoveIdx[i]])).y;
			axisMoveVal.zMove[i] = (rot*glm::vec3(vectors[zMoveIdx[i]])).z;
		}
	}
}

void Object::CreateHitboxes_HitboxCreateAlgorithm_CalcHitboxVertices(glm::vec3 center, const Hitbox::Axis& localAxis, const Hitbox::MoveValues& axisMoveVal, Hitbox* hitbox) {
	glm::vec3 vert;
	vert = center + axisMoveVal.xMove[0] * localAxis.x + axisMoveVal.yMove[0] * localAxis.y + axisMoveVal.zMove[0] * localAxis.z; 
	hitbox->basicVertices[0]=glm::vec4(vert,1.0f);
	hitbox->initVertices[0] = glm::vec4(vert, 1.0f);
	vert = center + axisMoveVal.xMove[1] * localAxis.x + axisMoveVal.yMove[0] * localAxis.y + axisMoveVal.zMove[0] * localAxis.z; 
	hitbox->basicVertices[1] = glm::vec4(vert, 1.0f);
	hitbox->initVertices[1] = glm::vec4(vert, 1.0f);
	vert = center + axisMoveVal.xMove[1] * localAxis.x + axisMoveVal.yMove[1] * localAxis.y + axisMoveVal.zMove[0] * localAxis.z; 
	hitbox->basicVertices[2] = glm::vec4(vert, 1.0f);
	hitbox->initVertices[2] = glm::vec4(vert, 1.0f);
	vert = center + axisMoveVal.xMove[0] * localAxis.x + axisMoveVal.yMove[1] * localAxis.y + axisMoveVal.zMove[0] * localAxis.z; 
	hitbox->basicVertices[3] = glm::vec4(vert, 1.0f);
	hitbox->initVertices[3] = glm::vec4(vert, 1.0f);

	vert = center + axisMoveVal.xMove[0] * localAxis.x + axisMoveVal.yMove[0] * localAxis.y + axisMoveVal.zMove[1] * localAxis.z; 
	hitbox->basicVertices[4] = glm::vec4(vert, 1.0f);
	hitbox->initVertices[4] = glm::vec4(vert, 1.0f);
	vert = center + axisMoveVal.xMove[1] * localAxis.x + axisMoveVal.yMove[0] * localAxis.y + axisMoveVal.zMove[1] * localAxis.z; 
	hitbox->basicVertices[5] = glm::vec4(vert, 1.0f);
	hitbox->initVertices[5] = glm::vec4(vert, 1.0f);
	vert = center + axisMoveVal.xMove[1] * localAxis.x + axisMoveVal.yMove[1] * localAxis.y + axisMoveVal.zMove[1] * localAxis.z; 
	hitbox->basicVertices[6] = glm::vec4(vert, 1.0f);
	hitbox->initVertices[6] = glm::vec4(vert, 1.0f);
	vert = center + axisMoveVal.xMove[0] * localAxis.x + axisMoveVal.yMove[1] * localAxis.y + axisMoveVal.zMove[1] * localAxis.z; 
	hitbox->basicVertices[7] = glm::vec4(vert, 1.0f);
	hitbox->initVertices[7] = glm::vec4(vert, 1.0f);
}

void Object::CreateHitboxes_CalcMainHitbox() {
	if (basicObject->controlPoints.size() > 0) {
		glm::vec3 minVec, maxVec, midVert;

		minVec = basicObject->controlPoints[0].coords;
		maxVec = basicObject->controlPoints[0].coords;
		midVert = basicObject->controlPoints[0].coords;

		for (int i = 1; i < basicObject->controlPoints.size(); ++i) {
			for (int j = 0; j < 3; ++j) {
				if (basicObject->controlPoints[i].coords[j] < minVec[j])
					minVec[j] = basicObject->controlPoints[i].coords[j];
				if (basicObject->controlPoints[i].coords[j] > maxVec[j])
					maxVec[j] = basicObject->controlPoints[i].coords[j];
			}
			midVert += glm::vec3(basicObject->controlPoints[i].coords);
		}
		midVert /= static_cast<float>(basicObject->controlPoints.size());

		glm::vec3 trans,vert;

		for (int i = 0; i < 3; ++i) trans[i] = maxVec[i] - minVec[i];
		
		if (trans.x > trans.y) trans.y = trans.x;
		else trans.x = trans.y;
		trans /= 2.0f;

		mainHitbox = new Hitbox;

		vert = midVert; vert.y -= trans.y; vert.x -= trans.x; vert.z -= trans.z;
		mainHitbox->basicVertices[0] = glm::vec4(vert, 1.0f);
		vert = midVert; vert.y -= trans.y; vert.x += trans.x; vert.z -= trans.z;
		mainHitbox->basicVertices[1] = glm::vec4(vert, 1.0f);
		vert = midVert; vert.y -= trans.y; vert.x += trans.x; vert.z += trans.z;
		mainHitbox->basicVertices[2] = glm::vec4(vert, 1.0f);
		vert = midVert; vert.y -= trans.y; vert.x -= trans.x; vert.z += trans.z;
		mainHitbox->basicVertices[3] = glm::vec4(vert, 1.0f);
		vert = midVert; vert.y += trans.y; vert.x -= trans.x; vert.z -= trans.z;
		mainHitbox->basicVertices[4] = glm::vec4(vert, 1.0f);
		vert = midVert; vert.y += trans.y; vert.x += trans.x; vert.z -= trans.z;
		mainHitbox->basicVertices[5] = glm::vec4(vert, 1.0f);
		vert = midVert; vert.y += trans.y; vert.x += trans.x; vert.z += trans.z;
		mainHitbox->basicVertices[6] = glm::vec4(vert, 1.0f);
		vert = midVert; vert.y += trans.y; vert.x -= trans.x; vert.z += trans.z;
		mainHitbox->basicVertices[7] = glm::vec4(vert, 1.0f);

		mainHitbox->jointIdx = -1;
	}
}

void Object::LoadHitboxBufferData() {
	hitboxVerticesCount = (hitboxes.size()+1) * 8 * (3+1);

	hitboxVertices = new float[hitboxVerticesCount];
	hitboxOutVertices = new float[hitboxVerticesCount];

	for (int j = 0; j < 8; ++j) {
		for (int k = 0; k < 3; ++k)
			hitboxVertices[j * 4 + k] = mainHitbox->basicVertices[j][k];
		hitboxVertices[j * 4 + 3] = static_cast<float>(mainHitbox->jointIdx);
	}

	int i = 1;
	for (HitboxMap::iterator it = hitboxes.begin(); it != hitboxes.end();++it) {
		for (unsigned j = 0; j < 8; ++j) {
			for (unsigned k = 0; k < 3; ++k)
				hitboxVertices[i * 32 + j * 4 + k] = it->second->basicVertices[j][k];
			hitboxVertices[i * 32 + j * 4 + 3] = static_cast<float>(it->second->jointIdx);
		}
		++i;
	}
}

void Object::WriteErrorToFile(std::string message) {
	std::fstream file("LOGS/" + basicObject->name + "_ERRORLOG.txt", std::ios::out | std::ios::app);
	assert(file.is_open());
	SYSTEMTIME time;
	GetSystemTime(&time);
	file << time.wDay << "." << time.wMonth << "." << time.wYear << " " << time.wHour << ":" << time.wMinute << ":" << time.wSecond << " - ";
	file << message << std::endl;
	file.close();
}

//-----------------------------------------------------------------------------
//--------------------------------DynamicObject--------------------------------
//-----------------------------------------------------------------------------

void DynamicObject::Init() {
	objectBufferVertexAttribCount = 13;
	hitboxObjectBufferVertexAttribCount = 4;

	animationManager = new AnimationManager(basicObject);

	SetVerticesBuffer();

	shaderManager->SetObjectBufferVertexAttribCount(objectBufferVertexAttribCount);
	shaderManager->SetHitboxObjectBufferVertexAttribCount(hitboxObjectBufferVertexAttribCount);
	shaderManager->Init();
	shaderManager->SetVAO(vertices, verticesCount);
	shaderManager->SetHitboxVAO(hitboxVertices, Hitbox::indices, hitboxVerticesCount);
	shaderManager->SetHitboxComputeBuffers(hitboxVertices, hitboxVerticesCount);
	shaderManager->SetJointsBuffers(animationManager->GetJointsMatricesCount());
	
	glUseProgram(shaderManager->GetHitboxShader()->GetProgram());
	glUniform3fv(shaderManager->GetHitboxColor1Loc(), 1, glm::value_ptr(glm::vec3(1.0f, 0.0f, 0.0f)));
	glUniform3fv(shaderManager->GetHitboxColor2Loc(), 1, glm::value_ptr(glm::vec3(0.0f, 1.0f, 0.0f)));
	glUniform1i(shaderManager->GetHitboxSetJointIdxLoc(), -2);

	memcpy(shaderManager->GetJointsPrevMatBufferPtr(), animationManager->GetJointsPreviousTransformMatrices(), animationManager->GetJointsMatricesCount() * 16 * sizeof(float));
	memcpy(shaderManager->GetJointsNextMatBufferPtr(), animationManager->GetJointsNextTransformMatrices(), animationManager->GetJointsMatricesCount() * 16 * sizeof(float));

	if (!LoadTexture(g_modelsDirectory + basicObject->textureName)) {
		WriteErrorToFile("Error loading texture!");
		WriteErrorToFile("Texture name: " + basicObject->textureName);
		throw std::exception();
	}
}

void DynamicObject::Update() {
	animationManager->Process();

	if (modelUpdate || viewUpdate || projectionUpdate) {
		if (modelUpdate)
			model = basicObject->globalTransform*model;

		glUseProgram(shaderManager->GetMainShader()->GetProgram());
		if (modelUpdate) glUniformMatrix4fv(shaderManager->GetModelLoc(), 1, GL_FALSE, glm::value_ptr(model));
		if (viewUpdate) glUniformMatrix4fv(shaderManager->GetViewLoc(), 1, GL_FALSE, glm::value_ptr(view));
		if (projectionUpdate)glUniformMatrix4fv(shaderManager->GetProjectionLoc(), 1, GL_FALSE, glm::value_ptr(projection));

		glUseProgram(shaderManager->GetHitboxShader()->GetProgram());
		if (modelUpdate) glUniformMatrix4fv(shaderManager->GetModelHitboxComputeLoc(), 1, GL_FALSE, glm::value_ptr(model));
		if (viewUpdate) glUniformMatrix4fv(shaderManager->GetViewHitboxLoc(), 1, GL_FALSE, glm::value_ptr(view));
		if (projectionUpdate) glUniformMatrix4fv(shaderManager->GetProjectionHitboxLoc(), 1, GL_FALSE, glm::value_ptr(projection));
		glUniform1i(shaderManager->GetHitboxSetJointIdxLoc(), currentHitboxJointIdx);

		modelUpdate = viewUpdate = projectionUpdate = false;
	}

	if (animationManager->GetJointsMatChangeStatus() == true) {
		memcpy(shaderManager->GetJointsPrevMatBufferPtr(), animationManager->GetJointsPreviousTransformMatrices(), animationManager->GetJointsMatricesCount() * 16 * sizeof(float));
		memcpy(shaderManager->GetJointsNextMatBufferPtr(), animationManager->GetJointsNextTransformMatrices(), animationManager->GetJointsMatricesCount() * 16 * sizeof(float));
	}

	UpdateHitboxes();
}

void DynamicObject::Draw() {
	static unsigned shaderProgram = shaderManager->GetMainShader()->GetProgram();
	static int interpolatioLoc = shaderManager->GetInterpolationLoc();
	static int modelLoc = shaderManager->GetModelLoc();
	static int texAvailableLoc = shaderManager->GetTextureAvailableLoc();
	static unsigned texture = shaderManager->GetTexture();
	static unsigned VAO = shaderManager->GetVAO();
	static unsigned prevMatBuffer = shaderManager->GetJointsPrevMatricesBuffer();
	static unsigned nextMatBuffer = shaderManager->GetJointsNextMatricesBuffer();
	static unsigned hitboxShaderProgram = shaderManager->GetHitboxShader()->GetProgram();
	static int hitboxInterpolationLoc = shaderManager->GetHitboxInterpolationLoc();
	static int hitboxModelLoc = shaderManager->GetModelHitboxLoc();
	static unsigned hitboxVAO = shaderManager->GetHitboxVAO();

	static unsigned jointsCount = animationManager->GetJointsMatricesCount();

	glUseProgram(shaderProgram);
	glUniform1f(interpolatioLoc, animationManager->GetInterpolationVal());
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform1i(texAvailableLoc, textureAvailable);

	if (textureAvailable) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
	}
	glBindVertexArray(VAO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, prevMatBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, nextMatBuffer);

	glDrawArrays(GL_TRIANGLES, 0, verticesCount / objectBufferVertexAttribCount);

	glUseProgram(hitboxShaderProgram);

	glUniform1f(hitboxInterpolationLoc, animationManager->GetInterpolationVal());
	glUniformMatrix4fv(hitboxModelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(hitboxVAO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, prevMatBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, nextMatBuffer);

	glDrawElementsBaseVertex(GL_LINES, 24, GL_UNSIGNED_INT, 0, 0);

	int i = 1;
	for (HitboxMap::iterator it = hitboxes.begin(); it != hitboxes.end(); ++it) {
		int startVal = 8 * i;
		glDrawElementsBaseVertex(GL_LINES, 24, GL_UNSIGNED_INT, 0, startVal);
		++i;
	}
}

//-----------------------------------------------------------------------------
//---------------------------DynamicObject::Private----------------------------
//-----------------------------------------------------------------------------

void DynamicObject::SetVerticesBuffer() {
	verticesCount = basicObject->faces.size() * 3 * 13;
	vertices = new float[verticesCount];
	for (unsigned i = 0; i < basicObject->faces.size(); ++i) {
		for (unsigned j = 0; j < 3; ++j) {
			vertices[i * objectBufferVertexAttribCount * 3 + j * objectBufferVertexAttribCount + 0] = basicObject->controlPoints[basicObject->faces[i].indices[j]].coords.x;
			vertices[i * objectBufferVertexAttribCount * 3 + j * objectBufferVertexAttribCount + 1] = basicObject->controlPoints[basicObject->faces[i].indices[j]].coords.y;
			vertices[i * objectBufferVertexAttribCount * 3 + j * objectBufferVertexAttribCount + 2] = basicObject->controlPoints[basicObject->faces[i].indices[j]].coords.z;
			vertices[i * objectBufferVertexAttribCount * 3 + j * objectBufferVertexAttribCount + 3] = basicObject->faces[i].uv[j].x;
			vertices[i * objectBufferVertexAttribCount * 3 + j * objectBufferVertexAttribCount + 4] = basicObject->faces[i].uv[j].y;
			for (unsigned k = 0; k < 4; ++k) {
				vertices[i * objectBufferVertexAttribCount * 3 + j * objectBufferVertexAttribCount + 2 * k + 5] = basicObject->controlPoints[basicObject->faces[i].indices[j]].blendingInfo[k].jointIndex;
				vertices[i * objectBufferVertexAttribCount * 3 + j * objectBufferVertexAttribCount + 2 * k + 1 + 5] = basicObject->controlPoints[basicObject->faces[i].indices[j]].blendingInfo[k].weight;
			}
		}
	}
}

void DynamicObject::UpdateHitboxes() {
	static unsigned computeShaderProgram = shaderManager->GetHitboxComputeShader()->GetProgram();
	static int modelLoc = shaderManager->GetModelHitboxComputeLoc();
	static int interpolationLoc = shaderManager->GetHitboxComputeInterpolationLoc();
	static unsigned computeInBuffer = shaderManager->GetHitboxComputeInBuffer();
	static unsigned computeOutBuffer = shaderManager->GetHitboxComputeOutBuffer();
	static unsigned prevMatBuffer = shaderManager->GetJointsPrevMatricesBuffer();
	static unsigned nextMatBuffer = shaderManager->GetJointsNextMatricesBuffer();

	if (updateHitboxVerticesInBuffer != 0) {
		glNamedBufferSubData(shaderManager->GetHitboxVBO(), 0, hitboxVerticesCount * sizeof(float), hitboxVertices);
		glNamedBufferSubData(shaderManager->GetHitboxComputeInBuffer(), 0, hitboxVerticesCount * sizeof(float), hitboxVertices);
		updateHitboxVerticesInBuffer = 0;
	}

	glUseProgram(computeShaderProgram);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform1f(interpolationLoc, animationManager->GetInterpolationVal());

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, computeInBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, computeOutBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, prevMatBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, nextMatBuffer);

	glDispatchCompute(hitboxes.size()+1, 1, 1);
	
	memcpy(hitboxOutVertices, shaderManager->GetHitboxComputeOutBuferPtr(), hitboxVerticesCount * sizeof(float));
}

//-----------------------------------------------------------------------------
//--------------------------------StaticObject---------------------------------
//-----------------------------------------------------------------------------

void StaticObject::Init() {
	objectBufferVertexAttribCount = 5;
	hitboxObjectBufferVertexAttribCount = 4;

	animationManager = nullptr;

	SetVerticesBuffer();

	shaderManager->SetObjectBufferVertexAttribCount(objectBufferVertexAttribCount);
	shaderManager->SetHitboxObjectBufferVertexAttribCount(hitboxObjectBufferVertexAttribCount);
	shaderManager->Init();
	shaderManager->SetVAO(vertices, verticesCount);
	shaderManager->SetHitboxVAO(hitboxVertices, Hitbox::indices, hitboxVerticesCount);
	shaderManager->SetHitboxComputeBuffers(hitboxVertices, hitboxVerticesCount);

	glUseProgram(shaderManager->GetHitboxShader()->GetProgram());
	glUniform3fv(shaderManager->GetHitboxColor1Loc(), 1, glm::value_ptr(glm::vec3(1.0f, 0.0f, 0.0f)));
	glUniform3fv(shaderManager->GetHitboxColor2Loc(), 1, glm::value_ptr(glm::vec3(0.0f, 1.0f, 0.0f)));

	if (!LoadTexture(g_modelsDirectory + basicObject->textureName)) {
		WriteErrorToFile("Error loading texture!");
		WriteErrorToFile("Texture name: " + basicObject->textureName);
		throw std::exception();
	}
}

void StaticObject::Update() {
	if (modelUpdate || viewUpdate || projectionUpdate) {
		if (modelUpdate) 
			model = basicObject->globalTransform*model;

		glUseProgram(shaderManager->GetMainShader()->GetProgram());
		if (modelUpdate) glUniformMatrix4fv(shaderManager->GetModelLoc(), 1, GL_FALSE, glm::value_ptr(model));
		if (viewUpdate) glUniformMatrix4fv(shaderManager->GetViewLoc(), 1, GL_FALSE, glm::value_ptr(view));
		if (projectionUpdate)glUniformMatrix4fv(shaderManager->GetProjectionLoc(), 1, GL_FALSE, glm::value_ptr(projection));

		glUseProgram(shaderManager->GetHitboxShader()->GetProgram());
		if (modelUpdate) glUniformMatrix4fv(shaderManager->GetModelHitboxComputeLoc(), 1, GL_FALSE, glm::value_ptr(model));
		if (viewUpdate) glUniformMatrix4fv(shaderManager->GetViewHitboxLoc(), 1, GL_FALSE, glm::value_ptr(view));
		if (projectionUpdate) glUniformMatrix4fv(shaderManager->GetProjectionHitboxLoc(), 1, GL_FALSE, glm::value_ptr(projection));

		glUniform1i(shaderManager->GetHitboxSetJointIdxLoc(), currentHitboxJointIdx);

		modelUpdate = viewUpdate = projectionUpdate = false;
	}

	UpdateHitboxes();
}

void StaticObject::Draw() {
	glUseProgram(shaderManager->GetMainShader()->GetProgram());
	glUniformMatrix4fv(shaderManager->GetModelLoc(), 1, GL_FALSE, glm::value_ptr(model));

	glUniform1i(shaderManager->GetTextureAvailableLoc(), textureAvailable);

	if (textureAvailable) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, shaderManager->GetTexture());
	}

	glBindVertexArray(shaderManager->GetVAO());
	glDrawArrays(GL_TRIANGLES, 0, verticesCount / objectBufferVertexAttribCount);

	glUseProgram(shaderManager->GetHitboxShader()->GetProgram());

	glUniformMatrix4fv(shaderManager->GetModelHitboxLoc(), 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(shaderManager->GetHitboxVAO());

	glDrawElementsBaseVertex(GL_LINES, 24, GL_UNSIGNED_INT, 0, 0);

	int i = 1;
	for (HitboxMap::iterator it = hitboxes.begin(); it != hitboxes.end(); ++it) {
		int startVal = 8 * i;
		glDrawElementsBaseVertex(GL_LINES, 24, GL_UNSIGNED_INT, 0, startVal);
		++i;
	}
}

//-----------------------------------------------------------------------------
//----------------------------StaticObject::Private----------------------------
//-----------------------------------------------------------------------------

void StaticObject::SetVerticesBuffer() {
	verticesCount = basicObject->faces.size() * 3 * 5;
	vertices = new float[verticesCount];
	for (unsigned i = 0; i < basicObject->faces.size(); ++i) {
		for (unsigned j = 0; j < 3; ++j) {
			vertices[i * objectBufferVertexAttribCount * 3 + j * objectBufferVertexAttribCount + 0] = basicObject->controlPoints[basicObject->faces[i].indices[j]].coords.x;
			vertices[i * objectBufferVertexAttribCount * 3 + j * objectBufferVertexAttribCount + 1] = basicObject->controlPoints[basicObject->faces[i].indices[j]].coords.y;
			vertices[i * objectBufferVertexAttribCount * 3 + j * objectBufferVertexAttribCount + 2] = basicObject->controlPoints[basicObject->faces[i].indices[j]].coords.z;
			vertices[i * objectBufferVertexAttribCount * 3 + j * objectBufferVertexAttribCount + 3] = basicObject->faces[i].uv[j].x;
			vertices[i * objectBufferVertexAttribCount * 3 + j * objectBufferVertexAttribCount + 4] = basicObject->faces[i].uv[j].y;
		}
	}
}

void StaticObject::UpdateHitboxes() {
	if (shaderManager->GetHitboxComputeShader() != nullptr) {
		glUseProgram(shaderManager->GetHitboxComputeShader()->GetProgram());
		glUniformMatrix4fv(shaderManager->GetModelHitboxComputeLoc(), 1, GL_FALSE, glm::value_ptr(model));

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, shaderManager->GetHitboxComputeInBuffer());
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, shaderManager->GetHitboxComputeOutBuffer());

		glDispatchCompute(hitboxes.size()+1, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		void* ptr = nullptr;
		ptr = glMapNamedBuffer(shaderManager->GetHitboxComputeOutBuffer(), GL_READ_ONLY);
		if (ptr != nullptr) memcpy(hitboxOutVertices, ptr, hitboxVerticesCount * sizeof(float));
		glUnmapNamedBuffer(shaderManager->GetHitboxComputeOutBuffer());
	}
}
