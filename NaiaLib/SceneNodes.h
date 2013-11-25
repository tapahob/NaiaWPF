#pragma once
#include "Material.h"
#include "Mesh.h"

struct SceneNodeProperties
{
	SceneNodeProperties()
	{
		ActorId = -1;
		ToWorld = glm::mat4x4(1.0f);
	}

	string      Name;
	int			Id;
	int         ActorId;

	glm::mat4x4 ToWorld;
	glm::mat4x4 FromWorld;

	bool		HasAlpha;
	Material	Material;
};

class Scene;

//------------------------------------------------------------------------
//	SceneNode
//------------------------------------------------------------------------
class SceneNode
{
public:
	SceneNode(){}
	SceneNode(Scene* scene);
	virtual ~SceneNode();

public:
	virtual void Update(float deltaMs);
	virtual void PreRender();
	virtual void Render();
	virtual void PostRender();
	virtual bool IsVisible();
	virtual void AddChild(SceneNode* kid);
	virtual void RemoveChild(SceneNode* kid);
	virtual void Destroy();

public:
	SceneNodeProperties Properties;

protected:
	Scene* m_Scene;
	vector<SceneNode*> children;
};

//------------------------------------------------------------------------
//	CameraNode
//------------------------------------------------------------------------
class CameraNode : public SceneNode
{
public:
	CameraNode(Scene* scene);

	virtual void Render();

private:
	glm::vec3 Position();

public:
	glm::vec3 Target;
	glm::mat4x4 ViewMatrix;

private:
	glm::vec3 LookAt;
};

//------------------------------------------------------------------------
//	MeshNode
//------------------------------------------------------------------------
class MeshNode : public SceneNode
{
public:
	MeshNode(Scene* scene);

	virtual void PreRender();
	virtual void Render();
	virtual void PostRender();
	virtual void Destroy();
private:
	Mesh mesh;
};