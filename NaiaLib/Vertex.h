#pragma once
#include "precompiled.h"

struct VertexTexturedLit
{
	VertexTexturedLit(){};
	VertexTexturedLit(glm::vec3 position, glm::vec3 normal, glm::vec2 texCoords)
	: Position(position), Normal(normal), TexCoords(texCoords) 
	{
	}
	glm::vec3 Position;
	glm::vec2 TexCoords;
	glm::vec3 Normal;
};