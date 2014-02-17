#pragma once
#include "precompiled.h"
#include "Vertex.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include "Texture.h"
#include "Material.h"
class Mesh
{
public:
	struct SubMesh
	{
		SubMesh();
		~SubMesh();

		bool Init(const std::vector<VertexTexturedLit>& vertices, const std::vector<unsigned int>& Indices);
		bool InitPMX(const GLuint VertexBuffer, const std::vector<unsigned int>& Indices);
		void Destroy();

		GLuint   VertexBuffer;
		GLuint   IndexBuffer;
		int      NumIndices;
		int		 ToonIndex;
		int      MaterialIndex;
		Material material;
	};

	enum PrimitiveTopology { Triangles, Lines };

	Mesh();
	~Mesh();

public:
	void Render();
	void Destroy();

public:
	std::vector<Texture> Textures;
	std::vector<Texture> Normals;
	PrimitiveTopology Topology;
	std::vector<SubMesh> SubMeshes;
};

