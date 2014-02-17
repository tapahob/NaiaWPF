#pragma once
#include <wtypes.h>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

class PMXFormat
{
#pragma region NestedClasses
public:
	//------------------------------------------------------------------------
	//	Meta Header
	//------------------------------------------------------------------------
	class MetaHeader
	{
	public:
		std::string Path;	// Fullpath
		std::string Name;	// The name of the file without path and extension
		std::string Folder; // Path without the file name
	};

	//------------------------------------------------------------------------
	//	Header
	//------------------------------------------------------------------------
	class Header
	{
	public:
		enum StringEncoding
		{
			Utf16le,
			Utf8
		};
		enum IndexSize
		{
			Byte1 = 1,
			Byte2 = 2,
			Byte4 = 4
		};

		char            Magic[4];		// "PMX "
		float           Version;			// 00 00 80 3F == 1.00
		BYTE            DataSize;
		StringEncoding  EncodingMethod;
		BYTE            AdditionalUV;
		IndexSize       VertexIndexSize;
		IndexSize       TextureIndexSize;
		IndexSize       MaterialIndexSize;
		IndexSize       BoneIndexSize;
		IndexSize       MorphIndexSize;
		IndexSize	    RigidBodyIndexSize;
		std::wstring    ModelName;
		std::wstring    ModelNameEnglish;
		std::wstring    Comment;
		std::wstring	CommentEnglish;
	};

	enum WeightMethod
	{
		eBDEF1,
		eBDEF2,
		eBDEF4,
		eSDEF,
		eQDEF
	};

	//------------------------------------------------------------------------
	//	BoneWeight
	//------------------------------------------------------------------------
	class BoneWeight
	{
	public:
		virtual WeightMethod		 Method()          { return WeightMethod::eBDEF1; };
		virtual UINT                 GetBone1()        { return 0; };
		virtual UINT                 GetBone2()        { return 0; };
		virtual UINT                 GetBone3()        { return 0; };
		virtual UINT                 GetBone4()        { return 0; };
		virtual float                GetBone1_Weight() { return 0.0f; };
		virtual float                GetBone2_Weight() { return 0.0f; };
		virtual float                GetBone3_Weight() { return 0.0f; };
		virtual float                GetBone4_Weight() { return 0.0f; };
		virtual glm::vec3            c_value()         { return glm::vec3(0.0f); }
		virtual glm::vec3            r0_value()        { return glm::vec3(0.0f); }
		virtual glm::vec3            r1_value()        { return glm::vec3(0.0f); }
	};

	//------------------------------------------------------------------------
	//	Vertex
	//------------------------------------------------------------------------
	class Vertex
	{
	public:
		glm::vec3              Position;
		glm::vec3              NormalVector;
		glm::vec2              UV;
		std::vector<glm::vec4> AdditionalUV;
		BoneWeight			   Boneweight;
		float                  edgeMagnification;
	};

	//------------------------------------------------------------------------
	//	VertexList
	//------------------------------------------------------------------------	
	typedef std::vector<Vertex> VertexList; // Vertex data (38bytes / vertex)

	//------------------------------------------------------------------------
	//	BDEF1
	//------------------------------------------------------------------------
	class BDEF1 : public BoneWeight
	{
	public:
		BDEF1() : bone1_value(0), bone1_weight(1.0f) {}
		WeightMethod Method()          override { return WeightMethod::eBDEF1; }
		UINT         GetBone1()        override { return bone1_value; }
		float		 GetBone1_Weight() override { return bone1_weight; };
	public:
		UINT         bone1_value;
		float        bone1_weight;
	};

	//------------------------------------------------------------------------
	//	BDEF2
	//------------------------------------------------------------------------
	class BDEF2 : public BoneWeight
	{
	public:
		BDEF2() : bone1_value(0), bone2_value(0), bone1_weight(1.0f) {}
		WeightMethod Method()          override { return WeightMethod::eBDEF2; }
		UINT         GetBone1()        override { return bone1_value; }
		UINT         GetBone2()        override { return bone2_value; }
		float		 GetBone1_Weight() override { return bone1_weight; };
		float		 GetBone2_Weight() override { return 1.0f - bone1_weight; };

	public:
		UINT  bone1_value;
		UINT  bone2_value;
		float bone1_weight;
	};

	//------------------------------------------------------------------------
	//	BDEF4	
	//------------------------------------------------------------------------
	class BDEF4 : public BoneWeight
	{
	public:
		BDEF4() : bone1_value(0), bone2_value(0), bone3_value(0), bone4_value(0),
			bone1_weight(0.0f), bone2_weight(0.0f), bone3_weight(0.0f), bone4_weight(0.0f) {}
		WeightMethod Method()          override { return WeightMethod::eBDEF4; }
		UINT         GetBone1()        override { return bone1_value; }
		UINT         GetBone2()        override { return bone2_value; }
		UINT         GetBone3()        override { return bone3_value; }
		UINT         GetBone4()        override { return bone4_value; }
		float		 GetBone1_Weight() override { return bone1_weight; };
		float		 GetBone2_Weight() override { return bone2_weight; };
		float		 GetBone3_Weight() override { return bone3_weight; };
		float		 GetBone4_Weight() override { return bone4_weight; };		

	public:
		UINT         bone1_value;
		UINT         bone2_value;
		UINT         bone3_value;
		UINT         bone4_value;
		float		 bone1_weight;
		float		 bone2_weight;
		float		 bone3_weight;
		float		 bone4_weight;
	};

	//------------------------------------------------------------------------
	//	SDEF
	//------------------------------------------------------------------------
	class SDEF : public BoneWeight
	{
	public:
		SDEF() : bone1_value(0), bone2_value(0), bone1_weight(1.0f), 
			c(0.0f), r0(0.0f), r1(0.0f) {}
		WeightMethod Method() override { return WeightMethod::eSDEF; }
		UINT         GetBone1() override { return bone1_value; }
		UINT         GetBone2() override { return bone2_value; }
		float        GetBone1_Weight() override { return bone1_weight; }
		float        GetBone2_Weight() override { return 1.0f - bone1_weight; };
		glm::vec3    c_value() override{ return c; }
		glm::vec3    r0_value() override { return r0; }
		glm::vec3    r1_value() override { return r1; }

	public:
		UINT         bone1_value;
		UINT         bone2_value;
		float		 bone1_weight;
		glm::vec3	 c;
		glm::vec3	 r0;
		glm::vec3	 r1;
	};

	//------------------------------------------------------------------------	
	//	QDEF
	//------------------------------------------------------------------------
	class QDEF : public BoneWeight
	{
	public:
		WeightMethod Method()          override { return WeightMethod::eQDEF; }
		UINT         GetBone1()        override { return bone1_value; }
		UINT         GetBone2()        override { return bone2_value; }
		UINT         GetBone3()        override { return bone3_value; }
		UINT         GetBone4()        override { return bone4_value; }
		float		 GetBone1_Weight() override { return bone1_weight; }
		float		 GetBone2_Weight() override { return bone2_weight; }
		float		 GetBone3_Weight() override { return bone3_weight; }
		float		 GetBone4_Weight() override { return bone4_weight; }

	public:
		UINT         bone1_value;
		UINT         bone2_value;
		UINT         bone3_value;
		UINT         bone4_value;
		float		 bone1_weight;
		float		 bone2_weight;
		float		 bone3_weight;
		float		 bone4_weight;
	};

	//------------------------------------------------------------------------
	//	TYPEDEFS
	//------------------------------------------------------------------------
	typedef std::vector<unsigned int> FaceVertexList; //  (3 / face)
	typedef std::vector<std::wstring> TextureList; // 100byte * 10 fixed

	//------------------------------------------------------------------------
	//	Material
	//------------------------------------------------------------------------
	class Material
	{
	public:
		enum Flag
		{
			Reversible        = 1 << 0,
			CastShadow        = 1 << 1, // Ground Shadow
			CastSelfShadow    = 1 << 2,
			ReceiveSelfShadow = 1 << 3,
			Edge              = 1 << 4
		};
		
		enum SphereMode
		{
			Null,
			MulSphere,
			AddSphere,
			SubTexture
		};

		std::wstring	    Name;
		std::wstring	    NameEnglish;
		glm::vec4		    DiffuseColor;
		glm::vec4		    SpecularColor;
		float			    Specularity;
		glm::vec4		    AmbientColor;
		Flag			    flag;
		glm::vec4		    EdgeColor;
		float			    EdgeSize;
		UINT	            UsuallyTextureIndex;
		BYTE	            SphereTextureIndex;
		SphereMode		    Sphere_Mode;
		BYTE	            CommonToon;
		UINT	            ToonTextureIndex;
		std::wstring		Memo;
		UINT	            FaceVertexCount;

	};

	typedef std::vector<Material> MaterialList;

	//------------------------------------------------------------------------
	//	IKLink
	//------------------------------------------------------------------------
	class IKLink
	{
	public:
		UINT TargetBoneIndex;
		BYTE AngleLimit;
		glm::vec3 LowerLimit;
		glm::vec3 UpperLimit;
	};

	//------------------------------------------------------------------------
	//	IKData
	//------------------------------------------------------------------------
	class IKData
	{
	public:
		UINT                IKBoneIndex;
		UINT                Iterations;
		float               LimitAngle;
		std::vector<IKLink> IKLinks;
	};

	//------------------------------------------------------------------------
	//	Bone
	//------------------------------------------------------------------------
	class Bone
	{
	public:
		enum Flag
		{
			Connection              = 1 << 0,
			Rotatable               = 1 << 1,
			Movable                 = 1 << 2,
			DisplayFlag             = 1 << 3,
			CanOperate              = 1 << 4,
			IkFlag                  = 1 << 5,
			AddLocal                = 1 << 7,
			AddRotation             = 1 << 8,
			AddMove                 = 1 << 9,
			FixedAxis               = 1 << 10,
			LocalAxis               = 1 << 11,
			PhysicsTransform        = 1 << 12,
			ExternalParentTransform = 1 << 13
		};

		std::wstring	BoneName;
		std::wstring	BoneNameEnglish;;
		glm::vec3		BonePosition;
		unsigned int	ParentBoneIndex; // Uint.MaxValue if not
		int				TransformLevel;
		Flag			BoneFlag;
		glm::vec3       PositionOffset;
		unsigned int	ConnectionIndex;
		unsigned int	AdditionalParentIndex;
		float	        AdditionalRate;
		glm::vec3       AxisVector;
		glm::vec3       xAxisVector;
		glm::vec3       zAxisVector;
		unsigned int	KeyValue;
		IKData			IkData;
	};

	typedef std::vector<Bone> BoneList; // (39bytes/bone

	//------------------------------------------------------------------------
	//	MorphOffset
	//------------------------------------------------------------------------
	class MorphOffset {};

	//------------------------------------------------------------------------
	//	VertexMorphOffset
	//------------------------------------------------------------------------
	class VertexMorphOffset : public MorphOffset
	{
	public:
		UINT      VertexIndex;
		glm::vec3 PositionOffset;
	};

	//------------------------------------------------------------------------
	//	UVMorphOffset
	//------------------------------------------------------------------------
	class UVMorphOffset : public MorphOffset
	{
	public:
		UINT      VertexIndex;
		glm::vec4 UVOffset;
	};

	//------------------------------------------------------------------------
	//	BoneMorphOffset
	//------------------------------------------------------------------------
	class BoneMorphOffset : public MorphOffset
	{
	public:
		UINT BoneIndex;
		glm::vec3 MoveValue;
		glm::quat RotateValue;
	};

	//------------------------------------------------------------------------
	//	MaterialMorphOffset
	//------------------------------------------------------------------------
	class MaterialMorphOffset : public MorphOffset
	{
	public:
		enum OffsetMethod
		{
			Mul,
			Add
		};
		UINT MaterialIndex;
		OffsetMethod Offsetmethod;
		glm::vec4 Diffuse;
		glm::vec4 Specular;
		float Specularity;
		glm::vec4 Ambient;
		glm::vec4 EdgeColor;
		float EdgeSize;
		glm::vec4 TextureCoefficient;
		glm::vec4 SphereTextureCoefficient;
		glm::vec4 ToonTextureCoefficient;
	};

	//------------------------------------------------------------------------
	//	MorphData
	//------------------------------------------------------------------------
	class MorphData
	{
	public:
		enum Panel
		{
			Base,
			Eyebrow,
			Eye,
			Lip,
			Other
		};

		enum MorphType
		{
			Group,
			Vertex,
			Bone,
			UV,
			AddUV1,
			AddUV2,
			AddUV3,
			AddUV4,
			Material,
			Flip,
			Impulse
		};

		std::wstring             MorphName;
		std::wstring             MorphEnglishName;
		Panel                    HandlePanel;
		MorphType                Morphtype;
		std::vector<MorphOffset> MorphOffsets;
	};

	typedef std::vector<MorphData> MorphList;

	//------------------------------------------------------------------------
	//	GroupMorphOffset
	//------------------------------------------------------------------------
	class GroupMorphOffset : public MorphOffset
	{
	public:
		UINT MorphIndex;
		float MorphRate;
	};

	//------------------------------------------------------------------------
	//	ImpulseMorphOffset
	//------------------------------------------------------------------------
	class ImpulseMorphOffset : public MorphOffset
	{
	public:
		UINT RigidBodyIndex;
		BYTE LocalFlag;
		glm::vec3 MoveVelocity;
		glm::vec3 RotationTorque;
	};

	//------------------------------------------------------------------------
	//	DisplayElement
	//------------------------------------------------------------------------
	class DisplayElement
	{
	public:
		BYTE ElementTarget;
		UINT ElementTargetIndex;
	};

	//------------------------------------------------------------------------
	//	DisplayFrame
	//------------------------------------------------------------------------
	class DisplayFrame
	{
	public:
		std::wstring                DisplayName;
		std::wstring                DisplayNameEnglish;
		BYTE                        SpecialFrameFlag;
		std::vector<DisplayElement> DisplayElements;
	};

	typedef std::vector<DisplayFrame> DisplayFrameList;

	//------------------------------------------------------------------------
	//	RigidBody
	//------------------------------------------------------------------------
	class RigidBody
	{
	public:
		enum ShapeType
		{
			Sphere,
			Box,
			Capsule
		};

		enum OperationType
		{
			Static,
			Dynamic,
			DynamicAndPosAdjust
		};

		std::wstring  Name;
		std::wstring  EnglishName;
		UINT          RelBoneIndex;
		BYTE          GroupIndex;
		USHORT        IgnoreCollisionGroup;
		ShapeType     Shape_Type;
		glm::vec3     ShapeSize;
		glm::vec3     ColliderPosition;
		glm::vec3     colliderRotation;
		float         Weight;
		float         Position_dim;
		float         Rotation_dim;
		float         Recoil;
		float         Friction;
		OperationType Operation_Type;
	};

	typedef std::vector<RigidBody> RigidbodyList;

	//------------------------------------------------------------------------
	//	Joint
	//------------------------------------------------------------------------
	class Joint
	{
	public:
		enum OperationType
		{
			Spring6D0F
		};

		std::wstring  Name;
		std::wstring  EnglishName;
		OperationType Operation_Type;
		UINT          RigidBodyA;
		UINT          RigidBodyB;
		glm::vec3     Position;
		glm::vec3     Rotation;
		glm::vec3     ConstrainPosLower;
		glm::vec3     ConstrainPosUpper;
		glm::vec3     ConstrainRotLower;
		glm::vec3     ConstrainRotUpper;
		glm::vec3     SpringPosition;
		glm::vec3     SpringRotation;
	};

	typedef std::vector<Joint> RigidbodyJointList;
#pragma endregion NestedClasses

public:
	PMXFormat();
	~PMXFormat();

public:
	MetaHeader         meta_header;
	Header             header;
	VertexList         vertex_list;
	FaceVertexList     face_vertex_list;
	TextureList        texture_list;
	MaterialList       material_list;
	BoneList           bone_list;
	MorphList          morph_list;
	DisplayFrameList   display_frame_list;
	RigidbodyList      rigidbody_list;
	RigidbodyJointList rigidbody_joint_list;
};

