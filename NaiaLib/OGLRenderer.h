#pragma once
class OGLRenderer
{
public:
	OGLRenderer();
	OGLRenderer(const OGLRenderer&){};
	~OGLRenderer();

	bool Initialize(HWND hwnd, int width, int height);
	bool Resize(int width, int height);
	bool Render();
	void Shutdown();

private:
	bool InitializeExtensions();

public:
	HWND m_hwnd;
	HDC m_hDC;
	HGLRC m_RC;

	char m_videoCardDescription[256];
};

