#pragma once

#include<GLFW/glfw3.h>
#include<glm/glm.hpp>

class Camera
{
public:
	glm::vec3 Position;
	glm::vec3 Orientation;
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

	bool firstClick = true;

	int width;
	int height;

	float speed = 0.1f;
	float sensitivity = 100.0f;

	float FOV;
    float near;
    float far;

	Camera(int width, int height, glm::vec3 position, float FOV, float near,
           float far);

	void PrepareMatrices(glm::mat4 &view, glm::mat4 &proj);
	void HandleInputs(GLFWwindow* window);
    float GetCursorRadius();

private:
    void KeyboardInputs(GLFWwindow *window);
	void MouseInputs(GLFWwindow *window);
};