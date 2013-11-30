#include "precompiled.h"
#include "EditorCore.h"
#include "NaiaCore.h"
#include "Input.h"

using namespace System;
using namespace System::Threading;

NaiaLib::EditorCore::EditorCore()
{
}

void NaiaLib::EditorCore::loop()
{
	NaiaCore::Instance()->StartMainLoop();
}

void NaiaLib::EditorCore::StartMainLoop()
{
	auto threadStart = gcnew ThreadStart(this, &NaiaLib::EditorCore::loop);
	RenderingThread = gcnew Thread(threadStart);
	RenderingThread->Start();
}

bool NaiaLib::EditorCore::Initialize(System::IntPtr hInstance, System::IntPtr hwnd)
{
	OutputDebugStringA("\n #### Input Controller Initializing .............. ");
	if (NaiaLib::Input::Instance()->Initialize((HINSTANCE)hInstance.ToPointer(), (HWND) hwnd.ToPointer()))
		OutputDebugStringA("[SUCCESS]\n");
	else 
		OutputDebugStringA("[FAILED]\n");
	NaiaCore::Instance()->Initialize();
	return true;
}

void NaiaLib::EditorCore::Shutdown()
{
	RenderingThread->Abort();
	OutputDebugStringA("\n ########## Rendering thread aborted! #########\n");
	NaiaCore::Instance()->Shutdown();
	NaiaLib::Input::Instance()->Shutdown();
}
