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
	glm::mat4x4				MVP;
	glm::mat4x4				ProjectionMatrix;
	char                    VideoCardDescription[256];
	Scene*					pScene;
	int						width, height;
};

