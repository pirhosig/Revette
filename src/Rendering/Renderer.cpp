#include "Renderer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>



Renderer::Renderer(GLFWwindow* window) : testShader("shader/testShader.vs", "shader/testShader.fs")
{
	mainWindow = window;
	VAO = NULL;
	VBO = NULL;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8, (void*)(0));
	glEnableVertexAttribArray(0);

	const float TEST_VERTICIES[] = {
		 0.0f,  0.0f,
		 0.5f,  1.5f,
		 0.0f,  1.0f
	};

	glBufferData(GL_ARRAY_BUFFER, 24, &TEST_VERTICIES[0], GL_STATIC_DRAW);
}



void Renderer::render(const EntityPosition& playerPos)
{
	glClearColor(0.53f, 0.52f, 0.83f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	const glm::mat4 projection = glm::perspective(glm::radians(45.0), 1920.0 / 1080.0, 0.1, 100.0);
	const glm::vec3 pos(playerPos.X, playerPos.Y, playerPos.Z);
	const glm::vec front = glm::normalize(glm::vec3(
		cos(glm::radians(playerPos.xRotation)) * cos(glm::radians(playerPos.yRotation)),
		sin(glm::radians(playerPos.yRotation)),
		sin(glm::radians(playerPos.xRotation)) * cos(glm::radians(playerPos.yRotation))
	));
	const glm::mat4 view = glm::lookAt(pos, pos + front, glm::vec3(0.0, 1.0, 0.0));
	const glm::mat4 projectionView = projection * view;

	testShader.use();
	testShader.setMat4("transform", projectionView);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glfwSwapBuffers(mainWindow);
}