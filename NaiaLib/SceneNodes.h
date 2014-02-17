#pragma once
#include "Material.h"
#include "Mesh.h"
#include "PMXLoader.h"

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
	vector<SceneNode*> children;

protected:
	Scene* m_Scene;
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
	enum MeshType 
	{ 
		None, 
		TriangleGrid, 
		LineGridXZ,
		LineGridXY,
		LineGridZY
	};
	
	MeshNode(Scene* scene, MeshType type);

	virtual void PreRender();
	virtual void Render();
	virtual void PostRender();
	virtual void Destroy();
	virtual bool IsVisible();

	bool LoadFromFile(std::string path);
private:
	bool LoadPMX(std::string path);

public:
	int RenderWindow;

private:
	Mesh mesh;
};