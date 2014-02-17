#pragma once

#pragma comment(lib, "libiconv.lib")

#include <iostream>
#include <fstream>
#include "VMDFormat.h"
#include <iconv.h>

class VMDLoader
{
public:
	VMDLoader();
	~VMDLoader();

	VMDFormat Read(std::wstring path);

private:
	VMDFormat::MetaHeader ReadMetaHeader(std::wstring str);
	VMDFormat::Header     ReadHeader();
	VMDFormat::MotionList ReadMotionList();
	VMDFormat::Motion     ReadMotion();
	VMDFormat::SkinList   ReadSkinList();
	VMDFormat::SkinData   ReadSkinData();
	glm::vec3             ReadSinglesToVector3();
	glm::vec4             ReadSinglesToVector4();

private:
	VMDFormat             format;
	std::ifstream         vmdFile;
};

