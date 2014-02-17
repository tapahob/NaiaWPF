#pragma once

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

namespace NaiaLib
{
	class Input
	{
	public:
		static Input* Instance();
		~Input();

		bool Initialize(HINSTANCE hInstance, HWND hwnd);
		bool Update();
		void Shutdown();

		void GetMouseLocation(int& x, int& y);
		bool IsKeyDown(unsigned char key);
		bool IsMouseKeyDown(unsigned char key);

	private:
		Input();
		Input(const Input& other);
		bool ReadKeyboard();
		bool ReadMouse();
		void ProcessInput();

	private:
		static Input* instance;

		IDirectInput8*       m_directInput;
		IDirectInputDevice8* m_keyboard;
		IDirectInputDevice8* m_mouse;

		unsigned char      m_keyboardState[256];
		unsigned char      m_keyboardStateOld[256];
		DIMOUSESTATE       m_mouseState;
		int m_screenWidth, m_screenHeight;
		int m_mouseX, m_mouseY;
	};


}

