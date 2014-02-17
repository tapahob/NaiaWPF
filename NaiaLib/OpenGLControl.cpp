#include "precompiled.h"
#include "OGLRenderer.h"
#include "OpenGLControl.h"
#include "SceneWrapper.h"
#include "NaiaCore.h"
#include "EditorCore.h"

namespace NaiaLib
{
	OpenGLControl::OpenGLControl()
	{
		this->Load += gcnew System::EventHandler(this, &NaiaLib::OpenGLControl::OnLoad);
		this->SizeChanged += gcnew System::EventHandler(this, &NaiaLib::OpenGLControl::OnSizeChanged);
	}

	OpenGLControl::~OpenGLControl()
	{
		if (this->m_OpenGL)
		{
			this->m_OpenGL->Shutdown();
			this->m_OpenGL = NULL;
		}
	}

	void OpenGLControl::OnClick(EventArgs^ e)
	{
		cout << "Setting Active renderer: " << m_OpenGL->id << endl;
		NaiaLib::EditorCore::Instance->activeRenderer = m_OpenGL->id;
	}

	void NaiaLib::OpenGLControl::OnLoad(System::Object ^sender, System::EventArgs ^e)
	{
		m_OpenGL = new OGLRenderer(SceneWrapper::pScene);
		HWND hwnd = (HWND) Handle.ToPointer();

		OutputDebugStringA("\n############# INITIALIZING OPENGL ............... ");

		if (!m_OpenGL->Initialize(hwnd, this->Width, this->Height))
			return;

		NaiaCore::Instance()->AddWindow(m_OpenGL);

		m_OpenGL->Resize(this->Width, this->Height);

		auto hdc = NaiaCore::Instance()->Windows[0]->Hdc;
		wglMakeCurrent(hdc, g_GLMainContext);
	}

	void NaiaLib::OpenGLControl::OnSizeChanged(System::Object ^sender, System::EventArgs ^e)
	{
			m_OpenGL->Resize(this->Width, this->Height);
	}

	void NaiaLib::OpenGLControl::OnPaint(PaintEventArgs^ e)
	{
	}
}