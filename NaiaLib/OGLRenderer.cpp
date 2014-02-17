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
	WorldMatrix = glm::mat4x4(1.0f);
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

	glClearDepth(1.0f);
	glFrontFace(GL_CW);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);


	FOV = 3.14159265358979323846f / 4.0f;
	aspect = (float) width / (float) height;

	if (State.UsePerspective)
		ProjectionMatrix = glm::perspective(FOV, aspect, 0.1f, 1000.0f);
	else
	{
		float halfWidth = (float) width / 2.0f;
		float halfHeight = (float) height / 2.0f;
		float zoom = 0.5f;
		ProjectionMatrix = glm::ortho(-halfWidth * zoom, halfWidth * zoom, halfHeight * zoom, -halfHeight * zoom, 0.1f, 1000.0f);
	}


	vendorString = (char*) glGetString(GL_VENDOR);
	rendererString = (char*) glGetString(GL_RENDERER);

	strcpy_s(VideoCardDescription, vendorString);
	strcat_s(VideoCardDescription, " - ");
	strcat_s(VideoCardDescription, rendererString);

	// no vsync
	wglSwapIntervalEXT(0);
	glGenVertexArrays(1, &vao);
	OutputDebugStringA(" SUCCESS ################\n");	
	wglMakeCurrent(Hdc, g_GLMainContext);
	return true;
}

void OGLRenderer::Shutdown()
{
	glBindVertexArray(NULL);
	glDeleteVertexArrays(1, &vao);

	if (Hrc)
	{
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
	glClearColor(0.17f, 0.17f, 0.17f, 1.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindVertexArray(vao);
	if (pScene)
	{
		auto shader = NaiaCore::Instance()->Shaders["ColorShader"];
		shader.Use();	
		pScene->Renderer = this;
		pScene->Render();
		shader.UnUse();
	}
	
	SwapBuffers(Hdc);
	return true;
}

bool OGLRenderer::Resize(int width, int height)
{
	this->width = width;
	this->height = height;

	float FOV = 3.14159265358979323846f / 4.0f * 4.0f;
	float aspect = (float) width / (float) height;

	if (State.UsePerspective)
		ProjectionMatrix = glm::perspective(FOV, aspect, 10.0f, 100000.0f);
	else
	{		
		float halfWidth = (float) width / 2.0f;
		float halfHeight = (float) height / 2.0f;
		float zoom = 0.1f;
		ProjectionMatrix = glm::ortho(-halfWidth * zoom, halfWidth * zoom, -halfHeight * zoom, halfHeight * zoom, 0.1f, 1000.0f);
	}
	return true;
}
