#pragma once
#include "ShapesDemoFrameBase.h"

class ShapesDemoFrame: public ShapesDemoFrameBase
{
	DECLARE_EVENT_TABLE();
public:
	ShapesDemoFrame();

protected:
	void OnClickedPublish(wxCommandEvent&);
	void OnClickedSubscribe(wxCommandEvent&);
};