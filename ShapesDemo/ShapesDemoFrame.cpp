#include "ShapesDemoFrame.h"
#include "ShapesDemo.h"
#include "PublicationDialog.h"
#include "SubscriptionDialog.h"

BEGIN_EVENT_TABLE(ShapesDemoFrame, ShapesDemoFrameBase)
EVT_TOOL(ID_PUBLISH, ShapesDemoFrame::OnClickedPublish)
EVT_TOOL(ID_SUBSCRIBE, ShapesDemoFrame::OnClickedSubscribe)
END_EVENT_TABLE()

ShapesDemoFrame::ShapesDemoFrame()
	: ShapesDemoFrameBase(nullptr, wxID_ANY, wxS("FastDDS ShapesDemo"))
{
}

void ShapesDemoFrame::OnClickedPublish(wxCommandEvent&)
{
	PublicationDialog dialog(this);
	auto id = dialog.ShowModal();
	if(id != wxID_OK)
	{
		return;
	}

	auto color = dialog.GetColor();
	auto partition = dialog.GetPartition();
	auto shapeKind = dialog.GetShapeKind();
	auto& controller = wxGetApp().GetController();
	controller.CreateDataWriter(shapeKind, eprosima::fastdds::dds::DATAWRITER_QOS_DEFAULT, partition, color);
}

void ShapesDemoFrame::OnClickedSubscribe(wxCommandEvent&)
{
	SubscriptionDialog dialog(this);
	auto id = dialog.ShowModal();
	if(id != wxID_OK)
	{
		return;
	}

	auto partition = dialog.GetPartition();
	auto shapeKind = dialog.GetShapeKind();
	auto& controller = wxGetApp().GetController();
	controller.CreateDataReader(shapeKind, eprosima::fastdds::dds::DATAREADER_QOS_DEFAULT, partition, false, false);
}
