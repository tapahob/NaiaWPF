#include "stdafx.h"
#include <GL/glew.h>
#include <GL/wglew.h>
#include <gl/GL.h>
#include "OGLRenderer.h"

OGLRenderer::OGLRenderer() :m_hwnd(NULL), m_RC(NULL), m_hDC(NULL)
{
}


OGLRenderer::~OGLRenderer()
{
}

//------------------------------------------------------------------------
//	Initialize Extensions
//------------------------------------------------------------------------
bool OGLRenderer::InitializeExtensions()
{
	// Create a fake window
	TCHAR szAppName [] = TEXT("fakeWindow");
	HWND         hwnd = {0};
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

//------------------------------------------------------------------------
//	Initialize OpenGL
//------------------------------------------------------------------------
bool OGLRenderer::Initialize(HWND hwnd, int width, int height)
{
	m_hwnd = hwnd;

	if (!InitializeExtensions())
	{
		OutputDebugStringA(" ########### Error: InitializeExtensions ##########\n");
		return false;
	}

	int pixelFormat[1];
	UINT formatCount;
	int result;
	PIXELFORMATDESCRIPTOR pixelFormatDescriptor = {0};
	float FOV, aspect;
	char *vendorString, *rendererString;

	
	m_hDC = GetDC(m_hwnd);
	if (!m_hDC)
		return false;

	const int pixelFormatAttribList [] =
	{
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_DOUBLE_BUFFER_ARB , GL_TRUE,
		WGL_PIXEL_TYPE_ARB    , WGL_TYPE_RGBA_ARB,
		WGL_COLOR_BITS_ARB    , 32,
		WGL_DEPTH_BITS_ARB    , 24,
		WGL_STENCIL_BITS_ARB  , 8,
		0 // End of attributes list
	};

	int contextAttribs [] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
		WGL_CONTEXT_MINOR_VERSION_ARB, 0,
		WGL_CONTEXT_FLAGS_ARB        , WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
		0 // End of attributes list
	};

	result = wglChoosePixelFormatARB(m_hDC, pixelFormatAttribList, NULL, 1, pixelFormat, &formatCount);
	if (result != 1)
		return false;

	result = SetPixelFormat(m_hDC, pixelFormat[0], &pixelFormatDescriptor);
	if (result != 1)
	{
		OutputDebugStringA("ERROR: CANNOT SET PIXEL FORMAT\n");
		auto er = GetLastError();
		return false;
	}


	m_RC = wglCreateContextAttribsARB(m_hDC, 0, contextAttribs);
	if (!m_RC)
		return false;

	wglMakeCurrent(m_hDC, m_RC);
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CW);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);


	//TODO: build identity matrix

	FOV = 3.14159265358979323846f / 4.0f;
	aspect = (float) width / (float) height;

	//TODO: build perspective projection

	vendorString = (char*) glGetString(GL_VENDOR);
	rendererString = (char*) glGetString(GL_RENDERER);

	strcpy_s(m_videoCardDescription, vendorString);
	strcat_s(m_videoCardDescription, " - ");
	strcat_s(m_videoCardDescription, rendererString);

	// no vsync
	wglSwapIntervalEXT(0);

	OutputDebugStringA("############## INITIALIZE OPENGL SUCCESS ################");
	return true;
}

void OGLRenderer::Shutdown()
{
	if (m_RC)
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(m_RC);
		m_RC = NULL;
	}

	if (m_hDC)
	{
		ReleaseDC(m_hwnd, m_hDC);
		m_hDC = NULL;
	}

	return;
}

bool OGLRenderer::Render()
{
	wglMakeCurrent(m_hDC, m_RC);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glFlush();
	SwapBuffers(m_hDC);
	return true;
}

bool OGLRenderer::Resize(int width, int height)
{
	glViewport(0, 0, width, height);
	//TODO: build projection
	return true;
}
