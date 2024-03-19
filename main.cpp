#define _USE_MATH_DEFINES
#include"imgui.h"
#include"imgui_impl_glfw.h"
#include"imgui_impl_opengl3.h"

#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<cmath>
#include<vector>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include <algorithm>

#include"shaderClass.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"
#include"figure.h"
#include"torus.h"
#include"grid.h"

using namespace std;

int width = 800;
int height = 600;
const glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, -3.0f);
const float fov = M_PI / 4.0f;
const float near = 0.1f;
const float far = 100.0f;

int rotating = 1;
bool firstClick = true;
bool change1 = false;
bool change2 = false;
bool change3 = false;

glm::vec3 scale = glm::vec3(1.0f);
glm::vec3 angle = glm::vec3(0.0f);
glm::vec3 translation = glm::vec3(0.0f);

const float scrollSensitivity = 20.0f;

vector<Figure*> figures;
Grid* grid;

glm::mat4 translate(glm::mat4 matrix, glm::vec3 vector);
glm::mat4 createXrotationMatrix(float angle);
glm::mat4 createYrotationMatrix(float angle);
glm::mat4 createZrotationMatrix(float angle);
glm::mat4 rotate(glm::mat4 matrix, glm::vec3 angle);
glm::mat4 projection(float fov, float ratio, float near, float far);
glm::mat4 scaling(glm::mat4 matrix, glm::vec3 scale);
void HandleInputs(GLFWwindow *window);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void window_size_callback(GLFWwindow *window, int width, int height);

int main() { 
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *window = glfwCreateWindow(width, height, "MKMG", NULL, NULL);
    if (window == NULL) {
      std::cout << "Failed to create GLFW window" << std::endl;
      glfwTerminate();
      return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSetScrollCallback(window, scroll_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);

    gladLoadGL();
    glViewport(0, 0, width, height);

    Shader shaderProgram("default.vert", "default.frag");

    figures.push_back(new Torus(0.5f, 0.2f, 10, 20));
    grid = new Grid(10.f, 50);

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = translate(glm::mat4(1.0f), cameraPosition);
    glm::mat4 proj = projection(fov, (float)width / height, near, far);
    int modelLoc = glGetUniformLocation(shaderProgram.ID, "model");
    int viewLoc = glGetUniformLocation(shaderProgram.ID, "view");
    int projLoc = glGetUniformLocation(shaderProgram.ID, "proj");
    int colorLoc = glGetUniformLocation(shaderProgram.ID, "color");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    while (!glfwWindowShouldClose(window)) 
    {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

		shaderProgram.Activate();

        HandleInputs(window);

        if (change1) {
          dynamic_cast<Torus*>(figures[0])->Recalculate();
          change1 = false;
        }
        if (change2)
        {
          model = translate(rotate(scaling(glm::mat4(1.0f), scale), angle),
                            translation);
          change2 = false;
        }
        if (change3)
        {
          glfwGetWindowSize(window, &width, &height);
          proj = projection(fov, (float)width / height, near, far);
          glViewport(0, 0, width, height);
          change3 = false;
        }

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

        grid->Render(colorLoc);
        std::for_each(figures.begin(), figures.end(),
                      [colorLoc](Figure *f) { f->Render(colorLoc); });

        if (ImGui::Begin("Options"))
        {
          ImGui::SeparatorText("Torus parameters");

          Torus *tor = dynamic_cast<Torus *>(figures[0]);

          if (ImGui::SliderFloat("R1", &(tor->R1), 0.01f, 5.f, "%.2f"))
            change1 = true;
          if (ImGui::SliderFloat("R2", &(tor->R2), 0.01f, 5.f, "%.2f"))
            change1 = true;
          if (ImGui::SliderInt("major", &(tor->n2), 3, 50))
            change1 = true;
          if (ImGui::SliderInt("minor", &(tor->n1), 3, 50))
            change1 = true;

          ImGui::SeparatorText("Transform mode");

          ImGui::RadioButton("rotate", &rotating, 1);
          ImGui::RadioButton("move", &rotating, 0);

          ImGui::SeparatorText("Scaling");

          if (ImGui::SliderFloat("Sx", &scale[0], 0.01f, 5.f, "%.2f"))
            change2 = true;
          if (ImGui::SliderFloat("Sy", &scale[1], 0.01f, 5.f, "%.2f"))
            change2 = true;
          if (ImGui::SliderFloat("Sz", &scale[2], 0.01f, 5.f, "%.2f"))
            change2 = true;
          if (ImGui::Button("Reset"))
          {
            scale = glm::vec3(1.0f);
            change2 = true;
          }
        }
        ImGui::End();

        ImGui::Render();
        // cout << ImGui::GetIO().Framerate << endl;
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

	std::for_each(figures.begin(), figures.end(),
                  [](Figure* f) { f->Delete(); });
    shaderProgram.Delete();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void HandleInputs(GLFWwindow* window) 
{
  if (!ImGui::IsWindowFocused(ImGuiHoveredFlags_AnyWindow) &&
      !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)) 
    {
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
          glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

          if (firstClick) {
            glfwSetCursorPos(window, (width / 2), (height / 2));
            firstClick = false;
          }

          double mouseX;
          double mouseY;
          glfwGetCursorPos(window, &mouseX, &mouseY);

          float rotX = 1.0f * (float)(mouseY - (height / 2)) / height;
          float rotY = 1.0f * (float)(mouseX - (width / 2)) / width;

          if (rotating == 1) 
          {
            angle[0] -= rotY;
            angle[1] += rotX;
          }
          else
          {
            translation[0] -= rotY;
            translation[1] += rotX;
          }
          change2 = true;

          glfwSetCursorPos(window, (width / 2), (height / 2));
        } 
        else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) ==
                   GLFW_RELEASE) 
        {
          glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
          firstClick = true;
        }
    }
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) 
{
    float yDiff = yoffset / 30.0f;

    if (rotating == 1) 
    {
      angle[2] += yDiff;
    } 
    else 
    {
      translation[2] += yDiff;
    }
    change2 = true;
}

void window_size_callback(GLFWwindow* window, int width, int height) 
{
  change3 = true;
}