#include "Camera.h"
#include "helpers.h"

#include"imgui.h"

Camera::Camera(int width, int height, glm::vec3 position, float FOV, float near,
               float far, int guiWidth) {
	Camera::width = width;
	Camera::height = height;
	Camera::Position = position;
    Camera::Orientation = glm::normalize(- Camera::Position);
    Camera::FOV = FOV;
    Camera::near = near;
    Camera::far = far;
    Camera::guiWidth = guiWidth;
}

void Camera::PrepareMatrices(glm::mat4 &view, glm::mat4 &proj) {
  view = CAD::lookAt(Position, Position + Orientation, Up);
  proj = CAD::projection(FOV, (float)(width - guiWidth) / height, near, far);
}

void Camera::HandleInputs(GLFWwindow *window) {
  if (!ImGui::IsWindowFocused(ImGuiHoveredFlags_AnyWindow) &&
      !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow))
  {
    KeyboardInputs(window);
    MouseInputs(window);
  }
}

void Camera::KeyboardInputs(GLFWwindow *window) 
{
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    Position += speed * Orientation;
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    Position += speed * -glm::normalize(glm::cross(Orientation, Up));
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    Position += speed * -Orientation;
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    Position += speed * glm::normalize(glm::cross(Orientation, Up));
  }
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    Position += speed * Up;
  }
  if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
    Position += speed * -Up;
  }
}

void Camera::MouseInputs(GLFWwindow *window) 
{
  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) 
  {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    if (firstClick) {
      glfwSetCursorPos(window, (double)(width / 2), (double)(height / 2));
      firstClick = false;
    }

    double mouseX;
    double mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    float rotX = sensitivity * (float)(mouseY - (double)(height / 2)) / height;
    float rotY = sensitivity * (float)(mouseX - (double)(width / 2)) / width;

    glm::mat4 rot = CAD::rotate(
        glm::mat4(1.f),
        glm::radians(-rotX) * glm::normalize(glm::cross(Orientation, Up)));
    glm::vec3 newOrientation = glm::mat3(rot) * Orientation;
    if (abs(CAD::angleBetweenVectors(newOrientation, Up) -
            glm::radians(90.0f)) <=
        glm::radians(85.0f)) {
      Orientation = newOrientation;
    }
    glm::mat4 rot2 = CAD::rotate(glm::mat4(1.f), glm::radians(-rotY) * Up);
    Orientation = glm::mat3(rot2) * Orientation;
    glfwSetCursorPos(window, (double)(width / 2), (double)(height / 2));
  } 
  else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) ==
             GLFW_RELEASE) {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    firstClick = true;
  }
}