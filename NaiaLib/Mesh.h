#pragma once
#include "Vertex.h"

class Mesh
{
public:
	struct SubMesh
	{
		SubMesh();
		~SubMesh();

		bool Init(const std::vector<VertexTexturedLit>& vertices, const std::vector<unsigned int>& Indices);
		void Destroy();

		GLuint VertexBuffer;
		GLuint IndexBuffer;
		int NumIndices;
	};

	Mesh();
	~Mesh();

public:
	void Render();
	void Destroy();

public:
	std::vector<SubMesh> SubMeshes;
};

