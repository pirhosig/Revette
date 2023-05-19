#include "ShaderProgram.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

#include "../GlobalLog.h"



// Utility function to read shader code files
std::string readFile(const char* filePath)
{
	std::string fileString;
	std::ifstream file;
	std::stringstream fileStream;

	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	file.open(filePath);
	if (!file.is_open()) throw std::runtime_error("Error reading shader file");
	fileStream << file.rdbuf();
	fileString = fileStream.str();

	return fileString;
}



GLuint createShader(const char* shaderCode, const int shaderType)
{
	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderCode, NULL);
	glCompileShader(shader);

	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char infoLog[1024]{};
		glGetShaderInfoLog(shader, 1024, NULL, infoLog);
		GlobalLog.Write(std::string(infoLog, infoLog + 1024));
		throw std::runtime_error("Shader creation error");
	}
	return shader;
}



ShaderProgram::ShaderProgram(const char* shaderVertexPath, const char* shaderFragmentPath)
{
	programID = NULL;

	std::string codeVertex = readFile(shaderVertexPath);
	std::string codeFragment = readFile(shaderFragmentPath);

	GLuint shaderVertex = createShader(codeVertex.c_str(), GL_VERTEX_SHADER);
	GLuint shaderFragment = createShader(codeFragment.c_str(), GL_FRAGMENT_SHADER);

	programID = glCreateProgram();
	glAttachShader(programID, shaderVertex);
	glAttachShader(programID, shaderFragment);
	glLinkProgram(programID);

	GLint success;
	glGetProgramiv(programID, GL_LINK_STATUS, &success);
	if (!success)
	{
		char infoLog[1024]{};
		glGetProgramInfoLog(programID, 1024, NULL, infoLog);
		GlobalLog.Write(std::string(infoLog, infoLog + 1024));
	}

	glDeleteShader(shaderVertex);
	glDeleteShader(shaderFragment);
}



ShaderProgram::~ShaderProgram()
{
	if (programID != NULL) glDeleteProgram(programID);
}



// The functions set a uniform in a shader
void ShaderProgram::setInt(const char* name, const GLint value) const
{
	glUniform1i(getUniformLocation(name), value);
}

void ShaderProgram::setMat4(const char* name, const glm::mat4& mat) const
{
	glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void ShaderProgram::setVec2(const char* name, const glm::vec2& vec) const
{
	glUniform2f(getUniformLocation(name), vec.x, vec.y);
}



// Sets the active shader to this one
void ShaderProgram::use() const
{
	glUseProgram(programID);
}



// Gets the location of a uniform in the shader program
GLint ShaderProgram::getUniformLocation(const char* uniformName) const
{
	return glGetUniformLocation(programID, uniformName);
}