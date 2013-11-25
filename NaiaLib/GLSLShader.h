#pragma once

class GLSLShader
{
public:
	GLSLShader();
	~GLSLShader();

	void   LoadFromFile(GLenum whichShader, const string& fileName);
	void   LoadFromString(GLenum whichShader, const string& source);
	void   CreateAndLinkProgram();
	void   Use();
	void   UnUse();
	void   AddAttribute(const string& attribute);
	void   AddUniform(const string& uniform);
	GLuint operator[] (const string& attribute);
	GLuint operator() (const string& uniform);
	void   DeleteShaderProgram();

private:
	enum ShaderType { VERTEX_SHADER, FRAGMENT_SHADER, GEOMETRY_SHADER };
	GLuint program;
	int totalShaders;
	GLuint shaders[3];
	map<string, GLuint> attributeList;
	map<string, GLuint> uniformLocationList;
};

