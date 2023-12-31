﻿#include "ShapesDemoFrame.h"
#include "ShapesDemo.h"
#include "PublicationDialog.h"
#include "ReaderSampleListDialog.h"
#include "SubscriptionDialog.h"

BEGIN_EVENT_TABLE(ShapesDemoFrame, ShapesDemoFrameBase)
EVT_TOOL(ID_PUBLISH, ShapesDemoFrame::OnClickedPublish)
EVT_TOOL(ID_SUBSCRIBE, ShapesDemoFrame::OnClickedSubscribe)
EVT_DATAVIEW_ITEM_ACTIVATED(ID_VIEW_ENDPOINTS, ShapesDemoFrame::OnActivatedEndpointsViewItem)
END_EVENT_TABLE()

ShapesDemoFrame::ShapesDemoFrame()
	: ShapesDemoFrameBase(nullptr, wxID_ANY, wxS("FastDDS ShapesDemo"))
{
}

void ShapesDemoFrame::UpdateView()
{
	auto window = wxWindow::FindWindowById(ShapesDemoFrame::ID_CANVAS, this);
	if(window == nullptr)
		return;

	window->Refresh();
	for(auto* pDialog: m_sampleListDialog)
	{
		pDialog->UpdateList();
	}
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
	auto reliabilityKind = dialog.GetReliabilityKind();
	auto& controller = wxGetApp().GetController();
	
	auto dwQos = eprosima::fastdds::dds::DATAWRITER_QOS_DEFAULT;
	dwQos.reliability().kind = reliabilityKind;
	auto ret = controller.CreateDataWriter(shapeKind, dwQos, partition, color);
	if(!ret.has_value())
	{
		return;
	}

	const auto& guid = ret.value();
	auto& model = wxGetApp().GetModel();
	auto& entity = model.entityInfoTable[guid];
	wxVector<wxVariant> row;
	wxString guidStr;
	for(int i = 0; i < 12; ++i)
	{
		guidStr += wxString::Format(wxS("%02X"), entity.guid.guidPrefix.value[i]);
		if(i % 4 == 3)
			guidStr += wxS('-');
	}

	for(int i = 0; i < 4; ++i)
	{
		guidStr += wxString::Format(wxS("%02X"), entity.guid.entityId.value[i]);
	}

	row.push_back(entity.entityType);
	row.push_back(guidStr);
	row.push_back(entity.topic);
	row.push_back(entity.color);
	row.push_back(entity.reliability);

	m_dataViewListCtrl->AppendItem(row, 0);
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
	auto reliabilityKind = dialog.GetReliabilityKind();
	auto& controller = wxGetApp().GetController();
	auto qos = eprosima::fastdds::dds::DATAREADER_QOS_DEFAULT;
	auto useTake = dialog.GetUseTakeMethod();
	qos.reliability().kind = reliabilityKind;
	qos.history().depth = 3;
	auto ret = controller.CreateDataReader(shapeKind, qos, partition, false, useTake);
	if(!ret.has_value())
	{
		return;
	}

	const auto& guid = ret.value();
	auto& model = wxGetApp().GetModel();
	auto& entity = model.entityInfoTable[guid];
	wxVector<wxVariant> row;
	wxString guidStr;
	for(int i = 0; i < 12; ++i)
	{
		guidStr += wxString::Format(wxS("%02X"), entity.guid.guidPrefix.value[i]);
		if(i % 4 == 3)
			guidStr += wxS('-');
	}

	for(int i = 0; i < 4; ++i)
	{
		guidStr += wxString::Format(wxS("%02X"), entity.guid.entityId.value[i]);
	}

	row.push_back(entity.entityType);
	row.push_back(guidStr);
	row.push_back(entity.topic);
	row.push_back(entity.color);
	row.push_back(entity.reliability);

	m_dataViewListCtrl->AppendItem(row, (wxUIntPtr)&entity);
}

void ShapesDemoFrame::OnActivatedEndpointsViewItem(wxDataViewEvent& event)
{
	EntityInfo* entityInfo = (EntityInfo*)m_dataViewListCtrl->GetItemData(event.GetItem());
	auto& model = wxGetApp().GetModel();
	auto it = model.shapeListTable.find(entityInfo->guid);
	if(it == model.shapeListTable.end())
		return;

	auto* pDialog = new ReaderSampleListDialog(this, wxID_ANY, entityInfo->guid);
	pDialog->Bind(wxEVT_CLOSE_WINDOW, [this, pDialog](wxCloseEvent& evt)
	{
		evt.Skip();
		auto it = std::find(m_sampleListDialog.begin(), m_sampleListDialog.end(), pDialog);
		m_sampleListDialog.erase(it);
	});

	m_sampleListDialog.push_back(pDialog);
	pDialog->UpdateList();
	pDialog->Show();
}
