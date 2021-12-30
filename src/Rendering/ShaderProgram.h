#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>



class ShaderProgram
{
public:
	~ShaderProgram();

	void loadProgram(const char* shaderVertexPath, const char* shaderFragmentPath);
	void setInt(const char* name, const GLint value) const;
	void setMat4(const char* name, const glm::mat4& mat) const;
	void setVec2(const char* name, const glm::vec2& vec) const;
	void use() const;

private:
	GLint getUniformLocation(const char* uniformName) const;

	GLuint programID = NULL;
};