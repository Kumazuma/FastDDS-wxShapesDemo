#include "PublicationDialog.h"

#include <fastrtps/qos/QosPolicies.h>

#include "Model.h"

BEGIN_EVENT_TABLE(PublicationDialog, wxDialog)
EVT_RADIOBUTTON(ID_CHK_RED, PublicationDialog::OnRadioColor)
EVT_RADIOBUTTON(ID_CHK_GREEN, PublicationDialog::OnRadioColor)
EVT_RADIOBUTTON(ID_CHK_BLUE, PublicationDialog::OnRadioColor)
EVT_RADIOBUTTON(ID_CHK_CYAN, PublicationDialog::OnRadioColor)
EVT_RADIOBUTTON(ID_CHK_MAGENTA, PublicationDialog::OnRadioColor)
EVT_RADIOBUTTON(ID_CHK_YELLOW, PublicationDialog::OnRadioColor)
EVT_RADIOBUTTON(ID_CHK_PURPLE, PublicationDialog::OnRadioColor)
EVT_RADIOBUTTON(ID_CHK_ORANGE, PublicationDialog::OnRadioColor)
EVT_CHECKBOX(ID_CHK_ALL, PublicationDialog::OnChkPartition)
EVT_CHECKBOX(ID_CHK_A, PublicationDialog::OnChkPartition)
EVT_CHECKBOX(ID_CHK_B, PublicationDialog::OnChkPartition)
EVT_CHECKBOX(ID_CHK_C, PublicationDialog::OnChkPartition)
EVT_CHECKBOX(ID_CHK_D, PublicationDialog::OnChkPartition)
EVT_RADIOBUTTON(ID_CHK_CIRCLE, PublicationDialog::OnRadioShapeKind)
EVT_RADIOBUTTON(ID_CHK_TRIANGLE, PublicationDialog::OnRadioShapeKind)
EVT_RADIOBUTTON(ID_CHK_SQUARE, PublicationDialog::OnRadioShapeKind)
EVT_RADIOBUTTON(ID_CHK_BEST_EFFORT, PublicationDialog::OnRadioReliability)
EVT_RADIOBUTTON(ID_CHK_RELIABLE, PublicationDialog::OnRadioReliability)
END_EVENT_TABLE()

PublicationDialog::PublicationDialog(wxWindow* parent)
	: PublicationDialogBase(parent)
	, m_shapeKind(ShapeKind::Circle)
	, m_partition()
	, m_selectedColor(wxS("RED"))
	, m_reliabilityKind(eprosima::fastrtps::RELIABLE_RELIABILITY_QOS)
{

}

wxString PublicationDialog::GetColor() const
{
	return m_selectedColor;
}

ShapeKind PublicationDialog::GetShapeKind() const
{
	return m_shapeKind;
}

uint8_t PublicationDialog::GetPartition() const
{
	return m_partition;
}

ReliabilityQosPolicyKind PublicationDialog::GetReliabilityKind() const
{
	return m_reliabilityKind;
}

void PublicationDialog::OnRadioColor(wxCommandEvent& event)
{
	auto radioButton = wxStaticCast(event.GetEventObject(), wxRadioButton);
	m_selectedColor = radioButton->GetLabel();
}

void PublicationDialog::OnChkPartition(wxCommandEvent& event)
{
	auto checkButton = wxStaticCast(event.GetEventObject(), wxCheckBox);
	uint8_t mask = 0;
	switch(checkButton->GetId())
	{
	case ID_CHK_ALL:
		mask = (0x1 << 0);
		break;

	case ID_CHK_A:
		mask = (0x1 << 1);
		break;

	case ID_CHK_B:
		mask = (0x1 << 2);
		break;

	case ID_CHK_C:
		mask = (0x1 << 3);
		break;

	case ID_CHK_D:
		mask = (0x1 << 4);
		break;
	}

	if(checkButton->GetValue())
	{
		m_partition = m_partition | mask;
	}
	else
	{
		m_partition = m_partition & ~mask;
	}
}

void PublicationDialog::OnRadioShapeKind(wxCommandEvent& event)
{
	switch(event.GetId())
	{
	case ID_CHK_CIRCLE:
		m_shapeKind = ShapeKind::Circle;
		break;

	case ID_CHK_TRIANGLE:
		m_shapeKind = ShapeKind::Triangle;
		break;

	case ID_CHK_SQUARE:
		m_shapeKind = ShapeKind::Square;
		break;
	}
}

void PublicationDialog::OnRadioReliability(wxCommandEvent& event)
{
	switch(event.GetId())
	{
	case ID_CHK_BEST_EFFORT:
		m_reliabilityKind = eprosima::fastdds::dds::BEST_EFFORT_RELIABILITY_QOS;
		break;

	case ID_CHK_RELIABLE:
		m_reliabilityKind = eprosima::fastdds::dds::RELIABLE_RELIABILITY_QOS;
		break;

	}
}
