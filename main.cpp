#define _USE_MATH_DEFINES
#include"imgui.h"
#include"imgui_impl_glfw.h"
#include"imgui_impl_opengl3.h"
#include"imgui_stdlib.h"

#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<vector>
#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<algorithm>

#include"Shader.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"
#include"Camera.h"

#include"figure.h"
#include"torus.h"
#include"grid.h"
#include"cursor.h"
#include"point.h"
#include"bezierC0.h"

const float near = 0.1f;
const float far = 100.0f;

std::vector<Figure*> figures;
std::vector<int> selected;
Grid* grid;
Camera *camera;
Cursor *center;
Cursor *cursor;

glm::mat4 view;
glm::mat4 proj;

static int currentMenuItem = 0;
const char *menuItems = "Move camera\0Place cursor\0Add element\0Select point";

void window_size_callback(GLFWwindow *window, int width, int height);
void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods);
void mouse_button_callback(GLFWwindow *window, int button, int action,
                           int mods);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void recalculateSelected(bool deleting = false);
void updateCurvesSelectedChange();
std::vector<int> GetClickedFigures(GLFWwindow *window);

glm::vec3 centerScale(1.f);
glm::vec3 centerAngle(0.f);
int cursorRadius = 5;

std::vector<BezierC0*> curves;
bool bezierSelection = false;
int division = 5;

int main() { 
    // initial values
    int width = 1500;
    int height = 800;
    glm::vec3 cameraPosition = glm::vec3(3.0f, 3.0f, 3.0f);
    float fov = M_PI / 4.0f;
    int guiWidth = 300;

    #pragma region gl_boilerplate
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(width, height, "MKMG", NULL, NULL);
    if (window == NULL) {
      std::cout << "Failed to create GLFW window" << std::endl;
      glfwTerminate();
      return -1;
    }
    glfwMakeContextCurrent(window);

    gladLoadGL();
    glViewport(0, 0, width - guiWidth, height);
    glEnable(GL_DEPTH_TEST);
    #pragma endregion

    // shaders and uniforms
    Shader shaderProgram("default.vert", "default.frag");
    int modelLoc = glGetUniformLocation(shaderProgram.ID, "model");
    int viewLoc = glGetUniformLocation(shaderProgram.ID, "view");
    int projLoc = glGetUniformLocation(shaderProgram.ID, "proj");
    int colorLoc = glGetUniformLocation(shaderProgram.ID, "color");

    Shader tessShaderProgram("tessellation.vert", "default.frag",
                             "tessellation.tesc", "tessellation.tese");
    int tessModelLoc = glGetUniformLocation(tessShaderProgram.ID, "model");
    int tessViewLoc = glGetUniformLocation(tessShaderProgram.ID, "view");
    int tessProjLoc = glGetUniformLocation(tessShaderProgram.ID, "proj");
    int tessColorLoc = glGetUniformLocation(tessShaderProgram.ID, "color");
    int tessDivisionLoc =
        glGetUniformLocation(tessShaderProgram.ID, "division");
    int tessCpCountLoc = glGetUniformLocation(tessShaderProgram.ID, "cpCount");

    // callbacks
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // init figures
    grid = new Grid();
    cursor = new Cursor();
    center = new Cursor();
    camera = new Camera(width, height, cameraPosition, fov, near, far, guiWidth);

    // matrices locations
    camera->PrepareMatrices(view, proj);

    #pragma region imgui_boilerplate
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");
    #pragma endregion

    while (!glfwWindowShouldClose(window)) 
    {
        #pragma region init
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::SetNextWindowSize(ImVec2(camera->guiWidth, camera->GetHeight()));
        ImGui::SetNextWindowPos(ImVec2(camera->GetWidth(), 0));
        #pragma endregion

        if (currentMenuItem == 0){
          camera->HandleInputs(window);
          camera->PrepareMatrices(view, proj);
        }

        // default shader activation
        shaderProgram.Activate();

        // matrices for default shader
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

        // objects rendering with default shader
        grid->Render(colorLoc, modelLoc);
        for (int i = 0; i < figures.size(); i++) {
          figures[i]->Render(colorLoc, modelLoc);
        }
        cursor->Render(colorLoc, modelLoc);
        if (selected.size() > 0) {
          center->Render(colorLoc, modelLoc);
        }
        // render curves polyline with default shader (still)
        for (int i = 0; i < curves.size(); i++) {
          if (curves[i]->selected) {
            curves[i]->RenderPolyline(colorLoc, modelLoc);
          }
        }

        // tessellation shader activation
        tessShaderProgram.Activate();

        // matrices for tessellation shader (with workaround)
        glm::mat4 tessView = glm::mat4(view);
        glm::mat4 tessProj = glm::mat4(proj);
        glUniformMatrix4fv(tessViewLoc, 1, GL_FALSE, glm::value_ptr(tessView));
        glUniformMatrix4fv(tessProjLoc, 1, GL_FALSE, glm::value_ptr(tessProj));

        // TEMP (to be deleted when adaptivity will get implemented)
        glUniform1i(tessDivisionLoc, division);

        // curves rendering with tessellation shader
        for (int i = 0; i < curves.size(); i++) {
          curves[i]->Render(tessColorLoc, tessModelLoc);
        }

        // imgui rendering
        if (ImGui::Begin("Menu", 0,
                         ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
          // mode selection
          if (ImGui::Combo(" ", &currentMenuItem, menuItems)) {
            if (currentMenuItem != 2) {
              bezierSelection = false;
            }
          }

          // TEMP (to be deleted when adaptivity will get implemented)
          ImGui::SliderInt("Bezier", &division, 1, 10);

          // cursor position & radius slider
          {
            cursor->CreateImgui();
            ImGui::Separator();
            ImGui::SliderInt("Radius", &cursorRadius, glm::max((int)near, 1),
                             glm::max((int)far, 1));
          }

          // add object buttons for object add mode
          if (currentMenuItem == 2) {
            ImGui::Separator();
            // torus
            if (ImGui::Button("Torus")) {
              figures.push_back(new Torus(cursor->GetPosition()));
            }
            // point
            ImGui::SameLine();
            if (ImGui::Button("Point")) {
              figures.push_back(new Point(cursor->GetPosition()));
            }
            // bezier C0
            ImGui::SameLine();
            if (ImGui::Button("Bezier C0")) {
              bezierSelection = !bezierSelection;
              if (bezierSelection) {
                curves.push_back(
                    new BezierC0(std::vector<Figure*>(), tessCpCountLoc));
              }
            }
            if (bezierSelection) {
              ImGui::Text("Bezier selection active!");
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
                recalculateSelected(true);
              }
            }
          }

          if (selected.size() == 1) 
          {
              ImGui::Separator();
              // change name window
              ImGui::InputText("Change name", &figures[selected[0]]->name);
              // display selected item position
              center->SetPosition(figures[selected[0]]->GetPosition());
              // display selected item menu
              if (figures[selected[0]]->CreateImgui()) {
                updateCurvesSelectedChange();
              }
          }

          // multiple figures manipulation
          if (selected.size() > 1) {
            bool change = false;
            // scaling manipulation
            ImGui::SeparatorText("Center scale");
            if (ImGui::InputFloat("cSx", &centerScale.x, 0.01f, 1.f, "%.2f")) {
              change = true;
              for (int i = 0; i < selected.size(); i++) {
                figures[selected[i]]->CalculatePivotTransformation(
                    center->GetPosition(), centerScale, centerAngle);
              }
            }
            if (ImGui::InputFloat("cSy", &centerScale.y, 0.01f, 1.f, "%.2f")) {
              change = true;
              for (int i = 0; i < selected.size(); i++) {
                figures[selected[i]]->CalculatePivotTransformation(
                    center->GetPosition(), centerScale, centerAngle);
              }
            }
            if (ImGui::InputFloat("cSz", &centerScale.z, 0.01f, 1.f, "%.2f")) {
              change = true;
              for (int i = 0; i < selected.size(); i++) {
                figures[selected[i]]->CalculatePivotTransformation(
                    center->GetPosition(), centerScale, centerAngle);
              }
            }
            if (ImGui::Button("Reset center scale")) {
              change = true;
              centerScale = glm::vec3(1.f);
              for (int i = 0; i < selected.size(); i++) {
                figures[selected[i]]->CalculatePivotTransformation(
                    center->GetPosition(), centerScale, centerAngle);
              }
            }
            // rotation manipulation
            ImGui::SeparatorText("Center scale");
            if (ImGui::SliderAngle("cX axis", &centerAngle.x, -180.f, 180.f)) {
              change = true;
              for (int i = 0; i < selected.size(); i++) {
                figures[selected[i]]->CalculatePivotTransformation(
                    center->GetPosition(), centerScale, centerAngle);
              }
            }
            if (ImGui::SliderAngle("cY axis", &centerAngle.y, -180.f, 180.f)) {
              change = true;
              for (int i = 0; i < selected.size(); i++) {
                figures[selected[i]]->CalculatePivotTransformation(
                    center->GetPosition(), centerScale, centerAngle);
              }
            }
            if (ImGui::SliderAngle("cZ axis", &centerAngle.z, -180.f, 180.f)) {
              change = true;
              for (int i = 0; i < selected.size(); i++) {
                figures[selected[i]]->CalculatePivotTransformation(
                    center->GetPosition(), centerScale, centerAngle);
              }
            }
            if (ImGui::Button("Reset center angle")) {
              change = true;
              centerAngle = glm::vec3(0.f);
              for (int i = 0; i < selected.size(); i++) {
                figures[selected[i]]->CalculatePivotTransformation(
                    center->GetPosition(), centerScale, centerAngle);
              }
            }
            if (change) {
              updateCurvesSelectedChange();
            }
          }
        }

        ImGui::End();
        #pragma region rest
        ImGui::Render();
        //std::cout << ImGui::GetIO().Framerate << std::endl;
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
        #pragma endregion
    }
    #pragma region exit
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    grid->Delete();
    cursor->Delete();
    center->Delete();
	std::for_each(figures.begin(), figures.end(),
                  [](Figure* f) { f->Delete(); });
    std::for_each(curves.begin(), curves.end(),
                  [](Figure *c) { c->Delete(); });
    shaderProgram.Delete();
    tessShaderProgram.Delete();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
    #pragma endregion
}

// callbacks
void window_size_callback(GLFWwindow *window, int width, int height) {
  camera->SetWidth(width);
  camera->SetHeight(height);
  camera->PrepareMatrices(view, proj);
  glViewport(0, 0, width - camera->guiWidth, height);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) 
{
  if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)) {
    cursorRadius = glm::clamp((int)(cursorRadius + yoffset),
                              glm::max((int)near, 1), glm::max((int)far, 1));
  }
}

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods) {
  if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
    currentMenuItem = 0;
    bezierSelection = false;
  } else if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
    currentMenuItem = 1;
    bezierSelection = false;
  } else if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
    currentMenuItem = 2;
  } else if (key == GLFW_KEY_4 && action == GLFW_PRESS) {
    currentMenuItem = 3;
    bezierSelection = false;
  }
}

void mouse_button_callback(GLFWwindow *window, int button, int action,
                           int mods) {
  if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) 
    {
      glm::vec3 newCursorPos =
          CAD::calculateNewCursorPos(window, proj, view, camera, cursorRadius);
      std::vector<int> clickedFigures = GetClickedFigures(window);

      // cursor movement
      if (currentMenuItem == 1) {
        cursor->SetPosition(newCursorPos);
      } 
      // point selection
      else if (currentMenuItem == 3) {
        for (int i = 0; i < clickedFigures.size(); i++) {
          figures[clickedFigures[i]]->selected = !figures[clickedFigures[i]]->selected;
          recalculateSelected();
        }
      }
      // add point to curve being created
      if (bezierSelection) {
        std::vector<Figure*> newCPs;
        // if any points have been clicked - add them
        if (clickedFigures.size() > 0) {
          for (int i = 0; i < clickedFigures.size(); i++) {
            newCPs.push_back(figures[clickedFigures[i]]);
          }
        } 
        // if none - add *only* newCursorPos
        else {
          newCPs.push_back(new Point(newCursorPos));
          figures.push_back(newCPs[newCPs.size() - 1]);
        }
        
        for (int i = 0; i < newCPs.size(); i++) {
          curves[curves.size() - 1]->AddControlPoint(newCPs[i]);
        }
      }
    }
  }
}

// other functions
void recalculateSelected(bool deleting) {
  // resolve center model matrix
  if (selected.size() > 1 && !deleting) {
    for (int i = 0; i < selected.size(); i++) {
      figures[selected[i]]->SavePivotTransformations();
    }
  }
  centerScale = glm::vec3(1.f);
  centerAngle = glm::vec3(0.f);
  // find selected figures
  selected.clear();
  for (int i = 0; i < figures.size(); i++) {
    if (figures[i]->selected)
      selected.push_back(i);
  }
  // center recalculation
  glm::vec3 centerVec(0.f);
  for (int i = 0; i < selected.size(); i++) {
    centerVec += figures[selected[i]]->GetPosition();
  }
  centerVec /= selected.size();
  center->SetPosition(centerVec);
}

void updateCurvesSelectedChange() {
  for (int i = 0; i < selected.size(); i++) {
    for (int j = 0; j < curves.size(); j++) {
      std::vector<Figure*> points = curves[j]->GetControlPoints();
      for (int k = 0; k < points.size(); k++) {
        if (figures[selected[i]] == points[k]) {
          curves[j]->RefreshBuffers();
        }
      }
    }
  }
}

std::vector<int> GetClickedFigures(GLFWwindow *window) {
  std::vector<int> result;

  CAD::Sphere sphere;
  for (int i = 0; i < figures.size(); i++) {
    if (figures[i]->GetBoundingSphere(sphere)) {
      if (CAD::circleIntersections(
              sphere, camera->Position,
              CAD::calculateCameraRay(window, proj, view, camera))
              .size() > 0) {
        result.push_back(i);
      }
    }
  }
  return result;
}
