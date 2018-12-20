#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <gl\glew.h>
#include <Windows.h>
#include <string>
#include <fstream>
#include <assert.h>

class Shader {
	enum TYPE{NOTYPE,GEOMETRY,COMPUTE};

	TYPE type;

	int vertexShader;
	int fragmentShader;
	int computeShader;

	int shaderProgram;

	std::string shaderProgramName;

	std::string LoadShaderFromFile(std::string filename);
	void LoadErrorToFile(const char* message);

public:
	//Shader() :vertexShader(-1), fragmentShader(-1), computeShader(-1), shaderProgram(-1), shaderProgramName("Standard shader"), type(TYPE::NOTYPE) {}
	Shader( std::string name, std::string computeShaderFilename);
	Shader(std::string name, std::string vertexShaderFilename, std::string fragmentShaderFilename);
	//Shader(std::string vertexShaderFilename_, std::string fragmentShaderFilename_);
	//Shader(std::string computeShaderFilename);

	int LoadVertexShader(std::string vertexShaderFilename_);
	int LoadFragmentShader(std::string fragmentShaderFilename_);
	int LoadComputeShader(std::string computeShaderFilename_);

	int LinkShaderProgram();

	unsigned GetProgram()const { return shaderProgram; }

	void SetName(std::string programName_) { shaderProgramName = programName_; }
	std::string GetName()const { return shaderProgramName; }
};