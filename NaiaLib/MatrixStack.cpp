#include "precompiled.h"
#include "MatrixStack.h"

MatrixStack::MatrixStack()
{
}

MatrixStack::~MatrixStack()
{
}

void MatrixStack::LoadIdentity()
{
	Stack.clear();
	Top = glm::mat4x4(1.0f);
}

void MatrixStack::Push()
{
	Stack.push_back(Top);
}

void MatrixStack::Pop()
{
	if (Stack.size() == 0)
		return;

	Top = Stack[Stack.size() - 1];
	Stack.pop_back();
}

void MatrixStack::MultiplyMatrixLocal(glm::mat4x4& matrix)
{
	Top = matrix * Top;
}
