#include "precompiled.h"
#include "NaiaLib.h"
#include "NaiaCore.h"
#include "MathHelper.h"
#include "Input.h"
#include "EditorCore.h"

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
	window->Camera = new CameraNode(window->pScene);
	window->pScene->RootNode.AddChild(window->Camera);
	window->Camera->Properties.Name = "Camera";

	switch (window->id)
	{
	case (0) :	// Side
		MathHelper::SetPosition(window->Camera->Properties.ToWorld, glm::vec3(500.0f, 0.0f, 0.0f));
		//window->State.Wireframe = true;
		break;
	case (1) :	// Front
		MathHelper::SetPosition(window->Camera->Properties.ToWorld, glm::vec3(0.0f, 0.0f, 500.0f));
		break;
	case (2) :	// Top
		//window->State.Wireframe = true;
		MathHelper::SetPosition(window->Camera->Properties.ToWorld, glm::vec3(0.0f, 500.0f, 0.0f));
		break;
	case (3) :	// Perspective
		window->State.UsePerspective = true;
		MathHelper::SetPosition(window->Camera->Properties.ToWorld, glm::vec3(500.0f, 500.0f, 500.0f));
		break;
	}

}

void NaiaCore::Render()
{
	for (auto it = Windows.begin(); it != Windows.end(); it++)
		it->second->Render();
}

void NaiaCore::Update()
{
	Timer.Update();

	const float cameraSpeed = 0.10f;

	auto deltaTime = Timer.DeltaTime();

	auto camera = NaiaLib::SceneWrapper::pScene->Camera();

	if (!camera)
		return;

	SceneNode* modelNode = NaiaLib::SceneWrapper::pScene->RootNode.children[NaiaLib::SceneWrapper::pScene->RootNode.children.size() - 1];
	modelNode->Properties.ToWorld = glm::rotate(modelNode->Properties.ToWorld, deltaTime * 0.07f, 0.0f, 1.0f, 0.0f);

	NaiaLib::Input::Instance()->Update();

	bool wkeyPressed = NaiaLib::Input::Instance()->IsKeyDown(DIK_W);
	bool sPressed = NaiaLib::Input::Instance()->IsKeyDown(DIK_S);

	int activeWindow = NaiaLib::EditorCore::Instance->activeRenderer;

	if (wkeyPressed || sPressed)
	{
		glm::vec3 direction(0.0f, 0.0f, 1.0f);
		switch (activeWindow)
		{
		case 0:
			direction = glm::vec3(0.0f, 1.0f, 0.0f);
			break;
		case 1:
			direction = glm::vec3(0.0f, 1.0f, 0.0f);
			break;
		case 2:
			direction = glm::vec3(0.0f, 0.0f, 1.0f);
			break;
		}
			
		auto delta = direction * deltaTime * cameraSpeed;

		if (wkeyPressed)
			delta *= -1.0f;

			camera->Target += delta;

		auto oldPos = MathHelper::GetPosition(camera->Properties.ToWorld);

		oldPos += delta;

		MathHelper::SetPosition(camera->Properties.ToWorld, oldPos);
	}

	bool dKeyPressed = NaiaLib::Input::Instance()->IsKeyDown(DIK_D);
	bool aPressed = NaiaLib::Input::Instance()->IsKeyDown(DIK_A);
	
	if (dKeyPressed || aPressed)
	{
		glm::vec3 direction = glm::vec3(1.0f, 0.0f, 0.0f);

		switch (activeWindow)
		{
		case 0:
			direction = glm::vec3(0.0f, 0.0f, 1.0f);
			break;
		case 1:
			direction = glm::vec3(-1.0f, 0.0f, 0.0f);
			break;
		}

		auto delta = direction * deltaTime * cameraSpeed;

		if (dKeyPressed)
			delta *= -1.0f;

		camera->Target += delta;

		auto oldPos = MathHelper::GetPosition(camera->Properties.ToWorld);
		oldPos += delta;

		MathHelper::SetPosition(camera->Properties.ToWorld, oldPos);
	}

	bool qKeyPressed = NaiaLib::Input::Instance()->IsKeyDown(DIK_Q);
	bool ePressed = NaiaLib::Input::Instance()->IsKeyDown(DIK_E);

	if (qKeyPressed || ePressed)
	{
		glm::vec3 direction = glm::vec3(0.0f, 1.0f, 0.0f);
		auto delta = direction * deltaTime * cameraSpeed;

		if (qKeyPressed)
			delta *= -1.0f;

		camera->Target += delta;

		auto oldPos = MathHelper::GetPosition(camera->Properties.ToWorld);
		oldPos += delta;

		MathHelper::SetPosition(camera->Properties.ToWorld, oldPos);
	}
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