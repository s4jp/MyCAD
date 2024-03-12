#define _USE_MATH_DEFINES
#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<cmath>
#include<vector>

#include"shaderClass.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"

using namespace std;

void calculateTorusData(vector<GLfloat> &vertices, vector<GLuint> &indices,
                       float R1, float R2, int n1, int n2);

int main() { 
    vector<GLfloat> vertices;
    vector<GLuint> indices;
    calculateTorusData(vertices, indices, 0.5, 0.2, 10, 20);

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "MKMG", NULL, NULL);
    if (window == NULL) {
      std::cout << "Failed to create GLFW window" << std::endl;
      glfwTerminate();
      return -1;
    }
    glfwMakeContextCurrent(window);

    gladLoadGL();
    glViewport(0, 0, 800, 600);

    Shader shaderProgram("default.vert", "default.frag");

    VAO VAO1;
    VAO1.Bind();

    VBO VBO1(vertices.data(), vertices.size() * sizeof(GLfloat));
    EBO EBO1(indices.data(), indices.size() * sizeof(GLint));

    VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 0, (void *)0);
    VAO1.Unbind();
    VBO1.Unbind();
    EBO1.Unbind();

    while (!glfwWindowShouldClose(window)) 
    {
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

		shaderProgram.Activate();
        VAO1.Bind();
        glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

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
