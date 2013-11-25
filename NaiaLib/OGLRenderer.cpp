#include "precompiled.h"
#include "Scene.h"
#include "OGLRenderer.h"
#include "NaiaCore.h"

HGLRC g_GLMainContext = NULL;

OGLRenderer::OGLRenderer(Scene* scene) : Hwnd(NULL), Hrc(NULL), Hdc(NULL)
{
	width = 800;
	height = 600;
	pScene = scene;
	id = -1;
}

OGLRenderer::~OGLRenderer()
{
}


//------------------------------------------------------------------------
//	Initialize OpenGL
//------------------------------------------------------------------------
bool OGLRenderer::Initialize(HWND hwnd, int width, int height)
{
	Hwnd = hwnd;

	int pixelFormat[1];
	UINT formatCount;
	int result;
	PIXELFORMATDESCRIPTOR pixelFormatDescriptor = {0};
	float FOV, aspect;
	char *vendorString, *rendererString;

	
	Hdc = GetDC(Hwnd);
	if (!Hdc)
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

	result = wglChoosePixelFormatARB(Hdc, pixelFormatAttribList, NULL, 1, pixelFormat, &formatCount);
	if (result != 1)
		return false;

	result = SetPixelFormat(Hdc, pixelFormat[0], &pixelFormatDescriptor);
	if (result != 1)
	{
		OutputDebugStringA("ERROR: CANNOT SET PIXEL FORMAT\n");
		auto er = GetLastError();
		return false;
	}

	// If main context is not yet created - create one
	if (!g_GLMainContext)
	{
		g_GLMainContext = wglCreateContextAttribsARB(Hdc, 0, contextAttribs);
		wglMakeCurrent(Hdc, g_GLMainContext);
		NaiaCore::Instance()->LoadShaders();
	}

	Hrc = wglCreateContextAttribsARB(Hdc, g_GLMainContext, contextAttribs);
	if (!Hrc)
		return false;

	wglMakeCurrent(Hdc, Hrc);
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	/*glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CW);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);*/


	//TODO: build identity matrix

	FOV = 3.14159265358979323846f / 4.0f;
	aspect = (float) width / (float) height;

	//TODO: build perspective projection

	vendorString = (char*) glGetString(GL_VENDOR);
	rendererString = (char*) glGetString(GL_RENDERER);

	strcpy_s(VideoCardDescription, vendorString);
	strcat_s(VideoCardDescription, " - ");
	strcat_s(VideoCardDescription, rendererString);

	// no vsync
	wglSwapIntervalEXT(1);
	glGenVertexArrays(1, &vao);
	OutputDebugStringA("\n############## INITIALIZE OPENGL SUCCESS ################\n");

	return true;
}

void OGLRenderer::Shutdown()
{
	if (Hrc)
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(Hrc);
		Hrc = NULL;
	}

	if (Hdc)
	{
		ReleaseDC(Hwnd, Hdc);
		Hdc = NULL;
	}
	return;
}

bool OGLRenderer::Render()
{
	wglMakeCurrent(Hdc, Hrc);
	glViewport(0, 0, width, height);
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindVertexArray(vao);
	if (pScene)
	{
		NaiaCore::Instance()->Shaders["ColorShader"].Use();
		pScene->Renderer = this;
		pScene->Render();
		NaiaCore::Instance()->Shaders["ColorShader"].UnUse();
	}
	
	SwapBuffers(Hdc);
	return true;
}

bool OGLRenderer::Resize(int width, int height)
{
	//wglMakeCurrent(Hdc, Hrc);
	this->width = width;
	this->height = height;
	//glViewport(0, 0, width, height);
	//TODO: build projection
	return true;
}
