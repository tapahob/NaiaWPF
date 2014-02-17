#pragma once
#include "precompiled.h"




class VMDFormat
{
public:
	//------------------------------------------------------------------------
	//	Meta Header
	//------------------------------------------------------------------------
	class MetaHeader
	{
	public:
		std::wstring Path;	// Fullpath
		std::wstring Name;	// The name of the file without path and extension
		std::wstring Folder; // Path without the file name
	};

	//------------------------------------------------------------------------
	//	Header
	//------------------------------------------------------------------------
	class Header
	{
	public:
		std::string VMDHeader; // 30byte, Vocaloid motion data 0002
		std::string ModelName; // 20byte
	};

	class Motion
	{
	public:
		BYTE GetInterpolation(int i, int j, int k) { return Interpolation[i * 16 + j * 4 + k]; };
		void SetInterpolation(BYTE val, int i, int j, int k) { Interpolation[i * 16 + j * 4 + k] = val; }

	public:
		std::wstring       BoneName;	// 15byte
		UINT               FlameNo;
		glm::vec3          Location;
		glm::quat          Rotation;
		std::vector<BYTE>  Interpolation; // [4][4][4], 64 byte
	};

	typedef std::map<std::wstring, std::vector<Motion>> MotionList;

	//------------------------------------------------------------------------
	//	SkinData
	//------------------------------------------------------------------------
	class SkinData
	{
	public:
		std::wstring Name; // 15byte
		UINT         FlameNo;
		float        Weight;
	};

	typedef std::map<std::wstring, std::vector<SkinData> > SkinList;

	//------------------------------------------------------------------------
	//	CameraData
	//------------------------------------------------------------------------
	class CameraData
	{
	public:
		BYTE GetInterpolation(int i, int j) { return Interpolation[i * 6 + j]; }
		void SetInterpolation(BYTE val, int i, int j) { Interpolation[i * 6 + j] = val; }

	public:
		UINT              FlameNo;
		float             Length;
		glm::vec3         Location;
		glm::vec3         Rotation; // the X - axis sign is inverted, Euler angles
		std::vector<BYTE> Interpolation; // [6][4], 24byte
	};

	typedef std::map<std::wstring, std::vector<CameraData>> CameraList;

	//------------------------------------------------------------------------
	//	LightData
	//------------------------------------------------------------------------
	class LightData
	{
	public:
		UINT      FlameNo;
		glm::vec3 RGB;
		glm::vec3 Location;
	};

	typedef std::vector<LightData> LightList;

	//------------------------------------------------------------------------
	//	SelfShadowData
	//------------------------------------------------------------------------
	class SelfShadowdata
	{
	public:
		UINT  FlameNo;
		BYTE  Mode; // 00-02
		float Distance;
	};

	typedef std::vector<SelfShadowdata> SelfShadowList;

public:
	VMDFormat(){};
	~VMDFormat(){};

public:
	MetaHeader	   metaHeader;
	Header         header;
	MotionList     Motion_list;
	SkinList       Skin_list;
	LightList      Light_list;
	CameraList     Camera_list;
	SelfShadowList Self_shadow_list;
};

