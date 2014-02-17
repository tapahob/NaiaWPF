#include "precompiled.h"
#include "Vertex.h"
#include "Mesh.h"
#include "NaiaCore.h"
#include "SceneWrapper.h"

extern HGLRC g_GLMainContext;

//------------------------------------------------------------------------
//	Mesh
//------------------------------------------------------------------------
Mesh::Mesh()
{
	Topology = PrimitiveTopology::Triangles;
	glActiveTexture(GL_TEXTURE0);

	auto error = glGetError();
	if (error)
		cerr << " texture bind ERROR!" << endl;

	auto shader = &(NaiaCore::Instance()->Shaders["ColorShader"]);
	shader->AddUniform("gSampler");
	shader->AddUniform("gNormalMap");
	shader->AddUniform("gAmbient");
	shader->AddUniform("gDiffuse");
	shader->AddUniform("gSpecular");
	shader->AddUniform("MV");
	error = glGetError();
	if (error)
		cerr << " texture bind ERROR!" << endl;

	shader->Use();

	auto gSampler = (*shader)("gSampler");
	glUniform1i(gSampler, 0);

	auto gNormalMap = (*shader)("gNormalMap");
	glUniform1i(gNormalMap, 1);

	error = glGetError();
	if (error)
	{
		switch (error)
		{
		case GL_INVALID_VALUE:
			cerr << " texture bind ERROR!" << endl;
			break;

		case GL_INVALID_OPERATION:
			cerr << " texture bind ERROR!" << endl;
			break;
		}
	}
}


Mesh::~Mesh()
{
}

void Mesh::Render()
{
	auto renderer = NaiaLib::SceneWrapper::pScene->Renderer;
	auto camera   = renderer->Camera;
	auto MVP      = renderer->ProjectionMatrix * camera->ViewMatrix * renderer->WorldMatrix;
	auto MV		  = glm::inverse( glm::transpose(camera->ViewMatrix * renderer->WorldMatrix));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	auto error = glGetError();
	if (error)
		cerr << " texture bind ERROR!" << endl;

	glUniformMatrix4fv(NaiaCore::Instance()->Shaders["ColorShader"]("MVP"), 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(NaiaCore::Instance()->Shaders["ColorShader"]("MV"), 1, GL_FALSE, &MV[0][0]);
	
	error = glGetError();
	if (error)
		cerr << " texture bind ERROR!" << endl;

	for (unsigned int i = 0; i < SubMeshes.size(); i++)
	{
		auto materialIndex = SubMeshes[i].MaterialIndex;
		if (materialIndex != -1)
		{
			glActiveTexture(GL_TEXTURE0);
			auto texture = Textures[materialIndex];
			if (texture.Loaded)
				texture.Bind();

			glActiveTexture(GL_TEXTURE0 + 1);
			if (Normals.size() >= materialIndex && Normals.size() > 0)
			{
				auto normal = Normals[materialIndex];
				if (normal.Loaded)
					normal.Bind();
			}

			UINT toonIndex = SubMeshes[i].ToonIndex;
			if (toonIndex > Textures.size())
				toonIndex = -1;
			if (toonIndex != -1 && Textures[SubMeshes[i].ToonIndex].Loaded)
				Textures[SubMeshes[i].ToonIndex].Bind();
	
			auto shader = &(NaiaCore::Instance()->Shaders["ColorShader"]);
			shader->Use();
			glUniform4fv((*shader)("gAmbient"), 1, &SubMeshes[i].material.Ambient[0]);
			glUniform4fv((*shader)("gDiffuse"), 1, &SubMeshes[i].material.Diffuse[0]);
			glUniform4fv((*shader)("gSpecular"), 1, &SubMeshes[i].material.Specular[0]);
			
			error = glGetError();
			if (error)
				cerr << " texture bind ERROR!" << endl;
		}
		glBindBuffer(GL_ARRAY_BUFFER, SubMeshes[i].VertexBuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexTexturedLit), 0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexTexturedLit), (const GLvoid*)12);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexTexturedLit), (const GLvoid*) 20);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, SubMeshes[i].IndexBuffer);
		
		auto topology = Topology == PrimitiveTopology::Lines ? GL_LINES : GL_TRIANGLES;

		if (renderer->State.Wireframe && Topology != PrimitiveTopology::Lines)
			topology = GL_POINTS;

		glDrawElements(topology, SubMeshes[i].NumIndices, GL_UNSIGNED_INT, 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
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
	auto tName = System::Threading::Thread::CurrentThread->Name;

	auto a = reinterpret_cast<char*>(System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(tName).ToPointer());
	
	cerr << "\n### Initializing Mesh in thread " << a << endl;

	auto hdc = NaiaCore::Instance()->Windows[0]->Hdc;
	wglMakeCurrent(hdc, g_GLMainContext);

	ToonIndex = -1;

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
	MaterialIndex = -1;
}

Mesh::SubMesh::~SubMesh()
{

}

void Mesh::SubMesh::Destroy()
{
	glDeleteBuffers(1, &VertexBuffer);
	glDeleteBuffers(1, &IndexBuffer);
}

bool Mesh::SubMesh::InitPMX(const GLuint VertexBuffer, const std::vector<unsigned int>& Indices)
{
	auto hdc = NaiaCore::Instance()->Windows[0]->Hdc;
	wglMakeCurrent(hdc, g_GLMainContext);

	NumIndices = Indices.size();

	this->VertexBuffer = VertexBuffer;

	glGenBuffers(1, &IndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * Indices.size(), &Indices[0], GL_STATIC_DRAW);
	return true;
}


