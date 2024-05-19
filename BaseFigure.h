#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>

#include "VAO.h"
#include "VBO.h"
#include "EBO.h"

class BaseFigure {
private:
	static int counter;

	const int id;
protected:
	VAO vao;
	VBO vbo;
	EBO ebo;
	GLint colorLoc;
	size_t indices_count;
	GLint modelLoc;
	glm::mat4 model;
public:
	std::string name;
	bool selected;

	BaseFigure(GLint modelLoc, std::string name, GLint colorLoc, bool numerate);
	~BaseFigure();

	void InitAndFillBuffers(std::vector<GLfloat> vertices, std::vector<GLuint> indices);
	void RefreshBuffers(std::vector<GLfloat> vertices, std::vector<GLuint> indices);
	glm::vec4 GetColor() const;
	virtual void Render() = 0;
	virtual void Calculate(bool recalculate) = 0;
	virtual bool CreateImgui();
	virtual const std::string GetClassName() = 0;
};