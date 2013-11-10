#include "stdafx.h"
#include "OGLRenderer.h"
#include "OpenGLControl.h"

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

	void NaiaLib::OpenGLControl::OnLoad(System::Object ^sender, System::EventArgs ^e)
	{
		m_OpenGL = new OGLRenderer();
		HWND hwnd = (HWND) Handle.ToPointer();

		OutputDebugStringA("\n############# INITIALIZING OPENGL ################\n");

		if (!m_OpenGL->Initialize(hwnd, this->Width, this->Height))
			return;
	}

	void NaiaLib::OpenGLControl::OnSizeChanged(System::Object ^sender, System::EventArgs ^e)
	{
		//m_OpenGL->Resize(this->Width, this->Height);
	}

	void NaiaLib::OpenGLControl::OnPaint(PaintEventArgs^ e)
	{
		m_OpenGL->Render();
	}
}


