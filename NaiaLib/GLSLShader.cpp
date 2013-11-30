#include "precompiled.h"
#include "GLSLShader.h"
#include "NaiaCore.h"
GLSLShader::GLSLShader()
{
	totalShaders             = 0;
	shaders[VERTEX_SHADER]   = 0;
	shaders[FRAGMENT_SHADER] = 0;
	shaders[GEOMETRY_SHADER] = 0;
	attributeList.clear();
	uniformLocationList.clear();
}


GLSLShader::~GLSLShader()
{
	attributeList.clear();
	uniformLocationList.clear();
}

void GLSLShader::DeleteShaderProgram()
{
	glDeleteProgram(program);
}

void GLSLShader::LoadFromString(GLenum whichShader, const string& source)
{
	GLuint shader = glCreateShader(whichShader);
	
	const char* sources = source.c_str();
	glShaderSource(shader, 1, &sources, NULL);

	GLint status;
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar *infoLog = new GLchar[infoLogLength];
		glGetShaderInfoLog(shader, infoLogLength, NULL, infoLog);
		OutputDebugStringA(infoLog);
		delete [] infoLog;
	}
	shaders[totalShaders++] = shader;
}

void GLSLShader::CreateAndLinkProgram()
{
	program = glCreateProgram();
	if (shaders[VERTEX_SHADER] != 0)
		glAttachShader(program, shaders[VERTEX_SHADER]);
	if (shaders[FRAGMENT_SHADER] != 0)
		glAttachShader(program, shaders[FRAGMENT_SHADER]);
	if (shaders[GEOMETRY_SHADER] != 0)
		glAttachShader(program, shaders[GEOMETRY_SHADER]);

	GLint status;
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar *infoLog = new GLchar[infoLogLength];
		glGetProgramInfoLog(program, infoLogLength, NULL, infoLog);
		OutputDebugStringA(infoLog);
		delete [] infoLog;
	}

	glDeleteShader(shaders[VERTEX_SHADER]);
	glDeleteShader(shaders[FRAGMENT_SHADER]);
	glDeleteShader(shaders[GEOMETRY_SHADER]);
}

void GLSLShader::Use()
{
	glUseProgram(program);
}

void GLSLShader::UnUse()
{
	glUseProgram(0);
}

void GLSLShader::AddAttribute(const string& attribute)
{
	attributeList[attribute] = glGetAttribLocation(program, attribute.c_str());
}

void GLSLShader::AddUniform(const string& uniform)
{
	uniformLocationList[uniform] = glGetUniformLocation(program, uniform.c_str());
}

GLuint GLSLShader::operator[](const string& attribute)
{
	return attributeList[attribute];
}

GLuint GLSLShader::operator()(const string& uniform)
{
	return uniformLocationList[uniform];
}

#include <fstream>
void GLSLShader::LoadFromFile(GLenum whichShader, const string& fileName)
{
	ifstream fp;
	fp.open(fileName.c_str(), ios_base::in);
	if (fp)
	{
		string line, buffer;
		while (getline(fp, line))
		{
			buffer.append(line);
			buffer.append("\r\n");
		}
		LoadFromString(whichShader, buffer);
	}
	else
		OutputDebugStringA("ERROR: cannot load shader!");
}