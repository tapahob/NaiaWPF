#include "precompiled.h"
#include "Material.h"
#include "Scene.h"
#include "SceneNodes.h"
#include "OGLRenderer.h"
#include "MathHelper.h"
#include "Mesh.h"
#include "Vertex.h"
#include "EditorCore.h"
#include <assimp\cimport.h>
#include <assimp\postprocess.h>
#include "NaiaCore.h"

//------------------------------------------------------------------------
//	SceneNode
//------------------------------------------------------------------------
SceneNode::SceneNode(Scene* scene) : m_Scene(scene)
{
	m_Scene->idCounter++;

	Properties.Id                              = m_Scene->idCounter;
	Properties.Name                            = "Unknown Scene Node";
	m_Scene->SceneNodeList[m_Scene->idCounter] = this;
}

SceneNode::~SceneNode()
{
}

void SceneNode::Destroy()
{
	for (auto it = children.begin(); it != children.end(); it++)
		(*it)->Destroy();

	children.clear();
}

void SceneNode::Update(float deltaMs)
{
	for (unsigned int i = 0; i < children.size(); i++)
		children[i]->Update(deltaMs);
}

void SceneNode::Render()
{
	for (unsigned int i = 0; i < children.size(); i++)
	{
		auto kid = children[i];

		kid->PreRender();

		if (kid->IsVisible())
			kid->Render();

		kid->PostRender();
	}
}

void SceneNode::PreRender()
{
	m_Scene->PushAndSetMatrix(Properties.ToWorld);
}

void SceneNode::PostRender()
{
	m_Scene->PopMatrix();
}

bool SceneNode::IsVisible()
{
	return true;
}

void SceneNode::AddChild(SceneNode* kid)
{
	children.push_back(kid);
}

void SceneNode::RemoveChild(SceneNode* kid)
{
	for (auto i = children.begin(); i != children.end(); i++)
	{
		if ((*i)->Properties.Id == kid->Properties.Id)
		{
			children.erase(i);
			break;
		}
	}
}

//------------------------------------------------------------------------
//	CameraNode
//------------------------------------------------------------------------
CameraNode::CameraNode(Scene* scene)
: SceneNode(scene)
{
	Target = glm::vec3(0.0f, 0.0f, 1.0f);
}

void CameraNode::Render()
{
	glm::mat4x4 rotation = Properties.ToWorld;
	MathHelper::SetPosition(rotation, glm::vec3(0.0f));
	glm::vec3 up = glm::vec3(rotation * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	
	auto position = MathHelper::GetPosition(Properties.ToWorld);
	ViewMatrix = glm::lookAt(position, Target, up);

	auto renderer = NaiaLib::SceneWrapper::pScene->Renderer;
	if (NaiaLib::EditorCore::Instance->activeRenderer != renderer->id)
		return;
	
	renderer->MVP = ViewMatrix * renderer->WorldMatrix * renderer->ProjectionMatrix;

	SceneNode::Render();
}

//------------------------------------------------------------------------
//	MeshNode
//------------------------------------------------------------------------

MeshNode::MeshNode(Scene* scene, MeshType type = None)
: SceneNode(scene), RenderWindow(-1)
{
	if (type == None)
	{
		//LoadFromFile("C:\\test\\girl.3ds");
		LoadFromFile("C:\\Users\\tapahob\\Downloads\\IAx\\IAx\\IAx.pmx");
		return;
	}
	
	if (type == LineGridXZ || type == LineGridZY || type == LineGridXY)
	{
		mesh.Topology = Mesh::PrimitiveTopology::Lines;
		Properties.Name = "LineGrid";
		Mesh::SubMesh grid;

		unsigned int WIDTH = NaiaLib::SceneWrapper::pScene->Renderer->width * 3;
		unsigned int DEPTH = WIDTH;

		unsigned int m = WIDTH / 20;
		unsigned int n = WIDTH / 20;

		cout << "######### TYPE: " << type << "; ############ WIDTH: " << WIDTH << endl;

		std::vector<VertexTexturedLit> vertices;
		std::vector<unsigned int> indices;

		unsigned int vertexCount = m*n;

		float halfWidth = 0.5f * WIDTH;
		float halfDepth = 0.5f * DEPTH;
		float dx        = (float) WIDTH / (n - 1);
		float dz        = (float) DEPTH / (m - 1);
		float du        = 1.0f / (n - 1);
		float dv        = 1.0f / (m - 1);

		// generate vertices
		vertices.resize(vertexCount);
		for (unsigned int i = 0; i < m; ++i)
		{
			float z = halfDepth - i * dz;
			for (unsigned int j = 0; j < n; ++j)
			{
				float x = -halfWidth + j * dx;
				
				int index = i*n + j;
				
				switch (type)
				{
				case MeshNode::LineGridXZ:
					vertices[index].Position = glm::vec3(x, 0.0f, z);
					vertices[index].Normal = glm::vec3(0, 1.0f, 0);
					break;
				case MeshNode::LineGridXY:
					vertices[index].Position = glm::vec3(x, z, 0.0f);
					vertices[index].Normal = glm::vec3(0, 0, 1.0f);
					break;
				case MeshNode::LineGridZY:
					vertices[index].Position = glm::vec3(0.0f, x, z);
					vertices[index].Normal = glm::vec3(1.0f, 0, 0);
					break;
				}
				
				vertices[index].TexCoords = glm::vec2(-1.0f, -1.0f);
			}
		}

		// generate indices
		indices.resize((m * 2) + (n * 2));
		
		// rows
		for (unsigned int i = 1; i < m * 2; i += 2)
		{
			indices[i] = (i - 1) / 2 * n + n - 1;
			indices[i-1] = indices[i] - (n - 1);
		}

		// columns
		for (unsigned int i = 0; i < m * 2; i += 2)
			indices[i + m*2] = i / 2;
		
		int k = m*n - n;
		for (unsigned int i = 1; i < m * 2; i += 2)
			indices[i + m*2] = i/2 + k;

		grid.Init(vertices, indices);
		mesh.SubMeshes.push_back(grid);
		return;
	}

	if (type == TriangleGrid)
	{
		Properties.Name = "Grid";
		Mesh::SubMesh grid;
		
		const int WIDTH = 100;
		const int DEPTH = 100;

		int m = WIDTH;
		int n = DEPTH;
		
		std::vector<VertexTexturedLit> vertices;
		std::vector<unsigned int> indices;

		int vertexCount = m*n;
		int faceCount = (m - 1)*(n - 1) * 2;

		float halfWidth = 0.5f * WIDTH;
		float halfDepth = 0.5f * DEPTH;
		float dx = (float)WIDTH / (n - 1);
		float dz = (float)DEPTH / (m - 1);
		float du = 1.0f / (n - 1);
		float dv = 1.0f / (m - 1);

		// generate vertices
		vertices.resize(vertexCount);
		for (int i = 0; i < m; ++i)
		{
			float z = halfDepth - i * dz;
			for (int j = 0; j < n; ++j)
			{
				float x = -halfWidth + j * dz;
				
				vertices[i*n + j].Position  = glm::vec3(x, 0.0f, z);
				vertices[i*n + j].Normal    = glm::vec3(0, 1.0f, 0);
				vertices[i*n + j].TexCoords = glm::vec2(j*du, i*dv);
			}
		}
		
		// generate Indices
		indices.resize(faceCount * 3);
		int k = 0;
		for (int i = 0; i < m - 1; ++i)
		{
			for (int j = 0; j < n - 1; ++j)
			{
				indices[k	 ] = i * n + j;
				indices[k + 1] = i * n + j + 1;
				indices[k + 2] = (i + 1) * n + j;
				indices[k + 3] = (i + 1) * n + j;
				indices[k + 4] = i * n + j + 1;
				indices[k + 5] = (i + 1) * n + j + 1;
				k += 6;
			}
		}

		grid.Init(vertices, indices);
		mesh.SubMeshes.push_back(grid);
	}
}

void MeshNode::PreRender()
{
	SceneNode::PreRender();
}

void MeshNode::Render()
{
	mesh.Render();
	SceneNode::Render();
}

void MeshNode::PostRender()
{
	SceneNode::PostRender();
}

void MeshNode::Destroy()
{
	SceneNode::Destroy();
	mesh.Destroy();
}

bool MeshNode::IsVisible()
{
	if (RenderWindow == -1)
		return true;

	auto currentRenderer = NaiaLib::SceneWrapper::pScene->Renderer->id;
	return RenderWindow == currentRenderer;
}

bool MeshNode::LoadFromFile(std::string path)
{
	auto extension = path.substr(path.find_last_of('.') + 1);
	if (extension == "pmx" || extension == "PMX")
		return LoadPMX(path);

	const aiScene* scene = aiImportFile(path.c_str(), aiProcess_FlipUVs);
	if (!scene)
	{
		cerr << "ERROR : [CANNOT IMPORT MODEL]" << endl;
		return false;
	}

	this->mesh.Textures.resize(scene->mNumMaterials);
	this->mesh.Normals.resize(scene->mNumMaterials);
	for (UINT j = 0; j < scene->mNumMeshes; ++j)
	{
		aiMesh* mesh = scene->mMeshes[j];

		vector<VertexTexturedLit> vertices(mesh->mNumVertices);

		for (UINT i = 0; i < mesh->mNumVertices; ++i)
		{
			VertexTexturedLit vertex;
			vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
			vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
			vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
			vertices[i] = (vertex);
		}

		cout << "Indices" << endl;

		vector<unsigned int> indices;
		for (UINT i = 0; i < mesh->mNumFaces; i++)
		{
			indices.push_back(mesh->mFaces[i].mIndices[0]);
			indices.push_back(mesh->mFaces[i].mIndices[1]);
			indices.push_back(mesh->mFaces[i].mIndices[2]);

			//cout << mesh->mFaces[i].mIndices[0] << ":" << mesh->mFaces[i].mIndices[1] << ":" << mesh->mFaces[i].mIndices[2] << endl;
		}

		Mesh::SubMesh subMesh;
		subMesh.MaterialIndex = mesh->mMaterialIndex;
		subMesh.Init(vertices, indices);
		this->mesh.SubMeshes.push_back(subMesh);
	}

	for (UINT i = 0; i < scene->mNumMaterials; ++i)
	{
		const aiMaterial* material = scene->mMaterials[i];

		if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
		{
			aiString path;
			if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
			{
				auto a = path.data;
				//a[0] = '\\';
				std::string fullPath = "c:\\test\\" + std::string(path.data);
				this->mesh.Textures[i].LoadTexture(fullPath);
				cerr << "Loading texture: " << fullPath << endl;

				/*fullPath = "c:\\" + std::string(path.data) + "N.tga";
				this->mesh.Normals[i].LoadTexture(fullPath);
				cerr << "Loading normal map: " << fullPath << endl;*/
			}
		}

		/*if (material->GetTextureCount(aiTextureType_NORMALS) > 0)
		{
			aiString path;
			if (material->GetTexture(aiTextureType_NORMALS, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
			{
				auto a = path.data;
				a[0] = '\\';
				std::string fullPath = "c:" + std::string(path.data) + "N.tga";
				this->mesh.Normals[i].LoadTexture(fullPath);
				cerr << "Loading texture: " << fullPath << endl;
			}
		}*/
	}
	
	return true;
}

bool MeshNode::LoadPMX(std::string path)
{
	PMXLoader loader;
	PMXFormat format = loader.Read(path);

	UINT subMeshCount = format.material_list.size();
	UINT counter = 0;

	std::vector<VertexTexturedLit> vertices(format.vertex_list.size());
	for (UINT i = 0; i < format.vertex_list.size(); ++i)
		vertices[i] = VertexTexturedLit(format.vertex_list[i].Position, format.vertex_list[i].NormalVector, format.vertex_list[i].UV);

	GLuint VertexBuffer;

	glGenBuffers(1, &VertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexTexturedLit) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	if (glGetError())
		cerr << "ERROR!" << endl;

	mesh.Textures.resize(format.texture_list.size());
	for (UINT i = 0; i < format.texture_list.size(); ++i)
	{
		std::string textureName;
		for (char x : format.texture_list[i])
			textureName += x;

		std::string fullPath = format.meta_header.Folder + '\\' + textureName;

		mesh.Textures[i].LoadTexture(fullPath);
	}

	for (UINT i = 0; i < subMeshCount; ++i)
	{
		auto currentMaterial = format.material_list[i];
		UINT count = currentMaterial.FaceVertexCount;

		

		std::vector<UINT> indices;
		indices.resize(count);
		for (UINT j = 0; j < count; ++j)
			indices[j] = format.face_vertex_list[j + counter];

		Mesh::SubMesh subMesh;
		subMesh.InitPMX(VertexBuffer, indices);

		Material material(currentMaterial.AmbientColor, currentMaterial.DiffuseColor, currentMaterial.SpecularColor, Color4(0.0f));
		material.Specular.a = currentMaterial.Specularity;

		subMesh.material = material;

		UINT index = format.material_list[i].UsuallyTextureIndex > format.texture_list.size() ? 0 : format.material_list[i].UsuallyTextureIndex;
		subMesh.ToonIndex = format.material_list[i].ToonTextureIndex > format.texture_list.size() ? -1 : format.material_list[i].ToonTextureIndex;

		subMesh.MaterialIndex = index;
		mesh.SubMeshes.push_back(subMesh);
		counter += count;
	}

	if (glGetError())
		cerr << "ERROR!" << endl;
	return true;
}

