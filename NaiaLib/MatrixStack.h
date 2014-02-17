#pragma once
#include <glm/glm.hpp>
class MatrixStack
{
public:
	MatrixStack();
	~MatrixStack();

public:
	void LoadIdentity();
	void Push();
	void Pop();
	void MultiplyMatrixLocal(glm::mat4x4& matrix);

public:
	glm::mat4x4 Top;
	vector<glm::mat4x4> Stack;
};

