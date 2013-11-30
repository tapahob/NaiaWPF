#include "precompiled.h"
#include "Input.h"

NaiaLib::Input* NaiaLib::Input::instance = NULL;

NaiaLib::Input::Input()
{
	m_directInput = 0;
	m_keyboard = 0;
	m_mouse = 0;
}

NaiaLib::Input::Input(const NaiaLib::Input& other)
{

}


NaiaLib::Input::~Input()
{
}

bool NaiaLib::Input::Initialize(HINSTANCE hInstance, HWND hwnd)
{
	int width;
	int height;
	width = 800; 
	height = 600;
	HRESULT result;

	m_screenWidth = width;
	m_screenHeight = height;

	m_mouseX = 0;
	m_mouseY = 0;

	result = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**) &m_directInput, NULL);
	if (FAILED(result))
		return false;

	// init keyboard
	result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
	if (FAILED(result))
		return false;

	result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result))
		return false;

	result = m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(result))
		return false;

	result = m_keyboard->Acquire();
	if (FAILED(result))
		return false;

	// init mouse
	result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	if (FAILED(result))
		return false;

	result = m_mouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(result))
		return false;

	result = m_mouse->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	if (FAILED(result))
		return false;

	result = m_mouse->Acquire();
	if (FAILED(result))
		return false;

	return true;
}

void NaiaLib::Input::Shutdown()
{
	// Release the mouse

	if (m_mouse)
	{
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = 0;
	}

	// release keyboard

	if (m_keyboard)
	{
		m_keyboard->Unacquire();
		m_keyboard->Release();
		m_mouse = 0;
	}

	// Release direct input

	if (m_directInput)
	{
		m_directInput->Release();
		m_directInput = 0;
	}

	delete Input::instance;
	Input::instance = NULL;
	return;
}

bool  NaiaLib::Input::Update()
{
	bool result;

	result = ReadKeyboard();
	if (!result)
		return false;

	result = ReadMouse();
	if (!result)
		return false;

	ProcessInput();

	return true;
}

bool NaiaLib::Input::ReadMouse()
{
	HRESULT result;

	result = m_mouse->GetDeviceState(sizeof(m_mouseState), (LPVOID) &m_mouseState);
	if (FAILED(result))
	{
		if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED)
			m_mouse->Acquire();
		else
			return false;
	}

	return true;
}

bool NaiaLib::Input::ReadKeyboard()
{
	HRESULT result;

	result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID) &m_keyboardState);
	if (FAILED(result))
	{
		if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED)
			m_keyboard->Acquire();
		else
			return false;
	}

	return true;
}

void NaiaLib::Input::ProcessInput()
{
	m_mouseX += m_mouseState.lX;
	m_mouseY += m_mouseState.lY;

	if (m_mouseX > m_screenWidth) { m_mouseX = m_screenWidth; }
	if (m_mouseY > m_screenHeight) { m_mouseY = m_screenHeight; }

	if (m_mouseX < 0) { m_mouseX = 0; }
	if (m_mouseY < 0) { m_mouseY = 0; }

	return;
}

void NaiaLib::Input::GetMouseLocation(int& mouseX, int& mouseY)
{
	mouseX = m_mouseX;
	mouseY = m_mouseY;
	return;
}

bool NaiaLib::Input::IsKeyDown(unsigned char key)
{
	if (m_keyboardState[key] & 0x80)
		return true;
	else
		return false;
}

NaiaLib::Input* NaiaLib::Input::Instance()
{
	if (!NaiaLib::Input::instance)
		NaiaLib::Input::instance = new NaiaLib::Input();

	return NaiaLib::Input::instance;
}




