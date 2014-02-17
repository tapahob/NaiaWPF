#include "PMXLoader.h"
#include <limits>
#include "VMDLoader.h"

PMXLoader::PMXLoader()
{
}


PMXLoader::~PMXLoader()
{
	pmxFile.close();
}

PMXFormat::Header PMXLoader::GetHeader()
{
	PMXFormat::Header result;
	if (!pmxFile.is_open())
		return result;
	
	BYTE indexSize;

	pmxFile.read(result.Magic, sizeof(float));
	pmxFile.read((char*) &result.Version, sizeof(float));
	pmxFile.read((char*) &indexSize, sizeof(BYTE));
	pmxFile.read((char*) &indexSize, sizeof(BYTE));
	result.EncodingMethod = (PMXFormat::Header::StringEncoding)indexSize;
	pmxFile.read((char*) &result.AdditionalUV, sizeof(BYTE));
	pmxFile.read((char*) &indexSize, sizeof(BYTE));
	result.VertexIndexSize = (PMXFormat::Header::IndexSize)indexSize;
	pmxFile.read((char*) &indexSize, sizeof(BYTE));
	result.TextureIndexSize = (PMXFormat::Header::IndexSize)indexSize;
	pmxFile.read((char*) &indexSize, sizeof(BYTE));
	result.MaterialIndexSize = (PMXFormat::Header::IndexSize)indexSize;
	pmxFile.read((char*) &indexSize, sizeof(BYTE));
	result.BoneIndexSize = (PMXFormat::Header::IndexSize)indexSize;
	pmxFile.read((char*) &indexSize, sizeof(BYTE));
	result.MorphIndexSize = (PMXFormat::Header::IndexSize)indexSize;
	pmxFile.read((char*) &indexSize, sizeof(BYTE));
	result.RigidBodyIndexSize = (PMXFormat::Header::IndexSize)indexSize;

	encoding = result.EncodingMethod;

	ReadString(result.ModelName);
	ReadString(result.ModelNameEnglish);
	ReadString(result.Comment);
	ReadString(result.CommentEnglish);

	return result;
}

void PMXLoader::ReadString(std::wstring& string)
{
	int length;
	pmxFile.read((char*) &length, sizeof(int));

	auto data = new char[length];
	pmxFile.read((char*) data, sizeof(BYTE) * length);
	string = std::wstring(reinterpret_cast<wchar_t*>(data), length / sizeof(wchar_t));
	delete [] data;
}

PMXFormat PMXLoader::Read(const std::string& pmxFilePath)
{
	pmxFile = std::ifstream(pmxFilePath.c_str(), std::ios::binary);
	if (!pmxFile.is_open())
		std::cerr << "Unable to open file" << std::endl;

	PMXFormat::MetaHeader metaHeader;
	metaHeader.Path   = pmxFilePath;
	metaHeader.Name   = metaHeader.Path.substr(metaHeader.Path.find_last_of('\\') + 1);
	metaHeader.Folder = metaHeader.Path.substr(0, metaHeader.Path.find_last_of('\\'));

	format.meta_header          = metaHeader;
	format.header               = GetHeader();
	format.vertex_list          = GetVertexList();
	format.face_vertex_list     = ReadFaceVertexList();
	format.texture_list         = ReadTextureList();
	format.material_list	    = ReadMaterialList();
	format.bone_list            = ReadBoneList();
	format.morph_list           = ReadMorphList();
	format.display_frame_list   = ReadDisplayFrameList();
	format.rigidbody_list	    = ReadRigidBodyList();
	format.rigidbody_joint_list = ReadRigidBodyJointList();


	auto loader = VMDLoader();

	const std::wstring vmdfile = L"D:\\Dev\\vmd\\h_IA_X.vmd";

	auto b = loader.Read(vmdfile);

	return format;
}

PMXFormat::VertexList PMXLoader::GetVertexList()
{
	PMXFormat::VertexList result;
	UINT vertexCount;
	pmxFile.read((char*) &vertexCount, sizeof(int));
	result.resize(vertexCount);
	for (UINT i = 0; i < vertexCount; ++i)
		result[i] = ReadVertex();
	return result;
}

PMXFormat::Vertex PMXLoader::ReadVertex()
{
	PMXFormat::Vertex result;
	result.Position = ReadSinglesToVector3();
	result.NormalVector = ReadSinglesToVector3();
	result.UV = ReadSinglesToVector2();
	result.AdditionalUV.resize(format.header.AdditionalUV);
	for (int i = 0; i < format.header.AdditionalUV; i++)
		result.AdditionalUV[i] = ReadSinglesToVector4();
		
	BYTE b;
	pmxFile.read((char*) &b, sizeof(BYTE));
	PMXFormat::WeightMethod weightMethod = (PMXFormat::WeightMethod)b;
	switch (weightMethod)
	{
	case PMXFormat::eBDEF1:
		result.Boneweight = ReadBoneWeightBDEF1();
		break;
	case PMXFormat::eBDEF2:
		result.Boneweight = ReadBoneWeightBDEF2();
		break;
	case PMXFormat::eBDEF4:
		result.Boneweight = ReadBoneWeightBDEF4();
		break;
	case PMXFormat::eSDEF:
		result.Boneweight = ReadBoneWeightSDEF();
		break;
	case PMXFormat::eQDEF:
		result.Boneweight = ReadBoneWeightQDEF();
		break;
	}
	
	pmxFile.read((char*) &result.edgeMagnification, sizeof(float));
	return result;
}

glm::vec2 PMXLoader::ReadSinglesToVector2()
{
	float result[2];
	for (int i = 0; i < 2; ++i)
		pmxFile.read((char*) &result[i], sizeof(float));
	return glm::vec2(result[0], result[1]);
}

glm::vec3 PMXLoader::ReadSinglesToVector3()
{
	float result[3];
	for (int i = 0; i < 3; ++i)
		pmxFile.read((char*) &result[i], sizeof(float));
	return glm::vec3(result[0], result[1], result[2]);
}
glm::vec4 PMXLoader::ReadSinglesToVector4()
{
	float result[4];
	for (int i = 0; i < 4; ++i)
		pmxFile.read((char*) &result[i], sizeof(float));
	return glm::vec4(result[0], result[1], result[2], result[3]);
}

UINT PMXLoader::CastIntRead(const PMXFormat::Header::IndexSize indexSize)
{
	UINT result = 0;
	auto maxByte = std::numeric_limits<BYTE>::max();
	auto maxUInt = std::numeric_limits<UINT>::max();
	auto maxUShort = std::numeric_limits<USHORT>::max();

	switch (indexSize)
	{
	case PMXFormat::Header::IndexSize::Byte1:
		pmxFile.read((char*) &result, sizeof(BYTE));
		if (result == maxByte)
			result = maxUInt;
		break;
	case PMXFormat::Header::IndexSize::Byte2:
		pmxFile.read((char*) &result, sizeof(UINT16));
		if (result == maxUShort)
			result = maxUInt;
		break;
	case PMXFormat::Header::IndexSize::Byte4:
		pmxFile.read((char*) &result, sizeof(UINT32));
		break;
	}

	return result;
}

PMXFormat::BDEF1 PMXLoader::ReadBoneWeightBDEF1()
{
	PMXFormat::BDEF1 result;
	result.bone1_value = CastIntRead(format.header.BoneIndexSize);
	return result;
}

PMXFormat::BDEF2 PMXLoader::ReadBoneWeightBDEF2()
{
	PMXFormat::BDEF2 result;
	result.bone1_value = CastIntRead(format.header.BoneIndexSize);
	result.bone2_value = CastIntRead(format.header.BoneIndexSize);
	pmxFile.read((char*) &result.bone1_weight, sizeof(float));
	return result;
}

PMXFormat::BDEF4 PMXLoader::ReadBoneWeightBDEF4()
{
	PMXFormat::BDEF4 result;
	result.bone1_value = CastIntRead(format.header.BoneIndexSize);
	result.bone2_value = CastIntRead(format.header.BoneIndexSize);
	result.bone3_value = CastIntRead(format.header.BoneIndexSize);
	result.bone4_value = CastIntRead(format.header.BoneIndexSize);
	pmxFile.read((char*) &result.bone1_weight, sizeof(float));
	pmxFile.read((char*) &result.bone2_weight, sizeof(float));
	pmxFile.read((char*) &result.bone3_weight, sizeof(float));
	pmxFile.read((char*) &result.bone4_weight, sizeof(float));
	return result;
}

PMXFormat::SDEF PMXLoader::ReadBoneWeightSDEF()
{
	PMXFormat::SDEF result;
	result.bone1_value = CastIntRead(format.header.BoneIndexSize);
	result.bone2_value = CastIntRead(format.header.BoneIndexSize);
	pmxFile.read((char*) &result.bone1_weight, sizeof(float));
	result.c = ReadSinglesToVector3();
	result.r0 = ReadSinglesToVector3();
	result.r1 = ReadSinglesToVector3();
	return result;
}

PMXFormat::QDEF PMXLoader::ReadBoneWeightQDEF()
{
	PMXFormat::QDEF result;
	result.bone1_value = CastIntRead(format.header.BoneIndexSize);
	result.bone2_value = CastIntRead(format.header.BoneIndexSize);
	result.bone3_value = CastIntRead(format.header.BoneIndexSize);
	result.bone4_value = CastIntRead(format.header.BoneIndexSize);
	pmxFile.read((char*) &result.bone1_weight, sizeof(float));
	pmxFile.read((char*) &result.bone2_weight, sizeof(float));
	pmxFile.read((char*) &result.bone3_weight, sizeof(float));
	pmxFile.read((char*) &result.bone4_weight, sizeof(float));
	return result;
}

PMXFormat::FaceVertexList PMXLoader::ReadFaceVertexList()
{
	PMXFormat::FaceVertexList result;
	UINT faceVertexCount = 0;
	pmxFile.read((char*) &faceVertexCount, sizeof(UINT32));
	result.resize(faceVertexCount);
	for (UINT i = 0; i < faceVertexCount; ++i)
		result[i] = CastIntRead(format.header.VertexIndexSize);
	return result;
}

PMXFormat::TextureList PMXLoader::ReadTextureList()
{
	PMXFormat::TextureList result;
	UINT textureFileCount;
	pmxFile.read((char*) &textureFileCount, sizeof(UINT32));
	result.resize(textureFileCount);
	for (UINT i = 0; i < textureFileCount; ++i)
		ReadString(result[i]);
	return result;
}

PMXFormat::MaterialList PMXLoader::ReadMaterialList()
{
	PMXFormat::MaterialList result;
	UINT materialCount;
	pmxFile.read((char*) &materialCount, sizeof(UINT32));
	result.resize(materialCount);
	for (UINT i = 0; i < materialCount; ++i)
		result[i] = ReadMaterial();
	return result;
}

PMXFormat::Material PMXLoader::ReadMaterial()
{
	PMXFormat::Material result;
	ReadString(result.Name);
	ReadString(result.NameEnglish);
	result.DiffuseColor = ReadSinglesToVector4();
	result.SpecularColor = glm::vec4(ReadSinglesToVector3(), 1.0f);
	pmxFile.read((char*) &result.Specularity, sizeof(float));
	result.AmbientColor = glm::vec4(ReadSinglesToVector3(), 1.0f);
	byte flag;
	pmxFile.read((char*) &flag, sizeof(BYTE));
	result.flag = (PMXFormat::Material::Flag)flag;
	result.EdgeColor = ReadSinglesToVector4();
	pmxFile.read((char*) &result.EdgeSize, sizeof(float));
	result.UsuallyTextureIndex = CastIntRead(format.header.TextureIndexSize);
	result.SphereTextureIndex = CastIntRead(format.header.TextureIndexSize);
	byte sphereMode;
	pmxFile.read((char*) &sphereMode, sizeof(BYTE));
	result.Sphere_Mode = (PMXFormat::Material::SphereMode)sphereMode;
	pmxFile.read((char*) &result.CommonToon, sizeof(BYTE));
	PMXFormat::Header::IndexSize textureIndexSize = (result.CommonToon == 0)
		? format.header.TextureIndexSize
		: PMXFormat::Header::IndexSize::Byte1;
	result.ToonTextureIndex = CastIntRead(textureIndexSize);
	ReadString(result.Memo);
	pmxFile.read((char*) &result.FaceVertexCount, sizeof(UINT32));
	return result;
}

PMXFormat::BoneList PMXLoader::ReadBoneList()
{
	PMXFormat::BoneList result;
	UINT boneCount;
	pmxFile.read((char*) &boneCount, sizeof(UINT32));
	result.resize(boneCount);
	for (UINT i = 0; i < boneCount; ++i)
		result[i] = ReadBone();
	return result;
}

PMXFormat::Bone PMXLoader::ReadBone()
{
	PMXFormat::Bone result;
	ReadString(result.BoneName);
	ReadString(result.BoneNameEnglish);
	result.BonePosition = ReadSinglesToVector3();
	result.ParentBoneIndex = CastIntRead(format.header.BoneIndexSize);
	pmxFile.read((char*) &result.TransformLevel, sizeof(INT32));
	USHORT flag;
	pmxFile.read((char*) &flag, sizeof(USHORT));
	result.BoneFlag = (PMXFormat::Bone::Flag)flag;
	if ((result.BoneFlag & PMXFormat::Bone::Flag::Connection) == 0)
		result.PositionOffset = ReadSinglesToVector3();
	else
		result.ConnectionIndex = CastIntRead(format.header.BoneIndexSize);

	if ((result.BoneFlag & (PMXFormat::Bone::Flag::AddRotation | PMXFormat::Bone::Flag::AddMove)) != 0)
	{
		result.AdditionalParentIndex = CastIntRead(format.header.BoneIndexSize);
		pmxFile.read((char*) &result.AdditionalRate, sizeof(float));
	}
	
	if ((result.BoneFlag & PMXFormat::Bone::Flag::FixedAxis) != 0)
	{
		result.AxisVector = ReadSinglesToVector3();
	}

	if ((result.BoneFlag & PMXFormat::Bone::Flag::LocalAxis) != 0)
	{
		result.xAxisVector = ReadSinglesToVector3();
		result.zAxisVector = ReadSinglesToVector3();
	}
	if ((result.BoneFlag & PMXFormat::Bone::Flag::ExternalParentTransform) != 0)
	{
		pmxFile.read((char*) &result.KeyValue, sizeof(UINT32));
	}
	if ((result.BoneFlag & PMXFormat::Bone::Flag::IkFlag) != 0)
		result.IkData = ReadIKData();

	return result;
}

PMXFormat::IKData PMXLoader::ReadIKData()
{
	PMXFormat::IKData result;
	result.IKBoneIndex = CastIntRead(format.header.BoneIndexSize);
	pmxFile.read((char*) &result.Iterations, sizeof(UINT32));
	pmxFile.read((char*) &result.LimitAngle, sizeof(float));
	UINT IKLinkCount;
	pmxFile.read((char*) &IKLinkCount, sizeof(UINT32));
	result.IKLinks.resize(IKLinkCount);
	for (UINT i = 0; i < IKLinkCount; ++i)
		result.IKLinks[i] = ReadIKLink();
	return result;
}

PMXFormat::IKLink PMXLoader::ReadIKLink()
{
	PMXFormat::IKLink result;
	result.TargetBoneIndex = CastIntRead(format.header.BoneIndexSize);
	pmxFile.read((char*) &result.AngleLimit, sizeof(BYTE));
	if (result.AngleLimit == 1)
	{
		result.LowerLimit = ReadSinglesToVector3();
		result.UpperLimit = ReadSinglesToVector3();
	}

	return result;
}

PMXFormat::MorphList PMXLoader::ReadMorphList()
{
	PMXFormat::MorphList result;
	UINT morphCount;
	pmxFile.read((char*) &morphCount, sizeof(UINT32));
	result.resize(morphCount);
	for (UINT i = 0; i < morphCount; ++i)
		result[i] = ReadMorphData();
	return result;
}

PMXFormat::MorphData PMXLoader::ReadMorphData()
{
	PMXFormat::MorphData result;
	ReadString(result.MorphName);
	ReadString(result.MorphEnglishName);
	byte handlePanel;
	pmxFile.read((char*) &handlePanel, sizeof(BYTE));
	result.HandlePanel = (PMXFormat::MorphData::Panel)handlePanel;
	byte morphType;
	pmxFile.read((char*) &morphType, sizeof(BYTE));
	result.Morphtype = (PMXFormat::MorphData::MorphType)morphType;
	UINT morphOffsetCount;
	pmxFile.read((char*) &morphOffsetCount, sizeof(UINT32));
	result.MorphOffsets.resize(morphOffsetCount);
	for (UINT i = 0; i < morphOffsetCount; ++i)
	{
		switch (result.Morphtype)
		{
		case PMXFormat::MorphData::MorphType::Group:
		case PMXFormat::MorphData::MorphType::Flip:
			result.MorphOffsets[i] = ReadGroupMorphOffset();
			break;
		case PMXFormat::MorphData::MorphType::Vertex:
			result.MorphOffsets[i] = ReadVertexMorphOffset();
			break;
		case PMXFormat::MorphData::MorphType::Bone:
			result.MorphOffsets[i] = ReadBoneMorphOffset();
			break;
		case PMXFormat::MorphData::MorphType::UV:
		case PMXFormat::MorphData::MorphType::AddUV1:
		case PMXFormat::MorphData::MorphType::AddUV2:
		case PMXFormat::MorphData::MorphType::AddUV3:
		case PMXFormat::MorphData::MorphType::AddUV4:
			result.MorphOffsets[i] = ReadUVMorphOffset();
			break;
		case PMXFormat::MorphData::MorphType::Material:
			result.MorphOffsets[i] = ReadMaterialMorphOffset();
			break;
		case PMXFormat::MorphData::MorphType::Impulse:
			result.MorphOffsets[i] = ReadImpulseMorphOffset();
			break;
		}
	}

	return result;
}

PMXFormat::MorphOffset PMXLoader::ReadGroupMorphOffset()
{
	PMXFormat::GroupMorphOffset result;
	result.MorphIndex = CastIntRead(format.header.MorphIndexSize);
	pmxFile.read((char*) &result.MorphRate, sizeof(float));
	return result;
}

PMXFormat::MorphOffset PMXLoader::ReadVertexMorphOffset()
{
	PMXFormat::VertexMorphOffset result;
	result.VertexIndex = CastIntRead(format.header.VertexIndexSize);
	result.PositionOffset = ReadSinglesToVector3();
	return result;
}

PMXFormat::MorphOffset PMXLoader::ReadBoneMorphOffset()
{
	PMXFormat::BoneMorphOffset result;
	result.BoneIndex = CastIntRead(format.header.BoneIndexSize);
	result.MoveValue = ReadSinglesToVector3();
	auto quat = ReadSinglesToVector4();
	result.RotateValue = glm::quat(quat.x, quat.y, quat.z, quat.w);
	return result;
}

PMXFormat::MorphOffset PMXLoader::ReadUVMorphOffset()
{
	PMXFormat::UVMorphOffset result;
	result.VertexIndex = CastIntRead(format.header.VertexIndexSize);
	result.UVOffset = ReadSinglesToVector4();
	return result;
}

PMXFormat::MorphOffset PMXLoader::ReadMaterialMorphOffset()
{
	PMXFormat::MaterialMorphOffset result;
	result.MaterialIndex = CastIntRead(format.header.MaterialIndexSize);
	BYTE offsetMethod;
	pmxFile.read((char*) &offsetMethod, sizeof(BYTE));
	result.Offsetmethod = (PMXFormat::MaterialMorphOffset::OffsetMethod)offsetMethod;
	result.Diffuse = ReadSinglesToVector4();
	result.Specular = glm::vec4(ReadSinglesToVector3(), 1.0f);
	pmxFile.read((char*) &result.Specularity, sizeof(float));
	result.Ambient = glm::vec4(ReadSinglesToVector3(), 1.0f);
	result.EdgeColor = ReadSinglesToVector4();
	pmxFile.read((char*) &result.EdgeSize, sizeof(float));
	result.TextureCoefficient = ReadSinglesToVector4();
	result.SphereTextureCoefficient = ReadSinglesToVector4();
	result.ToonTextureCoefficient = ReadSinglesToVector4();
	return result;
}

PMXFormat::MorphOffset PMXLoader::ReadImpulseMorphOffset()
{
	PMXFormat::ImpulseMorphOffset result;
	result.RigidBodyIndex = CastIntRead(format.header.MorphIndexSize);
	pmxFile.read((char*) &result.LocalFlag, sizeof(BYTE));
	result.MoveVelocity = ReadSinglesToVector3();
	result.RotationTorque = ReadSinglesToVector3();
	return result;
}

PMXFormat::DisplayFrameList PMXLoader::ReadDisplayFrameList()
{
	PMXFormat::DisplayFrameList result;
	UINT displayFrameCount;
	pmxFile.read((char*) &displayFrameCount, sizeof(UINT32));
	result.resize(displayFrameCount);
	for (UINT i = 0; i < displayFrameCount; ++i)
		result[i] = ReadDisplayFrame();
	return result;
}

PMXFormat::DisplayFrame PMXLoader::ReadDisplayFrame()
{
	PMXFormat::DisplayFrame result;
	ReadString(result.DisplayName);
	ReadString(result.DisplayNameEnglish);
	pmxFile.read((char*) &result.SpecialFrameFlag, sizeof(BYTE));
	UINT displayElementCount;
	pmxFile.read((char*) &displayElementCount, sizeof(UINT32));
	result.DisplayElements.resize(displayElementCount);
	for (UINT i = 0; i < displayElementCount; ++i)
		result.DisplayElements[i] = ReadDisplayElement();
	return result;
}

PMXFormat::DisplayElement PMXLoader::ReadDisplayElement()
{
	PMXFormat::DisplayElement result;
	pmxFile.read((char*) &result.ElementTarget, sizeof(BYTE));
	PMXFormat::Header::IndexSize elementTargetIndexSize = result.ElementTarget == 0
		? format.header.BoneIndexSize
		: format.header.MorphIndexSize;
	result.ElementTargetIndex = CastIntRead(elementTargetIndexSize);
	return result;
}

PMXFormat::RigidbodyList PMXLoader::ReadRigidBodyList()
{
	PMXFormat::RigidbodyList result;
	UINT rigidBodyCount;
	pmxFile.read((char*) &rigidBodyCount, sizeof(UINT32));
	result.resize(rigidBodyCount);
	for (UINT i = 0; i < rigidBodyCount; ++i)
		result[i] = ReadRigidBody();
	return result;
}

PMXFormat::RigidBody PMXLoader::ReadRigidBody()
{
	PMXFormat::RigidBody result;
	ReadString(result.Name);
	ReadString(result.EnglishName);
	result.RelBoneIndex = CastIntRead(format.header.BoneIndexSize);
	pmxFile.read((char*) &result.GroupIndex, sizeof(BYTE));
	pmxFile.read((char*) &result.IgnoreCollisionGroup, sizeof(USHORT));
	BYTE shapeType;
	pmxFile.read((char*) &shapeType, sizeof(BYTE));
	result.Shape_Type = (PMXFormat::RigidBody::ShapeType) shapeType;
	result.ShapeSize = ReadSinglesToVector3();
	result.ColliderPosition = ReadSinglesToVector3();
	result.colliderRotation = ReadSinglesToVector3();
	pmxFile.read((char*) &result.Weight, sizeof(float));
	pmxFile.read((char*) &result.Position_dim, sizeof(float));
	pmxFile.read((char*) &result.Rotation_dim, sizeof(float));
	pmxFile.read((char*) &result.Recoil, sizeof(float));
	pmxFile.read((char*) &result.Friction, sizeof(float));
	BYTE operationType;
	pmxFile.read((char*) &operationType, sizeof(BYTE));
	result.Operation_Type = (PMXFormat::RigidBody::OperationType)operationType;
	return result;
}

PMXFormat::RigidbodyJointList PMXLoader::ReadRigidBodyJointList()
{
	PMXFormat::RigidbodyJointList result;
	UINT rigidBodyJointCount;
	pmxFile.read((char*) &rigidBodyJointCount, sizeof(UINT32));
	result.resize(rigidBodyJointCount);
	for (UINT i = 0; i < rigidBodyJointCount; ++i)
		result[i] = ReadJoint();
	return result;
}

PMXFormat::Joint PMXLoader::ReadJoint()
{
	PMXFormat::Joint result;
	ReadString(result.Name);
	ReadString(result.EnglishName);
	BYTE operationType;
	pmxFile.read((char*) &operationType, sizeof(BYTE));
	result.Operation_Type = (PMXFormat::Joint::OperationType)operationType;
	if (result.Operation_Type == PMXFormat::Joint::Spring6D0F)
	{
		result.RigidBodyA = CastIntRead(format.header.RigidBodyIndexSize);
		result.RigidBodyB = CastIntRead(format.header.RigidBodyIndexSize);
		result.Position = ReadSinglesToVector3();
		result.Rotation = ReadSinglesToVector3();
		result.ConstrainPosLower = ReadSinglesToVector3();
		result.ConstrainPosUpper = ReadSinglesToVector3();
		result.ConstrainRotLower = ReadSinglesToVector3();
		result.ConstrainRotUpper = ReadSinglesToVector3();
		result.SpringPosition = ReadSinglesToVector3();
		result.SpringRotation = ReadSinglesToVector3();
	}
	return result;
}





