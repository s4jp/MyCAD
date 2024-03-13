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

#include"shaderClass.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"

using namespace std;

const unsigned int width = 800;
const unsigned int height = 600;
const glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, -2.0f);
const float fov = M_PI / 4.0f;
const float near = 0.1f;
const float far = 100.0f;

float R1 = 0.5f;
float R2 = 0.2f;
int n1 = 10;
int n2 = 20;

int rotating = 1;

glm::vec3 scale = glm::vec3(1.0f);

void calculateTorusData(vector<GLfloat> &vertices, vector<GLuint> &indices,
                       float R1, float R2, int n1, int n2);
glm::mat4 createXrotationMatrix(float angle);
glm::mat4 createYrotationMatrix(float angle);
glm::mat4 createZrotationMatrix(float angle);
glm::mat4 rotate(glm::mat4 matrix, float angleX, float angleY, float angleZ);
glm::mat4 projection(float fov, float ratio, float near, float far);
glm::mat4 scaling(glm::mat4 matrix, glm::vec3 scale);

int main() { 
    vector<GLfloat> vertices;
    vector<GLuint> indices;
    calculateTorusData(vertices, indices, R1, R2, n1, n2);

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

    Shader shaderProgram("default.vert", "default.frag");

    VAO VAO1;
    VAO1.Bind();

    VBO VBO1(vertices.data(), vertices.size() * sizeof(GLfloat));
    EBO EBO1(indices.data(), indices.size() * sizeof(GLint));

    VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 0, (void *)0);
    VAO1.Unbind();
    VBO1.Unbind();
    EBO1.Unbind();

    float rotation = 0.0f;
    double prevTime = glfwGetTime();

    glLineWidth(5.0f);

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = translate(glm::mat4(1.0f), cameraPosition);
    glm::mat4 proj = projection(fov, (float)width / height, near, far);
    int modelLoc = glGetUniformLocation(shaderProgram.ID, "model");
    int viewLoc = glGetUniformLocation(shaderProgram.ID, "view");
    int projLoc = glGetUniformLocation(shaderProgram.ID, "proj");

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

        double crntTime = glfwGetTime();
        if (crntTime - prevTime >= 1 / 60) 
        {
            rotation += 0.05f;
            prevTime = crntTime;
        }

        calculateTorusData(vertices, indices, R1, R2, n1, n2);
        VBO1.ReplaceBufferData(vertices.data(),
                              vertices.size() * sizeof(GLfloat));
        EBO1.ReplaceBufferData(indices.data(), indices.size() * sizeof(GLint));

        model = rotate(scaling(glm::mat4(1.0f), scale), 0,
                       glm::radians(rotation), 0);

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

        VAO1.Bind();
        glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);

        if (ImGui::Begin("Options"))
        {
          ImGui::SeparatorText("Torus parameters");

          ImGui::SliderFloat("R1", &R1, 0.01f, 10.f, "%.2f");
          ImGui::SliderFloat("R2", &R2, 0.01f, 10.f, "%.2f");
          ImGui::SliderInt("n1", &n1, 1, 100);
          ImGui::SliderInt("n2", &n2, 1, 100);

          ImGui::SeparatorText("Transform mode");

          ImGui::RadioButton("rotate", &rotating, 1);
          ImGui::RadioButton("move", &rotating, 0);

          ImGui::SeparatorText("Scaling");

          ImGui::SliderFloat("Sx", &scale[0], 0.01f, 5.f, "%.2f");
          ImGui::SliderFloat("Sy", &scale[1], 0.01f, 5.f, "%.2f");
          ImGui::SliderFloat("Sz", &scale[2], 0.01f, 5.f, "%.2f");
        }
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

	VAO1.Delete();
    VBO1.Delete();
    EBO1.Delete();
    shaderProgram.Delete();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void calculateTorusData(vector<GLfloat> &vertices, vector<GLuint> &indices,
                       float R1, float R2, int n1, int n2) {
  if (R1 <= 0 || R2 <= 0 || n1 <= 0 || n2 <= 0) return;

  vertices.clear();
  indices.clear();

  float R1step = 2 * M_PI / n1;
  float R2step = 2 * M_PI / n2;

  for (int i = 0; i < n1; i++) {
    float xyElem = R1 + R2 * cos(i * R1step);
    float z = R2 * sin(i * R1step);

    for (int j = 0; j < n2; j++) {
      vertices.push_back(xyElem * cos(j * R2step)); // X
      vertices.push_back(xyElem * sin(j * R2step)); // Y
      vertices.push_back(z);                        // Z

      // R2 loop
      indices.push_back(i * n2 + j);                // current
      indices.push_back(i * n2 + ((j + 1) % n2));   // next
      // R1 loop
      indices.push_back(i * n2 + j);                // current
      indices.push_back(((i + 1) % n1) * n2 + j);   // next
    }
  }
}

glm::mat4 translate(glm::mat4 matrix, glm::vec3 vector) 
{
  glm::mat4 translationMatrix = glm::mat4(1.0f);
  translationMatrix[3] = glm::vec4(vector, 1.0f);
  return translationMatrix * matrix;
}

glm::mat4 createXrotationMatrix(float angle) 
{
  glm::mat4 result = glm::mat4(1.0f);
  result[1][1] = cos(angle);
  result[1][2] = sin(angle);
  result[2][1] = -sin(angle);
  result[2][2] = cos(angle);
  return result;
}

glm::mat4 createYrotationMatrix(float angle) 
{
  glm::mat4 result = glm::mat4(1.0f);
  result[0][0] = cos(angle);
  result[0][2] = -sin(angle);
  result[2][0] = sin(angle);
  result[2][2] = cos(angle);
  return result;
}

glm::mat4 createZrotationMatrix(float angle) 
{
  glm::mat4 result = glm::mat4(1.0f);
  result[0][0] = cos(angle);
  result[0][1] = sin(angle);
  result[1][0] = -sin(angle);
  result[1][1] = cos(angle);
  return result;
}

glm::mat4 rotate(glm::mat4 matrix, float angleX, float angleY, float angleZ) 
{
  return createZrotationMatrix(angleZ) * createYrotationMatrix(angleY) *
         createXrotationMatrix(angleX) * matrix;
}

glm::mat4 projection(float fov, float ratio, float near, float far) {
  glm::mat4 result = glm::mat4(0.0f);
  result[0][0] = 1.0f / (tan(fov / 2.0f) * ratio);
  result[1][1] = 1.0f / tan(fov / 2.0f);
  result[2][2] = -(far + near) / (far - near);
  result[3][2] = (-2.0 * far * near) / (far - near);
  result[2][3] = -1.0f;
  result[3][3] = 0.0f;
  return result;
}

glm::mat4 scaling(glm::mat4 matrix, glm::vec3 scale)
{
  glm::mat4 scaleMatrix = glm::mat4(1.0f);
  scaleMatrix[0][0] = scale[0];
  scaleMatrix[1][1] = scale[1];
  scaleMatrix[2][2] = scale[2];
  return scaleMatrix * matrix;
}
