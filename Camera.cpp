#include"Camera.h"

#include"matrix.cpp"
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
  view = glm::lookAt(Position, Position + Orientation, Up);
  proj = projection(FOV, (float)width / height, near, far);
}

float Camera::HandleInputs(GLFWwindow *window) {
  if (!ImGui::IsWindowFocused(ImGuiHoveredFlags_AnyWindow) &&
      !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow))
  {
    KeyboardInputs(window);
    MouseInputs(window);
  }

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
    // Hides mouse cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    // Prevents camera from jumping on the first click
    if (firstClick) {
      glfwSetCursorPos(window, (width / 2), (height / 2));
      firstClick = false;
    }

    // Stores the coordinates of the cursor
    double mouseX;
    double mouseY;
    // Fetches the coordinates of the cursor
    glfwGetCursorPos(window, &mouseX, &mouseY);

    // Normalizes and shifts the coordinates of the cursor such that
    // they begin in the middle of the screen and then "transforms" them
    // into degrees
    float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
    float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;

    // Calculates upcoming vertical change in the Orientation
    glm::vec3 newOrientation =
        glm::rotate(Orientation, glm::radians(-rotX),
                    glm::normalize(glm::cross(Orientation, Up)));

    // Decides whether or not the next vertical Orientation is legal or
    // not
    if (abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <=
        glm::radians(85.0f)) {
      Orientation = newOrientation;
    }

    // Rotates the Orientation left and right
    Orientation = glm::rotate(Orientation, glm::radians(-rotY), Up);

    // Sets mouse cursor to the middle of the screen so that it doesn't
    // end up roaming around
    glfwSetCursorPos(window, (width / 2), (height / 2));
  } else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) ==
             GLFW_RELEASE) {
    // Unhides cursor since camera is not looking around anymore
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    // Makes sure the next time the camera looks around it doesn't jump
    firstClick = true;
  }
}