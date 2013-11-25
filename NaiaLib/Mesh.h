#pragma once
#include "Vertex.h"

class Mesh
{
public:
	Mesh();
	~Mesh();

	void Render();

	struct SubMesh
	{
		SubMesh();
		~SubMesh();

		bool Init(const std::vector<VertexTexturedLit>& vertices, const std::vector<unsigned int>& Indices);

		GLuint VAO;
		GLuint VertexBuffer;
		GLuint IndexBuffer;
		int NumIndices;
	};


public:
	std::vector<SubMesh> SubMeshes;
};

