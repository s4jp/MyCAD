#include"Shader.h"

#include<vector>

std::string get_file_contents(const char* filename)
{
	std::ifstream in(filename, std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	throw(errno);
}

Shader::Shader(const char *vertexFile, const char *fragmentFile,
               const char *tcFile, const char *teFile) {
	std::vector<GLuint> shaders;
    shaders.push_back(compileShader(GL_VERTEX_SHADER, vertexFile));
    shaders.push_back(compileShader(GL_FRAGMENT_SHADER, fragmentFile));
    if (tcFile != nullptr && teFile != nullptr) {
      shaders.push_back(compileShader(GL_TESS_CONTROL_SHADER, tcFile));
      shaders.push_back(compileShader(GL_TESS_EVALUATION_SHADER, teFile));
    }

	ID = glCreateProgram();
    for (int i = 0; i < shaders.size(); i++) {
       glAttachShader(ID, shaders[i]);
    }
	glLinkProgram(ID);
    compileErrors(ID, "PROGRAM");

	for (int i = 0; i < shaders.size(); i++) {
      glDeleteShader(shaders[i]);
    }
}

void Shader::Activate() {
	glUseProgram(ID);
}

void Shader::Delete()
{
	glDeleteProgram(ID);
}

void Shader::compileErrors(unsigned int shader, const char *type) 
{
	GLint hasCompiled;
	char infoLog[1024];
	if (type != "PROGRAM") 
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE) 
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "SHADER_COMPILATION_ERROR for:" << type << "\n"
			          << infoLog << std::endl;
		}
	} else 
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE) 
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "SHADER_LINKING_ERROR for:" << type << "\n"
			          << infoLog << std::endl;
		}
	}
}

GLuint Shader::compileShader(GLenum type, const char *file) {
  std::string code = get_file_contents(file);
  const char *source = code.c_str();

  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &source, NULL);
  glCompileShader(shader);
  compileErrors(shader, file);
  return shader;
}
