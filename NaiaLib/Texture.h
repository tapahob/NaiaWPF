#pragma once
class Texture
{
public:
	Texture();
	~Texture();

	bool LoadTexture(string path);
	void Bind();

public:
	bool Loaded;
	int	 id;

private:
	GLuint textureId;
};

