#pragma once
#include "MatrixStack.h"
#include "SceneNodes.h"

class OGLRenderer;

class Scene
{
public:
	Scene();
	~Scene();

public:
	void PushAndSetMatrix(glm::mat4x4 matrix);
	void PopMatrix();
	void Render() { RootNode.Render(); };
	void Destroy() { RootNode.Destroy(); };

private:
	glm::mat4x4 getTopMatrix();

public:
	SceneNode RootNode;
	std::map<int, SceneNode*> SceneNodeList;
	int idCounter;
	OGLRenderer* Renderer;

private:
	MatrixStack stack;
	
};