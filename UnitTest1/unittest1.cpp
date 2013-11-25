#include "stdafx.h"
#include "CppUnitTest.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace glm;

namespace UnitTest1
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			mat4x4 mat(1.0f);
			mat = glm::translate(mat, vec3(2.0f));

			vec3 position(mat[3].x, mat[3].y, mat[3].z);

			mat = glm::scale(mat, vec3(3.0));
			vec3 scale(mat[0].w, mat[1].w, mat[2].w);

			Assert::AreEqual(1, 2);
			// TODO: Your test code here
		}

	};
}