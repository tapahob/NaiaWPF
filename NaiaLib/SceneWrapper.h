#pragma once
#include "Scene.h"

using namespace System;
using namespace System::Windows::Controls;

namespace NaiaLib
{
	public ref class SceneWrapper
	{
	public:
		static void Init();
		static void Destroy();
		static void AddMeshNode();
		static void UpdateSceneWindow(TreeViewItem^ rootElement);
	private:
		
		//static void DisplayChildren(TreeViewItem);

	public:
		static Scene* pScene = NULL;

	private:
		SceneWrapper();
	};
}
