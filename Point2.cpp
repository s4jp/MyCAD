#include "Point2.h"

#include <glm/gtc/type_ptr.hpp>

std::string const Point2::GetClassName()
{
	return "Point";
}

Point2::Point2(float radius, glm::vec3 position, GLint modelLoc, GLint colorLoc, bool numerate)
	: radius(radius), PivotableFigure(position, modelLoc, GetClassName(), colorLoc, numerate) {
	Calculate(false);
}

void Point2::Calculate(bool recalculate)
{
	std::vector<GLfloat> vertices;
	std::vector<GLuint> indices;

	// bottom
	vertices.push_back(0.f);
	vertices.push_back(-radius);
	vertices.push_back(0.f);
	// left
	vertices.push_back(radius);
	vertices.push_back(0.f);
	vertices.push_back(0.f);
	// up
	vertices.push_back(0.f);
	vertices.push_back(0.f);
	vertices.push_back(-radius);
	// right
	vertices.push_back(-radius);
	vertices.push_back(0.f);
	vertices.push_back(0.f);
	// down
	vertices.push_back(0.f);
	vertices.push_back(0.f);
	vertices.push_back(radius);
	// top
	vertices.push_back(0.f);
	vertices.push_back(radius);
	vertices.push_back(0.f);


	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(4);

	indices.push_back(0);
	indices.push_back(2);
	indices.push_back(1);

	indices.push_back(0);
	indices.push_back(3);
	indices.push_back(2);

	indices.push_back(0);
	indices.push_back(4);
	indices.push_back(3);

	indices.push_back(5);
	indices.push_back(4);
	indices.push_back(1);

	indices.push_back(5);
	indices.push_back(1);
	indices.push_back(2);

	indices.push_back(5);
	indices.push_back(2);
	indices.push_back(3);

	indices.push_back(5);
	indices.push_back(3);
	indices.push_back(4);

	InitAndFillBuffers(vertices, indices);
}

void Point2::Render()
{
	vao.Bind();
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform4fv(colorLoc, 1, glm::value_ptr(GetColor()));
	glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_INT, 0);
	vao.Unbind();
}

bool Point2::GetBoundingSphere(CAD::Sphere& sphere)
{
	sphere = CAD::Sphere(GetPosition(), radius * 1.20f);
	return true;
}
