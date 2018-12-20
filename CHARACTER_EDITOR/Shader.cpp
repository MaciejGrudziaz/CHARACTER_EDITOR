#include "Shader.h"

//Shader::Shader(std::string vertexShaderFilename_, std::string fragmentShaderFilename_):shaderProgramName("Standard shader"),shaderProgram(-1),vertexShader(-1),fragmentShader(-1),computeShader(-1),type(GEOMETRY){
//	int error;
//	error = LoadVertexShader(vertexShaderFilename_);
//	assert(error);
//	error = LoadFragmentShader(fragmentShaderFilename_);
//	assert(error);
//	error = LinkShaderProgram();
//	assert(error);
//}
//
//Shader::Shader(std::string computeShaderFilename_) :shaderProgramName("Standard shader"), shaderProgram(-1),vertexShader(-1), fragmentShader(-1), computeShader(-1), type(TYPE::COMPUTE) {
//	int error;
//	error = LoadComputeShader(computeShaderFilename_);
//	assert(error);
//	error = LinkShaderProgram();
//	assert(error);
//}

Shader::Shader(std::string name, std::string computeShaderFilename):type(COMPUTE),shaderProgramName(name) {
	int error;
	error = LoadComputeShader(computeShaderFilename);
	assert(error);
	error = LinkShaderProgram();
	assert(error);
}

Shader::Shader(std::string name, std::string vertexShaderFilename, std::string fragmentShaderFilename):type(GEOMETRY),shaderProgramName(name) {
	int error;
	error = LoadVertexShader(vertexShaderFilename);
	assert(error);
	error = LoadFragmentShader(fragmentShaderFilename);
	assert(error);
	error = LinkShaderProgram();
	assert(error);
}

int Shader::LoadVertexShader(std::string vertexShaderFilename_) {
	if (type != TYPE::COMPUTE) {
		std::string shaderSource;
		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		shaderSource = LoadShaderFromFile(vertexShaderFilename_);
		const GLchar* shaderSourceChar = shaderSource.c_str();
		glShaderSource(vertexShader, 1, &(shaderSourceChar), NULL);
		glCompileShader(vertexShader);

		int success;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			char infoLog[512];
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			LoadErrorToFile("ERROR::SHADER::VERTEX::COMPILATION_FAILED");
			LoadErrorToFile(infoLog);
		}

		return success;
	}
	else return 0;
}

int Shader::LoadFragmentShader(std::string fragmentShaderFilename_) {
	if (type != TYPE::COMPUTE) {
		std::string shaderSource;
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		shaderSource = LoadShaderFromFile(fragmentShaderFilename_);
		const GLchar* shaderSourceChar = shaderSource.c_str();
		glShaderSource(fragmentShader, 1, &(shaderSourceChar), NULL);
		glCompileShader(fragmentShader);

		int success;
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			char infoLog[512];
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			LoadErrorToFile("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED");
			LoadErrorToFile(infoLog);
		}

		return success;
	}
	else return 0;
}

int Shader::LoadComputeShader(std::string computeShaderFilename_) {
	if (type != TYPE::GEOMETRY) {
		std::string shaderSource;
		computeShader = glCreateShader(GL_COMPUTE_SHADER);
		shaderSource = LoadShaderFromFile(computeShaderFilename_);
		const GLchar* shaderSourceChar = shaderSource.c_str();
		glShaderSource(computeShader, 1, &(shaderSourceChar), NULL);
		glCompileShader(computeShader);

		int success;
		glGetShaderiv(computeShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			char infoLog[512];
			glGetShaderInfoLog(computeShader, 512, NULL, infoLog);
			LoadErrorToFile("ERROR::SHADER::COMPUTE::COMPILATION_FAILED");
			LoadErrorToFile(infoLog);
		}

		return success;
	}
	else return 0;
}

int Shader::LinkShaderProgram() {
	shaderProgram = glCreateProgram();
	if (type == TYPE::GEOMETRY) {
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
	}
	else if (type == TYPE::COMPUTE)
		glAttachShader(shaderProgram, computeShader);

	glLinkProgram(shaderProgram);

	int success;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		LoadErrorToFile("ERROR::PROGRAM::LINKING_FAILED");
		LoadErrorToFile(infoLog);
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteShader(computeShader);

	return success;
}

std::string Shader::LoadShaderFromFile(std::string filename) {
	std::fstream file(filename, std::ios::in);
	if (!file.is_open()) {
		LoadErrorToFile("Shader file not found!");
		LoadErrorToFile(filename.c_str());
		assert(file.is_open());
	}

	std::string fileStr;
	char sign;
	do {
		sign = file.get();
		if (!file.eof())
			fileStr.push_back(sign);
	} while (!file.eof());
	file.close();
	return fileStr;
}

void Shader::LoadErrorToFile(const char* message) {
	std::fstream file("LOGS/"+shaderProgramName + "_SHADER_ERRORLOG.txt", std::ios::out | std::ios::app);
	assert(file.is_open());
	SYSTEMTIME time;
	GetSystemTime(&time);
	file << time.wDay << "." << time.wMonth << "." << time.wYear << " " << time.wHour << ":" << time.wMinute << ":" << time.wSecond << " - ";
	file << message <<std::endl;
	file.close();
}