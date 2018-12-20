#pragma once
#include <vector>
#include <map>
#include "Shader.h"

class ShaderManager {
	ShaderManager() {}
	ShaderManager(const ShaderManager&) {}
	ShaderManager& operator=(const ShaderManager&) {}

	typedef std::map<std::string, Shader*> ShaderMap;
	static ShaderMap shaders;

public:

	static Shader* RegisterShader(Shader* shader_);

	//static Shader* RegisterShader(const char* vertexShaderFilename, const char* fragmentShaderFilename){
	//	shaders.push_back(new Shader(vertexShaderFilename, fragmentShaderFilename));
	//	return shaders[shaders.size() - 1];
	//}

	//static Shader* RegisterShader(const char* computeShaderFilename) {
	//	shaders.push_back(new Shader(computeShaderFilename));
	//	return shaders[shaders.size() - 1];
	//}

	static unsigned GetShadersCount() { return shaders.size(); }
	//static Shader* GetShader(unsigned index) { if (index < shaders.size()) return shaders[index]; }
	static Shader* GetShader(std::string shaderName);

};