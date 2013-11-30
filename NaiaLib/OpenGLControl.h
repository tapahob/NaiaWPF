#pragma once
using namespace System;
using namespace System::Windows::Forms;

namespace NaiaLib
{
	public ref class OpenGLControl : public System::Windows::Forms::UserControl
	{
	public:
		OpenGLControl();
		virtual ~OpenGLControl();
		virtual void OnPaintBackground(PaintEventArgs^ e) override {}
		virtual void OnPaint(PaintEventArgs^ e) override;

	private:
		OGLRenderer *m_OpenGL;
		System::ComponentModel::Container^ components;
		void OnLoad(System::Object ^sender, System::EventArgs ^e);
		void OnSizeChanged(System::Object ^sender, System::EventArgs ^e);
	};
}

