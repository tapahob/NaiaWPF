#pragma once
#include "GLSLShader.h"
class Scene;
class OGLRenderer
{
public:
	OGLRenderer(Scene* scene);
	OGLRenderer(const OGLRenderer&){};
	~OGLRenderer();

	bool Initialize(HWND hwnd, int width, int height);
	bool Resize(int width, int height);
	bool Render();
	void Shutdown();

public:
	GLuint					vao;
	int						id;
	HWND                    Hwnd;
	HDC                     Hdc;
	HGLRC                   Hrc;
	glm::mat4x4             WorldMatrix;
	char                    VideoCardDescription[256];
	map<string, GLSLShader> Shaders;
	Scene*					pScene;
	int width, height;
};

