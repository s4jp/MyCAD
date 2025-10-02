#pragma once

#include <imgui.h>
#include <imgui_stdlib.h>
#include <iomanip>

static std::string calculateFormat(float num) {
	std::ostringstream out;
	out << std::fixed << std::setprecision(std::numeric_limits<float>::max_digits10) << num;
	std::string numStr = out.str();

	size_t dotPos = numStr.find('.');
	if (dotPos == std::string::npos) {
		return "%.0f";
	}

	size_t lastNonZeroPos = numStr.find_last_not_of('0');
	if (lastNonZeroPos == std::string::npos || lastNonZeroPos < dotPos) {
		lastNonZeroPos = numStr.size() - 1;
	}

	std::string trimmedNumStr = numStr.substr(0, numStr.size() - 1);
	size_t trimmedLastNonZeroPos = trimmedNumStr.find_last_not_of('0');
	if (trimmedLastNonZeroPos == std::string::npos || trimmedLastNonZeroPos < dotPos) {
		trimmedLastNonZeroPos = trimmedNumStr.size() - 1;
	}

	if (trimmedLastNonZeroPos < lastNonZeroPos){
		lastNonZeroPos = trimmedLastNonZeroPos;
	}

	if (lastNonZeroPos == dotPos) {
		return "%.0f";
	}

	int decimalPlaces = static_cast<int>(lastNonZeroPos - dotPos);

	std::ostringstream formatOut;
	formatOut << "%." << decimalPlaces << "f";

	return formatOut.str();
}

class ControlledInputFloat {
public:
	ControlledInputFloat(std::string label, float value, float step = 0.1f, float lowerBound = -FLT_MAX, float upperBound = FLT_MAX) {
		this->label = label;
		this->value = value;
		this->step = step;
		this->format = calculateFormat(step);
		this->lowerBound = lowerBound;
		this->upperBound = upperBound;
	}

	bool Render() {
		bool change = false;
		if (ImGui::InputFloat(label.c_str(), &value, step, step * 10, format.c_str()))
		{
			change = true;
			if (value < lowerBound) {
				value = lowerBound;
			}
			else if (value > upperBound) {
				value = upperBound;
			}
		}
		return change;
	}

	float GetValue() const {
		return value;
	}

private:
	std::string label;
	float value;
	float step;
	std::string format;
	float lowerBound;
	float upperBound;
};