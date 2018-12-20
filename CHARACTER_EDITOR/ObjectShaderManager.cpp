#include "ObjectShaderManager.h"

ObjectShaderManager::ObjectShaderManager():shaderProgram(nullptr),hitboxShaderProgram(nullptr),hitboxComputeShaderProgram(nullptr),modelLoc(-1),viewLoc(-1),projectionLoc(-1),modelHitboxLoc(-1),viewHitboxLoc(-1),
projectionHitboxLoc(-1),modelHitboxComputeLoc(-1),VBO(0xFFFFFFFF),VAO(0xFFFFFFFF),texture(0xFFFFFFFF),hitboxVAO(0xFFFFFFFF),hitboxVBO(0xFFFFFFFF),hitboxIndicesBuffer(0xFFFFFFFF),
hitboxComputeOutBuffer(0xFFFFFFFF),hitboxComputeInBuffer(0xFFFFFFFF),textureAvailableLoc(-1),jointsPrevMatricesBuffer(0xFFFFFFFF),jointsNextMatricesBuffer(0xFFFFFFFF),interpolationLoc(-1),
hitboxInterpolationLoc(-1),hitboxComputeInterpoaltionLoc(-1),jointsPrevMatBufferPtr(nullptr),jointsNextMatBufferPtr(nullptr){ }

int ObjectShaderManager::LoadMainShaderProgram(Shader* shaderProgram_) {
	if (shaderProgram_ != nullptr) {
		shaderProgram = shaderProgram_;
		return 1;
	}
	else return 0;
}

int ObjectShaderManager::LoadHitboxVisualizationShaderProgram(Shader* shaderProgram_) {
	if (shaderProgram_ != nullptr) {
		hitboxShaderProgram = shaderProgram_;
		return 1;
	}
	else return 0;
}

int ObjectShaderManager::LoadHitboxComputeShaderProgram(Shader* shaderProgram_) {
	if (shaderProgram_ != nullptr) {
		hitboxComputeShaderProgram = shaderProgram_;
		return 1;
	}
	else return 0;
}

void ObjectShaderManager::SetHitboxComputeBuffers(float* verticesBuffer_, int verticesCount_) {
	glUseProgram(hitboxComputeShaderProgram->GetProgram());

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, hitboxComputeInBuffer);
	//glBufferData(GL_SHADER_STORAGE_BUFFER, verticesCount_ * sizeof(float), verticesBuffer_, GL_STATIC_DRAW);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, verticesCount_ * sizeof(float), verticesBuffer_, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, hitboxComputeOutBuffer);
	//glBufferData(GL_SHADER_STORAGE_BUFFER, verticesCount_ * sizeof(float), NULL, GL_DYNAMIC_COPY);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, verticesCount_ * sizeof(float), NULL, GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, hitboxComputeInBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, hitboxComputeOutBuffer);

	hitboxComputeOutBufferPtr = glMapNamedBufferRange(hitboxComputeOutBuffer, 0,verticesCount_*sizeof(float),GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
}

void ObjectShaderManager::LoadTexture(const char* filename) {
	if (filename == "") {
		texture = 0;
	}
	else {
		if (texture != 0)
			glDeleteTextures(1, &texture);

		glGenTextures(1, &texture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		int width, height, nrChannels;

		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
		if (!data) {
			WriteErrorToFile("Error in texture loading!");
			WriteErrorToFile(filename);
			assert(data);
		}

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(data);
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------PRIVATE-----------------------------------
//-----------------------------------------------------------------------------

void ObjectShaderManager::GenerateBuffersAndArrays() {
	glGenVertexArrays(1, &VAO);
	glGenVertexArrays(1, &hitboxVAO);

	glGenBuffers(1, &VBO);
	glGenBuffers(1, &hitboxVBO);
	glGenBuffers(1, &hitboxIndicesBuffer);
	glGenBuffers(1, &hitboxComputeInBuffer);
	glGenBuffers(1, &hitboxComputeOutBuffer);
}

void ObjectShaderManager::LoadModelMatrixLoc() {
	modelLoc = glGetUniformLocation(shaderProgram->GetProgram(), "model");
	if (modelLoc < 0) {
		WriteErrorToFile("Character transform matrix location not found!");
		throw std::exception();
	}
}

void ObjectShaderManager::LoadViewMatrixLoc() {
	viewLoc = glGetUniformLocation(shaderProgram->GetProgram(), "view");
	if (viewLoc < 0) {
		WriteErrorToFile("View transform matrix location not found!");
		throw std::exception();
	}
}

void ObjectShaderManager::LoadProjectionMatrixLoc() {
	projectionLoc = glGetUniformLocation(shaderProgram->GetProgram(), "projection");
	if (projectionLoc < 0) {
		WriteErrorToFile("Projection transform matrix location not found!");
		throw std::exception();
	}
}

void ObjectShaderManager::LoadTextureAvailableLoc() {
	textureAvailableLoc = glGetUniformLocation(shaderProgram->GetProgram(), "textureAvailable");
	if (textureAvailableLoc < 0) {
		WriteErrorToFile("Texture available switch location not found!");
		throw std::exception();
	}
}

void ObjectShaderManager::LoadHitboxModelMatrixLoc() {
	modelHitboxLoc = glGetUniformLocation(hitboxShaderProgram->GetProgram(), "model");
	if (modelHitboxLoc < 0) {
		WriteErrorToFile("Hitbox model matrix location not found!");
		throw std::exception();
	}
}

void ObjectShaderManager::LoadHitboxViewMatrixLoc() {
	viewHitboxLoc = glGetUniformLocation(hitboxShaderProgram->GetProgram(), "view");
	if (viewHitboxLoc < 0) {
		WriteErrorToFile("Hitbox view matrix location not found!");
		throw std::exception();
	}
}

void ObjectShaderManager::LoadHitboxProjectionMatrixLoc() {
	projectionHitboxLoc = glGetUniformLocation(hitboxShaderProgram->GetProgram(), "projection");
	if (projectionHitboxLoc < 0) {
		WriteErrorToFile("Hitbox projection matrix location not found!");
		throw std::exception();
	}
}

void ObjectShaderManager::LoadHitboxColor1Loc() {
	hitboxColor1Loc = glGetUniformLocation(hitboxShaderProgram->GetProgram(), "color1");
	if (hitboxColor1Loc < 0) {
		WriteErrorToFile("Hitbox color1 value location not found!");
		throw std::exception();
	}
}

void ObjectShaderManager::LoadHitboxColor2Loc() {
	hitboxColor2Loc = glGetUniformLocation(hitboxShaderProgram->GetProgram(), "color2");
	if (hitboxColor2Loc < 0) {
		WriteErrorToFile("Hitbox color2 value location not found!");
		throw std::exception();
	}
}

void ObjectShaderManager::LoadHitboxSetCurrJointIdxLoc() {
	hitboxSetJointIdxLoc = glGetUniformLocation(hitboxShaderProgram->GetProgram(), "setJointIdx");
	if (hitboxSetJointIdxLoc < 0) {
		WriteErrorToFile("Hitbox setJointIdx value location not found!");
		throw std::exception();
	}
}

void ObjectShaderManager::LoadHitboxComputeModelMatrixLoc() {
	modelHitboxComputeLoc = glGetUniformLocation(hitboxComputeShaderProgram->GetProgram(), "model");
	if (modelHitboxComputeLoc < 0) {
		WriteErrorToFile("Hitbox compute model transform matrix location not found!");
		throw std::exception();
	}
}

//void ObjectShaderManager::LoadJointsPreviousMatricesLoc() {
//	jointsPrevMatricesLoc = glGetUniformLocation(shaderProgram->GetProgram(), "prevJoints");
//	if (jointsPrevMatricesLoc < 0) {
//		WriteErrorToFile("Previous joints matrices location not found!");
//		throw std::exception();
//	}
//}
//
//void ObjectShaderManager::LoadJointsNextMatricesLoc() {
//	jointsNextMatricesLoc = glGetUniformLocation(shaderProgram->GetProgram(), "nextJoints");
//	if (jointsNextMatricesLoc < 0) {
//		WriteErrorToFile("Next joints matrices location not found!");
//		throw std::exception();
//	}
//}

void ObjectShaderManager::LoadInterpolationLoc() {
	interpolationLoc = glGetUniformLocation(shaderProgram->GetProgram(), "interpolation");
	if (interpolationLoc < 0) {
		WriteErrorToFile("Interpolation value location not found!");
		throw std::exception();
	}
}

//void ObjectShaderManager::LoadHitboxJointsPreviousMatricesLoc() {
//	jointsHitboxPrevMatricesLoc = glGetUniformLocation(hitboxShaderProgram->GetProgram(), "prevJoints");
//	if (jointsHitboxPrevMatricesLoc < 0) {
//		WriteErrorToFile("Hitbox previous joints matrices location not found!");
//		throw std::exception();
//	}
//}
//
//void ObjectShaderManager::LoadHitboxJointsNextMatricesLoc() {
//	jointsHitboxNextMatricesLoc = glGetUniformLocation(hitboxShaderProgram->GetProgram(), "nextJoints");
//	if (jointsHitboxNextMatricesLoc < 0) {
//		WriteErrorToFile("Hitbox next joints matrices location not found!");
//		throw std::exception();
//	}
//}

void ObjectShaderManager::LoadHitboxInterpolationLoc() {
	hitboxInterpolationLoc = glGetUniformLocation(hitboxShaderProgram->GetProgram(), "interpolation");
	if (hitboxInterpolationLoc < 0) {
		WriteErrorToFile("Hitbox interpolation value location not found!");
		throw std::exception();
	}
}

//void ObjectShaderManager::LoadHitboxComputeJointsPreviousMatricesLoc() {
//	jointsHitboxComputePrevMatricesLoc = glGetUniformLocation(hitboxComputeShaderProgram->GetProgram(), "prevJoints");
//	if (jointsHitboxComputePrevMatricesLoc < 0) {
//		WriteErrorToFile("Hitbox compute previous joints matrices location not found!");
//		throw std::exception();
//	}
//}
//
//void ObjectShaderManager::LoadHitboxComputeJointsNextMatricesLoc() {
//	jointsHitboxComputeNextMatricesLoc = glGetUniformLocation(hitboxComputeShaderProgram->GetProgram(), "nextJoints");
//	if (jointsHitboxComputeNextMatricesLoc < 0) {
//		WriteErrorToFile("Hitbox compute next joints matrices location not found!");
//		throw std::exception();
//	}
//}

void ObjectShaderManager::LoadHitboxComputeInterpolationLoc() {
	hitboxComputeInterpoaltionLoc = glGetUniformLocation(hitboxComputeShaderProgram->GetProgram(), "interpolation");
	if (interpolationLoc < 0) {
		WriteErrorToFile("Hitbox compute interpolation value location not found!");
		throw std::exception();
	}
}

void ObjectShaderManager::WriteErrorToFile(const char* message) {
	std::fstream file("LOGS/ObjectShaderManager_ERRORLOG.txt", std::ios::out | std::ios::app);
	assert(file.is_open());
	SYSTEMTIME time;
	GetSystemTime(&time);
	file << time.wDay << "." << time.wMonth << "." << time.wYear << " " << time.wHour << ":" << time.wMinute << ":" << time.wSecond << " - ";
	file << message << std::endl;
	file.close();
}

//-----------------------------------------------------------------------------
//-------------------------DynamicObjectShaderManager--------------------------
//-----------------------------------------------------------------------------

void DynamicObjectShaderManager::SetVAO(float* verticesBuffer, int verticesCount){
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, verticesCount * sizeof(float), verticesBuffer, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, objectBufferVertexAttribCount * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, objectBufferVertexAttribCount * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	for (unsigned i = 0; i < 4; ++i) {
		glVertexAttribPointer(i + 2, 2, GL_FLOAT, GL_FALSE, objectBufferVertexAttribCount * sizeof(float), (void*)((2 * i + 5) * sizeof(float)));
		glEnableVertexAttribArray(i + 2);
	}
}

void DynamicObjectShaderManager::SetHitboxVAO(float* verticesBuffer, unsigned* indicesBuffer, int verticesCount) {
	glBindVertexArray(hitboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, hitboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*verticesCount, verticesBuffer, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hitboxIndicesBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) * 24, indicesBuffer, GL_DYNAMIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, hitboxObjectBufferVertexAttribCount * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, hitboxObjectBufferVertexAttribCount * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
}

void DynamicObjectShaderManager::Init() {
	GenerateBuffersAndArrays();

	LoadModelMatrixLoc();
	LoadViewMatrixLoc();
	LoadProjectionMatrixLoc();
	LoadTextureAvailableLoc();

	LoadHitboxColor1Loc();
	LoadHitboxColor2Loc();
	LoadHitboxSetCurrJointIdxLoc();

	LoadHitboxModelMatrixLoc();
	LoadHitboxViewMatrixLoc();
	LoadHitboxProjectionMatrixLoc();

	LoadHitboxComputeModelMatrixLoc();

	LoadInterpolationLoc();

	LoadHitboxInterpolationLoc();

	LoadHitboxComputeInterpolationLoc();
}

//-----------------------------------------------------------------------------
//---------------------DynamicObjectShaderManager::Private---------------------
//-----------------------------------------------------------------------------

void DynamicObjectShaderManager::GenerateBuffersAndArrays() {
	ObjectShaderManager::GenerateBuffersAndArrays();

	glGenBuffers(1, &jointsPrevMatricesBuffer);
	glGenBuffers(1, &jointsNextMatricesBuffer);
}

void DynamicObjectShaderManager::SetJointsBuffers(int jointsCount) {
	glUseProgram(shaderProgram->GetProgram());

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, jointsPrevMatricesBuffer);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, jointsCount * 16 * sizeof(float), NULL, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, jointsNextMatricesBuffer);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, jointsCount * 16 * sizeof(float), NULL, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, jointsPrevMatricesBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, jointsNextMatricesBuffer);

	GLsync sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
	GLint result;
	do { glGetSynciv(sync, GL_SYNC_STATUS, sizeof(GLint), NULL, &result); } while (result != GL_SIGNALED);

	jointsPrevMatBufferPtr = glMapNamedBufferRange(jointsPrevMatricesBuffer,0,jointsCount*16*sizeof(float),GL_MAP_WRITE_BIT|GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
	jointsNextMatBufferPtr = glMapNamedBufferRange(jointsNextMatricesBuffer, 0,jointsCount*16*sizeof(float),GL_MAP_WRITE_BIT|GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
}

//-----------------------------------------------------------------------------
//--------------------------StaticObjectShaderManager--------------------------
//-----------------------------------------------------------------------------

void StaticObjectShaderManager::SetVAO(float* verticesBuffer, int verticesCount) {
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, verticesCount * sizeof(float), verticesBuffer, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, objectBufferVertexAttribCount * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, objectBufferVertexAttribCount * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

void StaticObjectShaderManager::SetHitboxVAO(float* verticesBuffer, unsigned* indicesBuffer, int verticesCount) {

	glBindVertexArray(hitboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, hitboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*verticesCount, verticesBuffer, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hitboxIndicesBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) * 24, indicesBuffer, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,  hitboxObjectBufferVertexAttribCount * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, hitboxObjectBufferVertexAttribCount * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
}

void StaticObjectShaderManager::Init() {
	GenerateBuffersAndArrays();

	LoadModelMatrixLoc();
	LoadViewMatrixLoc();
	LoadProjectionMatrixLoc();
	LoadTextureAvailableLoc();

	LoadHitboxModelMatrixLoc();
	LoadHitboxViewMatrixLoc();
	LoadHitboxProjectionMatrixLoc();
	LoadHitboxColor1Loc();
	LoadHitboxColor2Loc();
	LoadHitboxSetCurrJointIdxLoc();

	LoadHitboxComputeModelMatrixLoc();
}
