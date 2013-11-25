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
	NaiaCore::Instance()->Initialize();
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

void NaiaLib::SceneWrapper::StartMainLoop()
{
	Thread^ t = gcnew Thread(gcnew ThreadStart(loop));
	t->Start();
}

void NaiaLib::SceneWrapper::loop()
{
	NaiaCore::Instance()->StartMainLoop();
}
