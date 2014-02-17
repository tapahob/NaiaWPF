#pragma once
#include "precompiled.h"

using namespace System::Threading;

namespace NaiaLib
{
	public ref class EditorCore
	{
	public:
		bool Initialize(System::IntPtr hInstance, System::IntPtr hwnd);
		void StartMainLoop();
		void Shutdown();

		static std::wstring ShiftJIS_To_UTF8(const unsigned char* str, int size);

	public:
		static property EditorCore^ Instance { EditorCore^ get() { return %instance; } };
		int activeRenderer;

	private:
		EditorCore();
		void loop();

	private:
		static  EditorCore instance;
		Thread^ RenderingThread;
	};
}
