#pragma once
using namespace System::Threading;

namespace NaiaLib
{
	public ref class EditorCore
	{
	public:
		bool Initialize(System::IntPtr hInstance, System::IntPtr hwnd);
		void StartMainLoop();
		void Shutdown();

	public:
		static property EditorCore^ Instance { EditorCore^ get() { return %instance; } };

	private:
		EditorCore();
		void loop();

	private:
		static  EditorCore instance;
		Thread^ RenderingThread;
	};
}
