#define _USE_MATH_DEFINES
#include"imgui.h"
#include"imgui_impl_glfw.h"
#include"imgui_impl_opengl3.h"
#include"imgui_stdlib.h"

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
Cursor *center;

float cursorRadius = glm::max(glm::abs(cameraPosition.x), glm::abs(cameraPosition.z));

static int currentMenuItem = 0;
const char *menuItems = "Move camera\0Place cursor\0Add element\0Select point";

glm::mat4 view;
glm::mat4 proj;

vector<int> selected;

void window_size_callback(GLFWwindow *window, int width, int height);
void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods);
void mouse_button_callback(GLFWwindow *window, int button, int action,
                           int mods);
void cursorHandleInput(GLFWwindow *window);
tuple<glm::vec3, glm::vec3> calculateNearFarProjections(double xMouse,
                                                        double yMouse);
void recalculateSelected();

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
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // init figures
    figures.push_back(new Torus());
    figures[0]->selected = true;
    selected.push_back(0);
    grid = new Grid();
    cursor = new Cursor();
    center = new Cursor();

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
        if (selected.size() > 0) {
          center->Render(colorLoc, modelLoc);
        }

        // imgui rendering
        if (ImGui::Begin("Mode")) {
          // mode selection
          ImGui::Combo(" ", &currentMenuItem, menuItems);

          // add object buttons
          if (currentMenuItem == 2) {
            ImGui::Separator();
            if (ImGui::Button("Torus")) {
              figures.push_back(new Torus(cursor->GetPosition()));
            }
            ImGui::SameLine();
            if (ImGui::Button("Point")) {
              figures.push_back(new Point(cursor->GetPosition()));
            }
          }
          
          ImGui::Separator();
          // object selection
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
                  recalculateSelected();
              }
          }

          // delete button
          if (selected.size() > 0) {
            ImGui::Separator();
            if (ImGui::Button("Delete selected")) {
              for (int i = selected.size() - 1; i >= 0; i--) {
                figures.erase(figures.begin() + selected[i]);
                recalculateSelected();
              }
            }
          }

          // change name window
          if (selected.size() == 1) 
          {
              ImGui::Separator();
              ImGui::InputText("Change name", &figures[selected[0]]->name);
              // display selected item menu
              figures[selected[0]]->CreateImgui();
          }
        }

        ImGui::End();
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
  camera.PrepareMatrices(view, proj);
  glViewport(0, 0, width, height);
}

void cursorHandleInput(GLFWwindow *window) 
{
  if (!ImGui::IsWindowFocused(ImGuiHoveredFlags_AnyWindow) &&
      !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)) 
  {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
      double mouseX;
      double mouseY;
      glfwGetCursorPos(window, &mouseX, &mouseY);
      tuple<glm::vec3, glm::vec3> projections =
          calculateNearFarProjections(mouseX, mouseY);

      vector<glm::vec3> points = CAD::circleIntersections(
          CAD::Sphere(get<0>(projections), cursorRadius), camera.Position,
          glm::normalize(get<1>(projections) - get<0>(projections)));
      // take positive solution
      cursor->SetPosition(points[1]);
    }
  }
}

tuple<glm::vec3, glm::vec3> calculateNearFarProjections(double xMouse,
                                                        double yMouse) {
  glm::mat4 invMat = glm::inverse(proj * view);

  float xMouseClip = (xMouse - camera.width / 2.0f) / (camera.width / 2.0f);
  float yMouseClip = -1 * (yMouse - camera.height / 2.0f) / (camera.height / 2.0f);

  glm::vec4 near = glm::vec4(xMouseClip, yMouseClip, -1.0f, 1.0f);
  glm::vec4 far = glm::vec4(xMouseClip, yMouseClip, 1.0f, 1.0f);
  glm::vec4 nearResult = invMat * near;
  glm::vec4 farResult = invMat * far;
  nearResult /= nearResult.w;
  farResult /= farResult.w;
  return tuple<glm::vec3, glm::vec3>(glm::vec3(nearResult),
                                     glm::vec3(farResult));
}

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods) {
  if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
    currentMenuItem = 0;
  } else if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
    currentMenuItem = 1;
  } else if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
    currentMenuItem = 2;
  } else if (key == GLFW_KEY_4 && action == GLFW_PRESS) {
    currentMenuItem = 3;
  }
}

void recalculateSelected() {
  selected.clear();
  for (int i = 0; i < figures.size(); i++) {
    if (figures[i]->selected)
      selected.push_back(i);
  }
  // center recalculation
  glm::vec3 centerVec(0.f);
  for (int i = 0; i < selected.size(); i++) {
    centerVec += figures[selected[i]]->GetTranslation() +
                 figures[selected[i]]->GetPosition();
  }
  centerVec /= selected.size();
  center->SetPosition(centerVec);
}

void mouse_button_callback(GLFWwindow *window, int button, int action,
                           int mods) {
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    if (currentMenuItem == 3) {
      double mouseX;
      double mouseY;
      glfwGetCursorPos(window, &mouseX, &mouseY);
      tuple<glm::vec3, glm::vec3> projections =
          calculateNearFarProjections(mouseX, mouseY);

      CAD::Sphere sphere;
      for (int i = 0; i < figures.size(); i++) {
        if (figures[i]->GetBoundingSphere(sphere)) {
          if (CAD::circleIntersections(
                  sphere, camera.Position,
                  glm::normalize(get<1>(projections) - get<0>(projections)))
                  .size() > 0) {
            figures[i]->selected = !figures[i]->selected;
            recalculateSelected();
          }
        }
      }
    }
  }
}