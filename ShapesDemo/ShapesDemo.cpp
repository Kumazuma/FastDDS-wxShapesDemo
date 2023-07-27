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
			delete m_pTimer;
		}

		m_pFrame = nullptr;
		m_controller.Shutdown();
	});
	m_pTimer = new wxTimer(this);
	m_pTimer->Start(1000 / 60);
	Bind(wxEVT_TIMER, &ShapesDemoApp::OnTimer, this);
	return true;
}

int ShapesDemoApp::OnExit()
{
	int ret = wxApp::OnExit();
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

void ShapesDemoApp::UpdateView()
{
	m_pFrame->UpdateView();
}

void ShapesDemoApp::OnTimer(wxTimerEvent&)
{
	if(m_pFrame == nullptr)
		return;

	m_controller.Update(1.f / 60.f);
	UpdateView();
}
