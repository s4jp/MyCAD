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
#include"cursor.h"
#include"Camera.h"
#include"point.h"

using namespace std;

const int width = 800;
const int height = 600;
const glm::vec3 cameraPosition = glm::vec3(3.0f, 3.0f, 3.0f);
const float fov = M_PI / 4.0f;
const float near = 0.1f;
const float far = 100.0f;

vector<Figure*> figures;
Grid* grid;
Cursor *cursor;
Camera camera(width, height, cameraPosition, fov, near, far);

float cursorRadius = glm::max(glm::abs(cameraPosition.x), glm::abs(cameraPosition.z));

static int currentMenuItem = 0;
const char *menuItems = "Move camera\0Place cursor\0Add element";

glm::mat4 view;
glm::mat4 proj;

void window_size_callback(GLFWwindow *window, int width, int height);
void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods);
void cursorHandleInput(GLFWwindow *window);
tuple<glm::vec3, glm::vec3> calculateNearFarProjections(double xMouse,
                                                        double yMouse);
vector<glm::vec3> circleIntersections(float radius, glm::vec3 center,
                                      glm::vec3 dir);

int main() { 

    #pragma region gl_boilerplate
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

    gladLoadGL();
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    #pragma endregion

    // shader
    Shader shaderProgram("default.vert", "default.frag");

    // callbacks
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetKeyCallback(window, key_callback);

    // init figures
    figures.push_back(new Torus());
    figures[0]->selected = true;
    grid = new Grid(30.f, 50);
    cursor = new Cursor(0.2f);

    // matrices locations
    camera.PrepareMatrices(view, proj);
    int modelLoc = glGetUniformLocation(shaderProgram.ID, "model");
    int viewLoc = glGetUniformLocation(shaderProgram.ID, "view");
    int projLoc = glGetUniformLocation(shaderProgram.ID, "proj");
    int colorLoc = glGetUniformLocation(shaderProgram.ID, "color");

    #pragma region imgui_boilerplate
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    #pragma endregion

    while (!glfwWindowShouldClose(window)) 
    {
        #pragma region init
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

		shaderProgram.Activate();
        #pragma endregion

        switch (currentMenuItem) { 
        case 0:
          cursorRadius = camera.HandleInputs(window);
          camera.PrepareMatrices(view, proj);
          break;
        case 1:
          cursorHandleInput(window);
          break;
        }

        // matrices
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

        // objects rendering
        grid->Render(colorLoc, modelLoc);
        std::for_each(figures.begin(), figures.end(),
                      [colorLoc, modelLoc](Figure *f) {
                        f->Render(colorLoc, modelLoc);
                      });
        cursor->Render(colorLoc, modelLoc);

        // imgui rendering
        if (ImGui::Begin("Mode")) {
          ImGui::Combo(" ", &currentMenuItem, menuItems);

          if (currentMenuItem == 2) {
            ImGui::Separator();
            if (ImGui::Button("Torus")) {
              figures.push_back(new Torus());
              figures[figures.size() - 1]->SetTranslation(
                  cursor->GetTranslation());
            }
            ImGui::SameLine();
            if (ImGui::Button("Point")) {
              figures.push_back(new Point());
              figures[figures.size() - 1]->SetTranslation(
                  cursor->GetTranslation());
            }
          }
          
          ImGui::Separator();
          for (int i = 0; i < figures.size(); i++) 
          {
              if (ImGui::Selectable(figures[i]->name.c_str(),
                  &figures[i]->selected))
              {
                  if (!ImGui::GetIO().KeyShift) 
                  {
                    bool temp = figures[i]->selected;
                    std::for_each(figures.begin(), figures.end(), 
                          [](Figure *f) { f->selected = false;;});
                    figures[i]->selected = temp;
                  }
              }
          }

          ImGui::Separator();
          if (ImGui::Button("Delete selected")) {
            for (int i = 0; i < figures.size(); i++) {
              if (figures[i]->selected) {
                figures.erase(figures.begin() + i);
                i--;
              }
            }
          }
        }
        ImGui::End();

        std::for_each(figures.begin(), figures.end(), [](Figure *f) {
          if (f->selected)
            f->CreateImgui();
        });

        #pragma region rest
        ImGui::Render();
        // cout << ImGui::GetIO().Framerate << endl;
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
        #pragma endregion
    }
    #pragma region exit
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

	std::for_each(figures.begin(), figures.end(),
                  [](Figure* f) { f->Delete(); });
    shaderProgram.Delete();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
    #pragma endregion
}

void window_size_callback(GLFWwindow *window, int width, int height) {
  camera.width = width;
  camera.height = height;
  glViewport(0, 0, width, height);
}

void cursorHandleInput(GLFWwindow *window) 
{
  if (!ImGui::IsWindowFocused(ImGuiHoveredFlags_AnyWindow) &&
      !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)) 
  {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) 
    {
      double mouseX;
      double mouseY;
      glfwGetCursorPos(window, &mouseX, &mouseY);

      tuple<glm::vec3, glm::vec3> projections = calculateNearFarProjections(mouseX, mouseY);
      vector<glm::vec3> points = circleIntersections(
          cursorRadius, get<0>(projections),
          glm::normalize(get<1>(projections) - get<0>(projections)));
      // take positive solution
      cursor->SetTranslation(points[0]);
    }
  }
}

tuple<glm::vec3, glm::vec3> calculateNearFarProjections(double xMouse,
                                                        double yMouse) {
  glm::mat4 invMat = glm::inverse(proj * view);

  float xMouseClip = (xMouse - camera.width / 2) / (camera.width / 2);
  float yMouseClip = -1 * (yMouse - camera.height / 2) / (camera.height / 2);

  glm::vec4 near = glm::vec4(xMouseClip, yMouseClip, -1.0f, 1.0f);
  glm::vec4 far = glm::vec4(xMouseClip, yMouseClip, 1.0f, 1.0f);
  glm::vec4 nearResult = invMat * near;
  glm::vec4 farResult = invMat * far;
  nearResult /= nearResult.w;
  farResult /= farResult.w;
  return tuple<glm::vec3, glm::vec3>(glm::vec3(nearResult),
                                     glm::vec3(farResult));
}

vector<glm::vec3> circleIntersections(float radius, glm::vec3 center, glm::vec3 dir) 
{
  vector<glm::vec3> result;
  float k = glm::sqrt(radius * radius / glm::dot(dir, dir));
  result.push_back(center + k * dir);
  if (k > 0)
    result.push_back(center - k * dir);
  return result;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods) {
  if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
    currentMenuItem = 0;
  } else if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
    currentMenuItem = 1;
  } else if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
    currentMenuItem = 2;
  }
}
