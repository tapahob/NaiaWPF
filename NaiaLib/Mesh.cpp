#include "precompiled.h"
#include "Vertex.h"
#include "Mesh.h"
#include "NaiaCore.h"

extern HGLRC g_GLMainContext;

//------------------------------------------------------------------------
//	Mesh
//------------------------------------------------------------------------
Mesh::Mesh()
{
}


Mesh::~Mesh()
{
}

void Mesh::Render()
{
	glEnableVertexAttribArray(0);

	for (unsigned int i = 0; i < SubMeshes.size(); i++)
	{
		
		glBindBuffer(GL_ARRAY_BUFFER, SubMeshes[i].VertexBuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexTexturedLit), 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, SubMeshes[i].IndexBuffer);
		
		glDrawElements(GL_TRIANGLES, SubMeshes[i].NumIndices, GL_UNSIGNED_INT, 0);
	}

	glDisableVertexAttribArray(0);
}

void Mesh::Destroy()
{
	glBindBuffer(GL_ARRAY_BUFFER,         NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);

	for (auto it = SubMeshes.begin(); it != SubMeshes.end(); it++)
		it->Destroy();

	SubMeshes.clear();
}

//------------------------------------------------------------------------
//	SubMesh
//------------------------------------------------------------------------
bool Mesh::SubMesh::Init(const std::vector<VertexTexturedLit>& vertices, const std::vector<unsigned int>& indices)
{	
	auto hdc = NaiaCore::Instance()->Windows[0]->Hdc;
	wglMakeCurrent(hdc, g_GLMainContext);

	NumIndices = indices.size();
	glGenBuffers(1, &VertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexTexturedLit) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &IndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);

	return true;
}

Mesh::SubMesh::SubMesh()
{

}

Mesh::SubMesh::~SubMesh()
{

}

void Mesh::SubMesh::Destroy()
{
	glDeleteBuffers(1, &VertexBuffer);
	glDeleteBuffers(1, &IndexBuffer);
}
