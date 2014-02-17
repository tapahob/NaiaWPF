#pragma once
#include "GLSLShader.h"

class Scene;
class CameraNode;

class OGLRenderer
{
public:
	struct RendererState
	{
		RendererState()
		{
			UsePerspective = false;
			Wireframe = false;
		}

		bool UsePerspective;
		bool Wireframe;
	};

public:
	OGLRenderer(Scene* scene);
	OGLRenderer(const OGLRenderer&){};
	~OGLRenderer();

	bool Initialize(HWND hwnd, int width, int height);
	bool Resize(const int width, const int height);
	bool Render();
	void Shutdown();

public:
	RendererState			State;
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
	CameraNode*				Camera;
	int						width, height;
};

