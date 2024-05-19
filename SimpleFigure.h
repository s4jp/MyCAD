#pragma once
#include "BaseFigure.h"

class SimpleFigure : public BaseFigure {
private:
	glm::vec3 position;
public:
	SimpleFigure(glm::vec3 position, GLint modelLoc, std::string name, GLint colorLoc, bool numerate);

	virtual glm::mat4 CalculateModelMtx();
	virtual glm::vec3 GetPosition();
	virtual void SetPosition(glm::vec3 position);
	virtual bool CreateImgui() override;
};