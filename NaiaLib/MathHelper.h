#pragma once
	
class MathHelper
{
public:
	static glm::vec3 GetPosition(const glm::mat4x4 matrix);
	static glm::vec3 GetScale(const glm::mat4x4 matrix);
	static void SetPosition(glm::mat4x4& matrix, const glm::vec3 position);
	static void SetScale(glm::mat4x4& matrix, const glm::vec3 scale);

private:
	MathHelper();
};

