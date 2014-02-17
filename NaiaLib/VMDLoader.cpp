#include "VMDFormat.h"
#include "VMDLoader.h"
#include <algorithm>
#include "EditorCore.h"

VMDLoader::VMDLoader()
{
}


VMDLoader::~VMDLoader()
{
}

VMDFormat VMDLoader::Read(std::wstring path)
{
	vmdFile = std::ifstream(path.c_str(), std::ios::binary);
	if (!vmdFile.is_open())
	{
		std::cerr << "Cant open file: " << path.c_str() << std::endl;
		return format;
	}

	format.metaHeader  = ReadMetaHeader(path);
	format.header      = ReadHeader();
	format.Motion_list = ReadMotionList();
	format.Skin_list   = ReadSkinList();

	return format;
}

VMDFormat::MetaHeader VMDLoader::ReadMetaHeader(std::wstring path)
{
	VMDFormat::MetaHeader metaHeader;
	metaHeader.Path = path;
	metaHeader.Name = metaHeader.Path.substr(metaHeader.Path.find_last_of('\\') + 1);
	metaHeader.Folder = metaHeader.Path.substr(0, metaHeader.Path.find_last_of('\\'));
	return metaHeader;
}

VMDFormat::Header VMDLoader::ReadHeader()
{
	VMDFormat::Header result;
	CHAR vmdHeaderBytes[30];
	vmdFile.read(vmdHeaderBytes, sizeof(BYTE) * 30);
	result.VMDHeader = std::string(vmdHeaderBytes, 30);

	CHAR vmdModeNameBytes[20];
	vmdFile.read(vmdModeNameBytes, sizeof(BYTE) * 20);
	result.ModelName = std::string(vmdModeNameBytes, 20);
	return result;
}

glm::vec3 VMDLoader::ReadSinglesToVector3()
{
	float result[3];
	for (int i = 0; i < 3; ++i)
		vmdFile.read((char*) &result[i], sizeof(float));
	return glm::vec3(result[0], result[1], result[2]);
}

glm::vec4 VMDLoader::ReadSinglesToVector4()
{
	float result[4];
	for (int i = 0; i < 4; ++i)
		vmdFile.read((char*) &result[i], sizeof(float));
	return glm::vec4(result[0], result[1], result[2], result[3]);
}

VMDFormat::MotionList VMDLoader::ReadMotionList()
{
	VMDFormat::MotionList result;
	UINT motionCount;
	vmdFile.read((char*) &motionCount, sizeof(UINT32));
	std::vector<VMDFormat::Motion> motions(motionCount);
	for (UINT i = 0; i < motionCount; ++i)
		motions[i] = ReadMotion();

	std::sort(motions.begin(), motions.end(), [](VMDFormat::Motion& x, VMDFormat::Motion& y) { return x.FlameNo < y.FlameNo; });

	for (UINT i = 0; i < motionCount; ++i)
		result[motions[i].BoneName].push_back(motions[i]); 

	return result;
}

VMDFormat::Motion VMDLoader::ReadMotion()
{
	VMDFormat::Motion result;
	CHAR boneNameBytes[15];
	vmdFile.read(boneNameBytes, sizeof(BYTE) * 15);

	int count = 0;
	for (; boneNameBytes[count] != 0 && count < 15; ++count);

	result.BoneName = NaiaLib::EditorCore::ShiftJIS_To_UTF8(reinterpret_cast<const BYTE*>(boneNameBytes), count);

	vmdFile.read((char*) &result.FlameNo, sizeof(UINT32));
	result.Location = ReadSinglesToVector3();
	auto quat = ReadSinglesToVector4();
	result.Rotation = glm::quat(quat.x, quat.y, quat.z, quat.w);
	BYTE interpolation[64];
	vmdFile.read((char*) interpolation, sizeof(BYTE) * 64);
	result.Interpolation.resize(64);
	
	for (int i = 0; i < 64; ++i)
		result.Interpolation[i] = interpolation[i];

	return result;
}

VMDFormat::SkinList VMDLoader::ReadSkinList()
{
	VMDFormat::SkinList result;
	UINT skinDataCount;
	vmdFile.read((char*) &skinDataCount, sizeof(UINT32));

	std::vector<VMDFormat::SkinData> dataVector(skinDataCount);
	for (UINT i = 0; i < skinDataCount; ++i)
		dataVector[i] = ReadSkinData();

	std::sort(dataVector.begin(), dataVector.end(), [](VMDFormat::SkinData x, VMDFormat::SkinData y){ return x.FlameNo < y.FlameNo; });

	for (UINT i = 0; i < skinDataCount; ++i)
		result[dataVector[i].Name] = std::vector<VMDFormat::SkinData>();

	for (UINT i = 0; i < skinDataCount; ++i)
		result[dataVector[i].Name].push_back(dataVector[i]);

	return result;
}

VMDFormat::SkinData VMDLoader::ReadSkinData()
{
	VMDFormat::SkinData result;
	CHAR skinNameBytes[15];
	vmdFile.read(skinNameBytes, sizeof(CHAR) * 15);
	int count = 0;
	for (; skinNameBytes[count] != 0 && count < 15; ++count);
	result.Name = NaiaLib::EditorCore::ShiftJIS_To_UTF8(reinterpret_cast<const BYTE*>(skinNameBytes), count);
	vmdFile.read((char*) &result.FlameNo, sizeof(UINT32));
	vmdFile.read((char*) &result.Weight, sizeof(float));
	return result;
}
