#include"Camera.h"

#include"helpers.h"
#include"imgui.h"
#include"imgui_impl_glfw.h"
#include"imgui_impl_opengl3.h"

Camera::Camera(int width, int height, glm::vec3 position, float FOV, float near,
               float far) {
	Camera::width = width;
	Camera::height = height;
	Camera::Position = position;
    Camera::Orientation = glm::normalize(- Camera::Position);
    Camera::FOV = FOV;
    Camera::near = near;
    Camera::far = far;
}

void Camera::PrepareMatrices(glm::mat4 &view, glm::mat4 &proj) {
  view = CAD::lookAt(Position, Position + Orientation, Up);
  proj = CAD::projection(FOV, (float)width / height, near, far);
}

void Camera::HandleInputs(GLFWwindow *window) {
  if (!ImGui::IsWindowFocused(ImGuiHoveredFlags_AnyWindow) &&
      !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow))
  {
    KeyboardInputs(window);
    MouseInputs(window);
  }
}

float Camera::GetCursorRadius() {
  return glm::max(glm::abs(Position.x), glm::abs(Position.z));
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
      glfwSetCursorPos(window, (width / 2), (height / 2));
      firstClick = false;
    }

    double mouseX;
    double mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
    float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;

    glm::vec3 newOrientation =
        glm::rotate(Orientation, glm::radians(-rotX),
                    glm::normalize(glm::cross(Orientation, Up)));
    if (abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <=
        glm::radians(85.0f)) {
      Orientation = newOrientation;
    }
    Orientation = glm::rotate(Orientation, glm::radians(-rotY), Up);
    glfwSetCursorPos(window, (width / 2), (height / 2));
  } 
  else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) ==
             GLFW_RELEASE) {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    firstClick = true;
  }
}