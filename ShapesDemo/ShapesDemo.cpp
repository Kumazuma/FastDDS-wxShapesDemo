// ShapesDemo.cpp : 애플리케이션의 진입점을 정의합니다.
//

#include "ShapesDemo.h"
#include "ShapesDemoFrame.h"

wxIMPLEMENT_APP(ShapesDemoApp);

bool ShapesDemoApp::OnInit()
{
	if(!wxApp::OnInit())
	{
		return false;
	}

	m_controller.Init(0);
	m_pFrame = new ShapesDemoFrame();
	m_pFrame->Show();
	m_pFrame->Bind(wxEVT_CLOSE_WINDOW, [this](wxCloseEvent& event)
	{
		event.Skip();
		if(m_pTimer != nullptr)
		{
			m_pTimer->Stop();
		}

		m_pFrame = nullptr;
	});
	m_pTimer = new wxTimer(this);
	m_pTimer->Start(16);
	Bind(wxEVT_TIMER, &ShapesDemoApp::OnTimer, this);
	return true;
}

int ShapesDemoApp::OnExit()
{
	if(m_pTimer != nullptr)
	{
		delete m_pTimer;
	}
	
	int ret = wxApp::OnExit();
	m_controller.Shutdown();
	return ret;
}

Model& ShapesDemoApp::GetModel()
{
	return m_model;
}

Controller& ShapesDemoApp::GetController()
{
	return m_controller;
}

void ShapesDemoApp::OnTimer(wxTimerEvent&)
{
	if(m_pFrame == nullptr)
		return;

	m_controller.Update(16.f / 1000.f);
	m_pFrame->UpdateView();

}
