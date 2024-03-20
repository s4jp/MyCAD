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

using namespace std;

const int width = 800;
const int height = 600;
const glm::vec3 cameraPosition = glm::vec3(3.0f, 3.0f, 3.0f);
const float fov = M_PI / 4.0f;
const float near = 0.1f;
const float far = 100.0f;

vector<Figure*> figures;
vector<int> selected;
Grid* grid;
Cursor *cursor;
Camera camera(width, height, cameraPosition, fov, near, far);

void window_size_callback(GLFWwindow *window, int width, int height);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

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

    // define callbacks
    glfwSetWindowSizeCallback(window, window_size_callback);
    //glfwSetScrollCallback(window, scroll_callback);       // FIX

    // init figures
    figures.push_back(new Torus(0.5f, 0.2f, 10, 20));
    selected.push_back(0);
    grid = new Grid(10.f, 50);
    cursor = new Cursor(0.2f);
    glm::vec3 cursorTranslation = cursor->GetTranslation();
    cursorTranslation.y = 1.f;
    cursor->SetTranslation(cursorTranslation);

    // matrices locations
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

        // camera
        camera.HandleInputs(window);
        camera.PrepareMatrices(viewLoc, projLoc);

        // objects rendering
        grid->Render(colorLoc, modelLoc);
        std::for_each(figures.begin(), figures.end(),
                      [colorLoc, modelLoc](Figure *f) {
                        f->Render(colorLoc, modelLoc);
                      });
        cursor->Render(colorLoc, modelLoc);

        // imgui rendering
        std::for_each(selected.begin(), selected.end(),
                      [](int idx) { figures[idx]->CreateImgui(); });

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

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) 
{
    //float yDiff = yoffset / 30.0f;

    //if (rotating == 1) 
    //{
    //  angle[2] += yDiff;
    //} 
    //else if (rotating == 0)
    //{
    //  translation[2] += yDiff;
    //}
}