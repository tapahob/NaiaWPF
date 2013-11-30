#include "precompiled.h"
#include "NaiaLib.h"
#include "NaiaCore.h"
class NaiaCore;

NaiaCore::NaiaCore()
{
	renderCounter = -1;
	instance = NULL;
	Shaders.clear();
}

NaiaCore* NaiaCore::Instance()
{
	if (!NaiaCore::instance)
		NaiaCore::instance = new NaiaCore();

	return NaiaCore::instance;
}

NaiaCore* NaiaCore::instance = NULL;

void NaiaCore::Initialize()
{
	if (!initializeExtensions())
	{
		OutputDebugStringA("\n ########### Error: InitializeExtensions ##########\n");
		return;
	}

	LoadShaders();

	if (!Timer.Initialize())
		OutputDebugStringA("\n ########### Error: Initialize TIMER ##########\n");
}

void NaiaCore::Shutdown()
{
	/*wglMakeCurrent(Windows[0]->Hdc, g_GLMainContext);*/

	for (auto it = Windows.begin(); it != Windows.end(); it++)
		it->second->Shutdown();

	NaiaLib::SceneWrapper::Destroy();
	DestroyShaders();

	wglMakeCurrent(NULL, NULL);
	if (g_GLMainContext)
	{
		wglDeleteContext(g_GLMainContext);
		g_GLMainContext = NULL;
	}
}

//------------------------------------------------------------------------
//	Initialize Extensions
//------------------------------------------------------------------------
bool NaiaCore::initializeExtensions()
{
	// Create a fake window
	TCHAR szAppName [] = TEXT("fakeWindow");
	HWND         hwnd = { 0 };
	WNDCLASSEX   wndclassex = { 0 };
	wndclassex.cbSize = sizeof(WNDCLASSEX);
	wndclassex.style = CS_HREDRAW | CS_VREDRAW;
	wndclassex.lpfnWndProc = DefWindowProc;
	wndclassex.lpszClassName = szAppName;

	if (!RegisterClassEx(&wndclassex))
	{
		MessageBox(NULL, TEXT("RegisterClassEx failed!"), szAppName, MB_ICONERROR);
		return 0;
	}

	hwnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, szAppName, TEXT("WindowTitle"), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, NULL, NULL);

	ShowWindow(hwnd, SW_HIDE);
	if (!hwnd)
		return false;

	HDC deviceContext = GetDC(hwnd);
	PIXELFORMATDESCRIPTOR pixelFormatDescriptor = { 0 };

	if (!SetPixelFormat(deviceContext, 1, &pixelFormatDescriptor))
		return false;

	// Create Old Style context
	HGLRC renderingContext = wglCreateContext(deviceContext);
	wglMakeCurrent(deviceContext, renderingContext);

	glewExperimental = true;
	if (glewInit() != GLEW_OK)
		return false;

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(renderingContext);
	DestroyWindow(hwnd);
	UnregisterClass(szAppName, NULL);
	return true;
}


bool NaiaCore::LoadShaders()
{
	//------------------------------------------------------------------------
	//	Color Shader
	//------------------------------------------------------------------------
	GLSLShader colorShader;
	colorShader.LoadFromFile(GL_VERTEX_SHADER, "Shaders/ColorShader.vert");
	colorShader.LoadFromFile(GL_FRAGMENT_SHADER, "Shaders/ColorShader.frag");
	colorShader.CreateAndLinkProgram();

	colorShader.Use();
		colorShader.AddAttribute("Position");
		colorShader.AddAttribute("TexCoords");
		colorShader.AddAttribute("Normal");
		colorShader.AddUniform("MVP");
	colorShader.UnUse();

	Shaders["ColorShader"] = colorShader;
	return true;
}

void NaiaCore::DestroyShaders()
{
	for (auto it = Shaders.begin(); it != Shaders.end(); it++)
		it->second.DeleteShaderProgram();
}

void NaiaCore::AddWindow(OGLRenderer* window)
{
	renderCounter++;
	Windows[renderCounter] = window;
	window->id = renderCounter;
}

void NaiaCore::Render()
{
	for (auto it = Windows.begin(); it != Windows.end(); it++)
		it->second->Render();
}

void NaiaCore::Update()
{
	Timer.Update();
	return;
}

void NaiaCore::Loop()
{
	for (;;)
	{
		Update();
		Render();
	}
}

void NaiaCore::StartMainLoop()
{
	Loop();
}