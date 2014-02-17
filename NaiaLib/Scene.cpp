#include "precompiled.h"
#include "Scene.h"
#include "OGLRenderer.h"
#include "MathHelper.h"
#include "NaiaCore.h"
#include "EditorCore.h"

Scene::Scene()
{
	idCounter = 0;
	RootNode = SceneNode(this);
	RootNode.Properties.Name = "RootNode";

	/*Camera = new CameraNode(this);
	Camera->Properties.Name = "Default Camera";
	MathHelper::SetPosition(Camera->Properties.ToWorld, glm::vec3(0.0f, 80.0f, 0.0f));
	RootNode.children.push_back(Camera);*/
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

CameraNode* Scene::Camera()
{
	int windowIndex = NaiaLib::EditorCore::Instance->activeRenderer;
	if (windowIndex == -1)
		return NULL;

	auto activeWindow = NaiaCore::Instance()->Windows[windowIndex];
	return activeWindow->Camera;
}
