#pragma once
#include "GLSLShader.h"
#include "NaiaTimer.h"

class NaiaCore
{
public:
	static NaiaCore* Instance();
	void Initialize();
	void Shutdown();
	void AddWindow(OGLRenderer* window);
	bool LoadShaders();
	void StartMainLoop();

protected:
	NaiaCore();
	
	void DestroyShaders();
	void Update();
	void Render();
	void Loop();
	bool initializeExtensions();

public:
	map<string, GLSLShader> Shaders;
	map<int, OGLRenderer*> Windows;
	Time Timer;
private:
	static NaiaCore* instance;
	int renderCounter;
};

