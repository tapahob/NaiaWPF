#include "precompiled.h"
#include "Scene.h"
#include "OGLRenderer.h"

Scene::Scene()// : //RootNode(this), idCounter(0)
{
	idCounter = 0;
	RootNode = SceneNode(this);
	RootNode.Properties.Name = "RootNode";
}

Scene::~Scene()
{
}

void Scene::PopMatrix()
{
	stack.Pop();
	Renderer->WorldMatrix = getTopMatrix();
}

void Scene::PushAndSetMatrix(glm::mat4x4 matrix)
{
	stack.Push();
	stack.MultiplyMatrixLocal(matrix);
	Renderer->WorldMatrix = getTopMatrix();
}

glm::mat4x4 Scene::getTopMatrix()
{
	return stack.Top;
}
