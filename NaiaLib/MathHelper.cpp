#include "precompiled.h"
#include "MathHelper.h"

MathHelper::MathHelper()
{
}

glm::vec3 MathHelper::GetPosition(const glm::mat4x4 matrix)
{
	return glm::vec3(matrix[3]);
}

glm::vec3 MathHelper::GetScale(const glm::mat4x4 matrix)
{
	return glm::vec3(matrix[0].x, matrix[1].y, matrix[2].z);
}

void MathHelper::SetPosition(glm::mat4x4& matrix, const glm::vec3 position)
{
	matrix[3].x = position.x;
	matrix[3].y = position.y;
	matrix[3].z = position.z;
}

void MathHelper::SetScale(glm::mat4x4& matrix, const glm::vec3 scale)
{
	matrix[0].x = scale.x;
	matrix[1].y = scale.y;
	matrix[2].z = scale.z;
}

