#pragma once
#include <fastdds/rtps/common/Guid.h>
#include "ShapesDemoFrameBase.h"

class ReaderSampleListDialog;
class wxDialog;

class ShapesDemoFrame: public ShapesDemoFrameBase
{
	DECLARE_EVENT_TABLE();
public:
	ShapesDemoFrame();
	void UpdateView();
protected:
	void OnClickedPublish(wxCommandEvent&);
	void OnClickedSubscribe(wxCommandEvent&);
	void OnActivatedEndpointsViewItem(wxDataViewEvent&);
private:
	std::vector<ReaderSampleListDialog*> m_sampleListDialog;
};
