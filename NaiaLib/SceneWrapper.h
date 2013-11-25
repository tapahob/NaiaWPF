#pragma once
#include "Scene.h"

namespace NaiaLib
{
	public ref class SceneWrapper
	{
	public:
		static void Init();
		static void Destroy();
		static void AddMeshNode();

		static void StartMainLoop();
		
	public:
		static Scene* pScene = NULL;

	private:
		SceneWrapper();
		static void loop();
	};
}
