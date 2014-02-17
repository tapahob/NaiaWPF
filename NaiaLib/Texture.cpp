#include "precompiled.h"
#include "stb_image.h"
#include "Texture.h"
#include <fstream>


Texture::Texture() : Loaded(false), id(-1)
{
}


Texture::~Texture()
{
}

bool Texture::LoadTexture(string path)
{
	cerr << "\nLOADING TEXTURE: " << path.c_str() << endl;


	auto tName = System::Threading::Thread::CurrentThread->Name;
	auto a = std::string(reinterpret_cast<char*>(System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(tName).ToPointer()));
	
	cerr << "\n### Load texture in thread " << a << endl;

	/*static const GLubyte imgData [] =
	{
		0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00,
		0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
		0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00,
		0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
		0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00,
		0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
		0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00,
		0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF
	};*/

	int width = 0;
	int height = 0;
	int cpp = 0;

	auto b = path.c_str();

	std::ifstream stream(path.c_str());
	if (!stream.good())
		return false;

	stream.close();

	unsigned char* imgData = stbi_load(path.c_str(), &width, &height, &cpp, 0);
	glActiveTexture(GL_TEXTURE0);

	for (int i = 0; i < 9; i++)
	{
		cout << " #" << (int)imgData[i];
		if (i % 3 == 0)
			cout << endl;
	}

	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	
	if (cpp == 3)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imgData);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgData);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imgData);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 8, 8, 0, GL_RED, GL_UNSIGNED_BYTE, imgData);

	auto error = glGetError();

	if (!error)
		cerr << "TextureLoad success!\n";
	else
		switch (error)
	{
		case GL_INVALID_ENUM:
			cerr << "invalid enum" << endl;
			break;
		case GL_INVALID_VALUE:
			cerr << "invalid value" << endl;
			break;
		case GL_INVALID_OPERATION:
			cerr << "invalid enum" << endl;
			break;
		default:
			cerr << "another error" << endl;
			break;
	}

	bool wrap = true;

	if (wrap)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);*/
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	}

	// filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	

	//stbi_image_free(imgData);

	error = glGetError();

	if (error)
		cerr << "TextureParameter error!\n";

	Loaded = true;
	return true;
}

void Texture::Bind()
{
	auto tName = System::Threading::Thread::CurrentThread->Name;

	auto a = std::string(reinterpret_cast<char*>(System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(tName).ToPointer()));

	if (a.compare("RenderingThread") != 0)
		cerr << "\n### Bind texture in thread " << a << endl;

	auto error = glGetError();
	if (error)
		cerr << " texture bind ERROR!" << endl;

	glBindTexture(GL_TEXTURE_2D, textureId);
	error = glGetError();
	if (error)
		cerr << " texture bind ERROR!" << endl;
}
