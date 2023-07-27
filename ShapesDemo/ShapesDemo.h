// ShapesDemo.h: 표준 시스템 포함 파일
// 또는 프로젝트 특정 포함 파일이 들어 있는 포함 파일입니다.

#pragma once

#include <wx/wx.h>
#include <iostream>
#include "Model.h"
#include "Controller.h"

class ShapesDemoFrame;
class ShapesDemoApp: public wxApp
{
public:
	bool OnInit() override;
	int OnExit() override;
	Model& GetModel();
	Controller& GetController();
	void UpdateView();
protected:
	void OnTimer(wxTimerEvent&);
private:
	wxTimer* m_pTimer;
	ShapesDemoFrame* m_pFrame;
	Model m_model;
	Controller m_controller;
};

wxDECLARE_APP(ShapesDemoApp);

constexpr int CANVAS_WIDTH = 236;
constexpr int CANVAS_HEIGHT = 266;