#include "ShaderManager.h"

std::map<std::string, Shader*> ShaderManager::shaders;

Shader* ShaderManager::RegisterShader(Shader* shader_) {
	if (shader_ != nullptr) {
		std::pair<ShaderMap::iterator, bool> insertVal;
		insertVal = shaders.insert(ShaderMap::value_type(shader_->GetName(), shader_));
		if (insertVal.second == false) return nullptr;
		else return shader_; 
	}
	else return nullptr;
}

Shader* ShaderManager::GetShader(std::string shaderName) {
	ShaderMap::iterator it = shaders.find(shaderName);
	if (it != shaders.end()) return (*it).second;
	else return nullptr;
}
