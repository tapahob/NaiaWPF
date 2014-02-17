#include "precompiled.h"
#include "EditorCore.h"
#include "NaiaCore.h"
#include "Input.h"

using namespace System;
using namespace System::Threading;
using namespace System::Text;

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
	RenderingThread->Name = gcnew String("RenderingThread");
	RenderingThread->Start();
	RenderingThread->CurrentThread->Name = gcnew String("MainThread");
}

bool NaiaLib::EditorCore::Initialize(System::IntPtr hInstance, System::IntPtr hwnd)
{
	activeRenderer = -1;
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

std::wstring NaiaLib::EditorCore::ShiftJIS_To_UTF8(const unsigned char* str, int size)
{
	auto bytes = gcnew array<Byte, 1>(size * sizeof(Byte));
	for (int i = 0; i < size; ++i)
		bytes[i] = str[i];

	System::String^ encodedString = Encoding::UTF8->GetString( Encoding::Convert(Encoding::GetEncoding(932), Encoding::UTF8, bytes));
	std::wstring result = (reinterpret_cast<wchar_t*>(System::Runtime::InteropServices::Marshal::StringToHGlobalUni(encodedString).ToPointer()));
	return result;
}
