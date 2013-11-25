#include "precompiled.h"
#include "SceneWrapper.h"
#include "NaiaCore.h"

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
}
