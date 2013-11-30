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

	pScene->RootNode.AddChild(new MeshNode(pScene));

	auto newMeshNode = new MeshNode(pScene);
	MathHelper::SetPosition(newMeshNode->Properties.ToWorld, glm::vec3(0.0f, 0.5f, 1.0f));
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
