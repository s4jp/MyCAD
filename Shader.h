#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include<glad/glad.h>
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>

std::string get_file_contents(const char* filename);

class Shader
{
public:
	GLuint ID;
	Shader(const char* vertexFile, const char* fragmentFile, const char* tcFile = nullptr, const char* teFile = nullptr);

	void Activate();
	void Delete();

private:
	void compileErrors(unsigned int shader, const char *type);
	GLuint compileShader(GLenum type, const char *file);
};
#endif