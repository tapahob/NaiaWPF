#include "precompiled.h"
#include "SceneWrapper.h"
#include "NaiaCore.h"
#include <iostream>
#include "MathHelper.h"
using namespace System;
using namespace System::Threading;

NaiaLib::SceneWrapper::SceneWrapper()
{
}

void NaiaLib::SceneWrapper::Init()
{
	pScene = new Scene();
}

void NaiaLib::SceneWrapper::Destroy()
{
	delete pScene;
}

void NaiaLib::SceneWrapper::AddMeshNode()
{
	if (!pScene)
		return;

	float gridScale = 0.1f;

	auto naiaCore = NaiaCore::Instance();
	
	auto newMeshNode = new MeshNode(pScene, MeshNode::LineGridXZ);
	newMeshNode->RenderWindow = 2;
	MathHelper::SetPosition(newMeshNode->Properties.ToWorld, glm::vec3(0.0f, -100.0f, 0.0f));
	MathHelper::SetScale(newMeshNode->Properties.ToWorld, glm::vec3(gridScale, gridScale, gridScale));
	//naiaCore->Windows[2]->Camera->AddChild(newMeshNode);
	pScene->RootNode.AddChild(newMeshNode);

	newMeshNode = new MeshNode(pScene, MeshNode::LineGridXY);
	newMeshNode->RenderWindow = 1;
	MathHelper::SetPosition(newMeshNode->Properties.ToWorld, glm::vec3(0.0f, 0.0f, -100.0f));
	MathHelper::SetScale(newMeshNode->Properties.ToWorld, glm::vec3(gridScale, gridScale, gridScale));
	//naiaCore->Windows[1]->Camera->AddChild(newMeshNode);
	pScene->RootNode.AddChild(newMeshNode);

	newMeshNode = new MeshNode(pScene, MeshNode::LineGridZY);
	newMeshNode->RenderWindow = 0;
	MathHelper::SetPosition(newMeshNode->Properties.ToWorld, glm::vec3(-100.0f, 0.0f, 0.0f));
	MathHelper::SetScale(newMeshNode->Properties.ToWorld, glm::vec3(gridScale, gridScale, gridScale));
	//naiaCore->Windows[0]->Camera->AddChild(newMeshNode);
	pScene->RootNode.AddChild(newMeshNode);

	/*newMeshNode = new MeshNode(pScene, MeshNode::TriangleGrid);
	newMeshNode->Properties.Name = "Floor";
	MathHelper::SetPosition(newMeshNode->Properties.ToWorld, glm::vec3(0.0f, 0.0f, 0.0f));
	MathHelper::SetScale(newMeshNode->Properties.ToWorld, glm::vec3(0.5f, 0.5f, 0.5f));
	pScene->RootNode.AddChild(newMeshNode);*/

	newMeshNode = new MeshNode(pScene, MeshNode::None);
	newMeshNode->Properties.Name = "Cube";
	MathHelper::SetPosition(newMeshNode->Properties.ToWorld, glm::vec3(0.0f, 0.0f, 0.0f));
	MathHelper::SetScale(newMeshNode->Properties.ToWorld, glm::vec3(3.0f, 3.0f, -3.0f));
	pScene->RootNode.AddChild(newMeshNode);
}

void NaiaLib::SceneWrapper::UpdateSceneWindow(TreeViewItem^ rootElement)
{
	cerr << "\nTotal Children: " << pScene->RootNode.children.size() << endl;

	rootElement->Items->Clear();

	for (auto it = pScene->RootNode.children.begin(); it != pScene->RootNode.children.end(); it++)
	{
		SceneNode* kid = *it;
		TreeViewItem^ newItem = gcnew TreeViewItem();
		newItem->Header = gcnew String( kid->Properties.Name.c_str());
		rootElement->Items->Add(newItem);
	}
}
