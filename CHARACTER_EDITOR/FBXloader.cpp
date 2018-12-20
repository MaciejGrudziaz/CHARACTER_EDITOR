#include "FBXloader.h"

FBXloader* FBXloader::GetInstance() {
	static FBXloader loader;
	return &loader;
}

int FBXloader::LoadModel(const char* filename, BasicModel* model) {
	errorCode = 0;
	FbxManager* lSdkManager = FbxManager::Create();
	FbxIOSettings* ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
	lSdkManager->SetIOSettings(ios);
	FbxImporter* lImporter = FbxImporter::Create(lSdkManager, "");

	if (!lImporter->Initialize(filename, -1, lSdkManager->GetIOSettings())) {
		WriteErrorToFile("Importing file: " + static_cast<std::string>(filename));
		WriteErrorToFile("Call to FbxImporter::Initialize() failed");
		WriteErrorToFile("Error returned: " + static_cast<std::string>(lImporter->GetStatus().GetErrorString()));

		errorCode = 1;
		return errorCode;
		//throw std::exception();
	}

	FbxScene* lScene = FbxScene::Create(lSdkManager, "scene");
	lImporter->Import(lScene);

	FbxNode* lRootNode = lScene->GetRootNode();
	FbxNode* meshNode=nullptr;
	FbxMesh* mesh=nullptr;
	unsigned n = lRootNode->GetChildCount();
	FbxNode* child;
	FbxNodeAttribute::EType attrib;
	for (unsigned i = 0; i < n; ++i) {
		child = lRootNode->GetChild(i);
		attrib = (child->GetNodeAttribute()->GetAttributeType());
		if(attrib==FbxNodeAttribute::eMesh){
			BasicObject* object = new BasicObject();
			meshNode = child;
			mesh= static_cast<FbxMesh*>(child->GetNodeAttribute());

			object->name = mesh->GetName();
			LoadVertices(object, mesh);
			LoadGlobalTransformMatrix(object, meshNode);
			LoadFaces(object, mesh);
			LoadNormals(object, mesh);
			LoadUV(object, mesh);
			LoadTextures(object, meshNode);
			ProcessSkeletonHierarchy(object, lRootNode);
			ProcessJointsAndAnimations(object, meshNode, lScene);

			model->LoadObject(object);
		}
	}

	lImporter->Destroy();
	lSdkManager->Destroy();

	return errorCode;
}

//-----------------------------------------------------------------------------
//-----------------------------------PRIVATE-----------------------------------
//-----------------------------------------------------------------------------

void FBXloader::GetMeshNode(FbxNode* rootNode, FbxNode** meshNode, FbxMesh** mesh) {
	*meshNode = nullptr; *mesh = nullptr;
	FbxNode* child;
	FbxNodeAttribute::EType attrib;
	unsigned n = rootNode->GetChildCount();
	for (unsigned i = 0; i < n; ++i) {
		child = rootNode->GetChild(i);
		attrib = (child->GetNodeAttribute()->GetAttributeType());
		if (attrib == FbxNodeAttribute::eMesh) {
			*meshNode = child;
			*mesh = static_cast<FbxMesh*>(child->GetNodeAttribute());
			break;
		}
	}
}

void FBXloader::LoadVertices(BasicObject* object,FbxMesh* mesh) {
	/*object->verticesCount = mesh->GetControlPointsCount();
	object->vertices = new glm::vec4[object->verticesCount];
	FbxVector4* vertices = mesh->GetControlPoints();
	for (unsigned i = 0; i < object->verticesCount; ++i) {
		object->vertices[i].x = vertices[i].mData[0];
		object->vertices[i].y = vertices[i].mData[1];
		object->vertices[i].z = vertices[i].mData[2];
		object->vertices[i].w = vertices[i].mData[3];
	}*/
	BasicObject::ControlPoint currControlPoint;
	FbxVector4* fbxControlPoints = mesh->GetControlPoints();
	for (unsigned i = 0; i < mesh->GetControlPointsCount(); ++i) {
		currControlPoint.coords.x = fbxControlPoints[i].mData[0];
		currControlPoint.coords.y = fbxControlPoints[i].mData[1];
		currControlPoint.coords.z = fbxControlPoints[i].mData[2];
		currControlPoint.coords.w = fbxControlPoints[i].mData[3];
		object->controlPoints.push_back(currControlPoint);
	}
}

void FBXloader::LoadGlobalTransformMatrix(BasicObject* object, FbxNode* meshNode) {
	FbxAMatrix globalTransform = meshNode->EvaluateGlobalTransform(FBXSDK_TIME_INFINITE);

	for(int i=0;i<4;++i)
		for(int j=0;j<4;++j)
			object->globalTransform[i][j] = globalTransform.mData[i][j];
}

void FBXloader::LoadFaces(BasicObject* object,FbxMesh* mesh) {
	/*object->facesCount = mesh->GetPolygonCount();
	object->faces = new BasicObject::Face[object->facesCount];
	int* indices = mesh->GetPolygonVertices();
	for (int i = 0; i < object->facesCount; ++i) {
		for (int j = 0; j < 3; ++j) {
			object->faces[i].indices[j] = indices[3 * i + j];
		}
	}*/
	int* indices = mesh->GetPolygonVertices();
	BasicObject::Face currFace;
	for (unsigned i = 0; i < mesh->GetPolygonCount(); ++i) {
		for (int j = 0; j < 3; ++j)
			currFace.indices[j] = indices[3 * i + j];
		object->faces.push_back(currFace);
	}
}

void FBXloader::LoadNormals(BasicObject* object, FbxMesh* mesh) {
	unsigned normalCount = mesh->GetElementNormalCount();
	if (normalCount > 0) {
		FbxLayerElementNormal* vertexNormal = mesh->GetElementNormal();
		if (vertexNormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex) {
			glm::vec4 normal;
			normal.w = 0.0;
			for (unsigned i = 0; i < object->faces.size(); ++i) {
				for (unsigned j = 0; j < 3; ++j) {
					switch (vertexNormal->GetReferenceMode()) {
					case FbxGeometryElement::eDirect:
					{
						normal.x = vertexNormal->GetDirectArray().GetAt(i * 3 + j).mData[0];
						normal.y = vertexNormal->GetDirectArray().GetAt(i * 3 + j).mData[1];
						normal.z = vertexNormal->GetDirectArray().GetAt(i * 3 + j).mData[2];
					}
					break;
					case FbxGeometryElement::eIndexToDirect:
					{
						int index = vertexNormal->GetIndexArray().GetAt(i * 3 + j);
						normal.x = vertexNormal->GetDirectArray().GetAt(index).mData[0];
						normal.y = vertexNormal->GetDirectArray().GetAt(index).mData[1];
						normal.z = vertexNormal->GetDirectArray().GetAt(index).mData[2];
					}
					break;
					default:
						throw std::exception();
					}
					object->faces[i].normals[j] = normal;
				}
			}
		}
		else throw std::exception();
	}
}

void FBXloader::LoadUV(BasicObject* object,FbxMesh* mesh) {
	unsigned uvCount = mesh->GetElementUVCount();
	if (uvCount > 0) {
		FbxLayerElementUV* vertexUV = mesh->GetElementUV();
		if (vertexUV->GetMappingMode() == FbxGeometryElement::eByPolygonVertex) {
			glm::vec2 uv;
			for (unsigned i = 0; i < object->faces.size(); ++i) {
				for (unsigned j = 0; j < 3; ++j) {
					switch (vertexUV->GetReferenceMode()) {
					case FbxGeometryElement::eDirect:
					{
						uv.x = vertexUV->GetDirectArray().GetAt(i * 3 + j).mData[0];
						uv.y = vertexUV->GetDirectArray().GetAt(i * 3 + j).mData[1];
					}
					break;
					case FbxGeometryElement::eIndexToDirect:
					{
						int index = vertexUV->GetIndexArray().GetAt(i * 3 + j);
						uv.x = vertexUV->GetDirectArray().GetAt(index).mData[0];
						uv.y = vertexUV->GetDirectArray().GetAt(index).mData[1];
					}
					break;
					default:
						throw std::exception();
					}
					object->faces[i].uv[j] = uv;
				}
			}
		}
		else throw std::exception();
	}
}

void FBXloader::LoadTextures(BasicObject* object,FbxNode* meshNode) {
	int mcount = meshNode->GetSrcObjectCount<FbxSurfaceMaterial>();
	for (int i = 0; i < mcount; ++i) {
		FbxSurfaceMaterial* material = (FbxSurfaceMaterial*)(meshNode->GetSrcObject<FbxSurfaceMaterial>(i));
		if (material) {
			FbxProperty prop = material->FindProperty(FbxSurfaceMaterial::sDiffuse);
			if (prop.IsValid()) {
				int layered_texture_count = prop.GetSrcObjectCount<FbxLayeredTexture>();
				if (layered_texture_count > 0) {
					for (int j = 0; j < layered_texture_count; ++j) {
						FbxLayeredTexture* layered_texture = FbxCast<FbxLayeredTexture>(prop.GetSrcObject<FbxLayeredTexture>(j));
						int lcount = layered_texture->GetSrcObjectCount<FbxTexture>();
						for (int k = 0; k < lcount; ++k) {
							FbxTexture* texture = FbxCast<FbxTexture>(layered_texture->GetSrcObject<FbxTexture>(k));
							FbxFileTexture* texFile = static_cast<FbxFileTexture*>(texture);
							object->textureName = texFile->GetFileName();
						}
					}
				}
				else {
					int texture_count = prop.GetSrcObjectCount<FbxTexture>();
					for (int j = 0; j < texture_count; ++j) {
						FbxTexture* texture = FbxCast<FbxTexture>(prop.GetSrcObject<FbxTexture>(j));
						FbxFileTexture* texFile = static_cast<FbxFileTexture*>(texture);
						object->textureName = texFile->GetFileName();
					}
				}
				unsigned idx = object->textureName.find_last_of('\\');
				object->textureName.erase(0, idx + 1);
			}
			else throw std::exception();
		}
		else throw std::exception();
	}
}

void FBXloader::ProcessSkeletonHierarchy(BasicObject* object, FbxNode* inRootNode) {
	for (int childIdx = 0; childIdx < inRootNode->GetChildCount(); ++childIdx) {
		FbxNode* currNode = inRootNode->GetChild(childIdx);
		ProcessSkeletonHierarchyRecursively(object,currNode, 0, -1);
	}
}

void FBXloader::ProcessSkeletonHierarchyRecursively(BasicObject* object, FbxNode* inNode, int myIndex, int inParentIndex) {
	if (inNode->GetNodeAttribute() && inNode->GetNodeAttribute()->GetAttributeType() &&
		inNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton) {
		BasicObject::Joint* currJoint=new BasicObject::Joint;
		currJoint->parentIndex = inParentIndex;
		currJoint->name = inNode->GetName();
		object->skeleton.joints.push_back(currJoint);
	}
	for (int i = 0; i < inNode->GetChildCount(); ++i) {
		ProcessSkeletonHierarchyRecursively(object, inNode->GetChild(i), object->skeleton.joints.size(), myIndex);
	}
}

void FBXloader::ProcessJointsAndAnimations(BasicObject* object, FbxNode* inNode, FbxScene* mFBXScene) {
	FbxMesh* currMesh = inNode->GetMesh();
	unsigned numOfDeformers = currMesh->GetDeformerCount();

	FbxAMatrix geometryTransform = GetGeometryTransformation(inNode);

	for (unsigned deformerIndex = 0; deformerIndex < numOfDeformers; ++deformerIndex) {
		FbxSkin* currSkin = static_cast<FbxSkin*>(currMesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));
		if (!currSkin) continue;

		unsigned int numOfClusters = currSkin->GetClusterCount();
		for (unsigned clusterIndex = 0; clusterIndex < numOfClusters; ++clusterIndex) {
			FbxCluster* currCluster = currSkin->GetCluster(clusterIndex);
			std::string currJointName = currCluster->GetLink()->GetName();
			int currJointIndex = object->skeleton.FindJointIndexUsingName(currJointName);
			FbxAMatrix transformMatrix;
			FbxAMatrix transformLinkMatrix;
			FbxAMatrix globalBindPoseInverseMatrix;
			currCluster->GetTransformMatrix(transformMatrix);
			currCluster->GetTransformLinkMatrix(transformLinkMatrix);
			globalBindPoseInverseMatrix = transformLinkMatrix.Inverse() * transformMatrix * geometryTransform;

			//object->skeleton.joints[currJointIndex].globalBindposeInverse = globalBindPoseInverseMatrix;
			LoadFbxMatToGlmMat(globalBindPoseInverseMatrix, object->skeleton.joints[currJointIndex]->globalBindposeInverse);

			unsigned numOfIndices = currCluster->GetControlPointIndicesCount();
			for (unsigned i = 0; i < numOfIndices; ++i) {
				BasicObject::BlendingIndexWeightPair currBlendingIndexWeightPair;
				currBlendingIndexWeightPair.jointIndex = currJointIndex;
				currBlendingIndexWeightPair.weight = (currCluster->GetControlPointWeights()[i]);
				object->controlPoints[(currCluster->GetControlPointIndices())[i]].blendingInfo.push_back(currBlendingIndexWeightPair);
			}

			FbxAnimEvaluator* sceneEvaluator = mFBXScene->GetAnimationEvaluator();

			for (unsigned i = 0; i < mFBXScene->GetSrcObjectCount<FbxAnimStack>(); ++i) {
				BasicObject::AnimationJoint* currAnimation=new BasicObject::AnimationJoint;
				FbxAnimStack* currAnimStack = mFBXScene->GetSrcObject<FbxAnimStack>(i);
				mFBXScene->SetCurrentAnimationStack(currAnimStack);

				FbxString animStackName = currAnimStack->GetName();
				currAnimation->name = animStackName.Buffer();
				size_t pos=currAnimation->name.find('|', 0);
				currAnimation->name.erase(0, pos + 1);
				FbxTakeInfo* takeInfo = mFBXScene->GetTakeInfo(animStackName);
				FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
				FbxTime end = takeInfo->mLocalTimeSpan.GetStop();
				currAnimation->frameCount = end.GetFrameCount(FbxTime::eFrames24) - start.GetFrameCount(FbxTime::eFrames24) + 1;
				object->skeleton.joints[currJointIndex]->animations.push_back(currAnimation);

				//BasicObject::Keyframe** currFrame = &object->skeleton.joints[currJointIndex].animations[i].frames;
				BasicObject::Keyframe* currFrame;
				for (FbxLongLong j = start.GetFrameCount(FbxTime::eFrames24); j <= end.GetFrameCount(FbxTime::eFrames24); ++j) {
					FbxTime currTime;
					currTime.SetFrame(j, FbxTime::eFrames24);
					//*currFrame = new BasicObject::Keyframe();
					currFrame = new BasicObject::Keyframe();
					//if (object->skeleton.joints[currJointIndex].animations[i].firstFrame == nullptr) 
					//	object->skeleton.joints[currJointIndex].animations[i].firstFrame = *currFrame;
					//(*currFrame)->frameNum = j;
					currFrame->frameNum = j;
					FbxAMatrix currentTransformOffset = inNode->EvaluateGlobalTransform(currTime) *geometryTransform;
					//(*currFrame)->globalTransform = currentTransformOffset.Inverse()*currCluster->GetLink()->EvaluateGlobalTransform(currTime);
					//currFrame->globalTransform = currentTransformOffset.Inverse()*currCluster->GetLink()->EvaluateGlobalTransform(currTime);
					LoadFbxMatToGlmMat(currentTransformOffset.Inverse()*currCluster->GetLink()->EvaluateGlobalTransform(currTime), currFrame->globalTransform);
					//currFrame = &((*currFrame)->nextFrame);
					object->skeleton.joints[currJointIndex]->animations[i]->frames.push_back(currFrame);
				}
			}
		}
	}

	BasicObject::BlendingIndexWeightPair currBlendingIndexWeightPair;
	currBlendingIndexWeightPair.jointIndex = 0;
	currBlendingIndexWeightPair.weight = 0;
	for (auto itr = object->controlPoints.begin(); itr != object->controlPoints.end(); ++itr) {
		for (unsigned i = itr->blendingInfo.size(); i < 4; ++i) {
			itr->blendingInfo.push_back(currBlendingIndexWeightPair);
		}
	}

	if (object->skeleton.joints.size() > 0) {
		for (unsigned i = 0; i < object->skeleton.joints[0]->animations.size(); ++i) {
			BasicObject::AnimationInfo animInfo;
			animInfo.name = object->skeleton.joints[0]->animations[i]->name;
			animInfo.frameCount = object->skeleton.joints[0]->animations[i]->frameCount;
			object->animationsInfo.push_back(animInfo);
		}
	}
}

FbxAMatrix FBXloader::GetGeometryTransformation(FbxNode* inNode) {
	if (!inNode)
		throw std::exception("Null for mesh geometry");

	const FbxVector4 lT = inNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 lR = inNode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 lS = inNode->GetGeometricScaling(FbxNode::eSourcePivot);

	return FbxAMatrix(lT, lR, lS);
}

void FBXloader::LoadFbxMatToGlmMat(const FbxAMatrix& fbxMat, glm::mat4& glmMat) {
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			glmMat[i][j] = fbxMat.mData[i][j];
}

void FBXloader::WriteErrorToFile(std::string message) {
	std::fstream file("FBXloader_error_log.txt", std::ios::out | std::ios::app);
	assert(file.is_open());
	SYSTEMTIME time;
	GetSystemTime(&time);
	file << time.wDay << "." << time.wMonth << "." << time.wYear << " " << time.wHour << ":" << time.wMinute << ":" << time.wSecond << " - ";
	file << message << std::endl;
	file.close();
}
