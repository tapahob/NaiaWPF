#include "precompiled.h"
#include "Material.h"
#include "Scene.h"
#include "SceneNodes.h"
#include "OGLRenderer.h"
#include "MathHelper.h"
#include "Mesh.h"
#include "Vertex.h"

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
	Target = glm::vec3(1.0f);
}

void CameraNode::Render()
{
	glm::mat4x4 rotation = Properties.ToWorld;
	MathHelper::SetPosition(rotation, glm::vec3(0.0f));
	glm::vec3 up = glm::vec3(rotation * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	auto position = MathHelper::GetPosition(Properties.ToWorld);
	ViewMatrix = glm::lookAt(position, Target, up);

	auto renderer = NaiaLib::SceneWrapper::pScene->Renderer;
	renderer->MVP = renderer->WorldMatrix * ViewMatrix * renderer->ProjectionMatrix;

	SceneNode::Render();
}

//------------------------------------------------------------------------
//	MeshNode
//------------------------------------------------------------------------

MeshNode::MeshNode(Scene* scene)
: SceneNode(scene)
{
	Properties.Name = "Unknown Mesh Node";

	Mesh::SubMesh triangle;

	std::vector<VertexTexturedLit> vertices =
	{
		VertexTexturedLit(glm::vec3(1.0f), glm::vec3(0.0f), glm::vec2(1.0f)),
		VertexTexturedLit(glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(0.0f), glm::vec2(1.0f)),
		VertexTexturedLit(glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec3(0.0f), glm::vec2(1.0f)),
	};

	vector<unsigned int> indices = {0, 1, 2};

	triangle.Init(vertices, indices);
	mesh.SubMeshes.push_back(triangle);
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

