#pragma once
#include "PMXFormat.h"
#include <iostream>
#include <fstream>

class PMXLoader
{
public:
	PMXLoader();
	~PMXLoader();

	PMXFormat Read(const std::string& pmxFilePath);

private:
	PMXFormat::Header                 GetHeader();
	void                              ReadString(std::wstring& str);
	PMXFormat::MetaHeader             GetMetaHeader();
	PMXFormat::Vertex				  ReadVertex();
	PMXFormat::VertexList			  GetVertexList();
	glm::vec4                         ReadSinglesToVector4();
	glm::vec3                         ReadSinglesToVector3();
	glm::vec2                         ReadSinglesToVector2();
	PMXFormat::BDEF1                  ReadBoneWeightBDEF1();
	PMXFormat::BDEF2                  ReadBoneWeightBDEF2();
	PMXFormat::BDEF4                  ReadBoneWeightBDEF4();
	PMXFormat::SDEF					  ReadBoneWeightSDEF();
	PMXFormat::QDEF					  ReadBoneWeightQDEF();
	UINT							  CastIntRead(const PMXFormat::Header::IndexSize indexSize);
	PMXFormat::FaceVertexList		  ReadFaceVertexList();
	PMXFormat::TextureList			  ReadTextureList();
	PMXFormat::MaterialList			  ReadMaterialList();
	PMXFormat::Material				  ReadMaterial();
	PMXFormat::BoneList				  ReadBoneList();
	PMXFormat::Bone					  ReadBone();
	PMXFormat::IKData				  ReadIKData();
	PMXFormat::IKLink				  ReadIKLink();
	PMXFormat::MorphList			  ReadMorphList();
	PMXFormat::MorphData			  ReadMorphData();
	PMXFormat::MorphOffset			  ReadGroupMorphOffset();
	PMXFormat::MorphOffset			  ReadVertexMorphOffset();
	PMXFormat::MorphOffset			  ReadBoneMorphOffset();
	PMXFormat::MorphOffset			  ReadUVMorphOffset();
	PMXFormat::MorphOffset			  ReadMaterialMorphOffset();
	PMXFormat::MorphOffset			  ReadImpulseMorphOffset();
	PMXFormat::DisplayFrameList		  ReadDisplayFrameList();
	PMXFormat::DisplayFrame			  ReadDisplayFrame();
	PMXFormat::DisplayElement		  ReadDisplayElement();
	PMXFormat::RigidbodyList		  ReadRigidBodyList();
	PMXFormat::RigidBody			  ReadRigidBody();
	PMXFormat::RigidbodyJointList	  ReadRigidBodyJointList();
	PMXFormat::Joint				  ReadJoint();

private:
	std::ifstream                     pmxFile;
	PMXFormat::Header::StringEncoding encoding;
	PMXFormat						  format;
};

