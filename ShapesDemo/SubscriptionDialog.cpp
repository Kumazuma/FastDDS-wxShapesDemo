#include "SubscriptionDialog.h"

#include "Model.h"

BEGIN_EVENT_TABLE(SubscriptionDialog, wxDialog)
EVT_CHECKBOX(ID_CHK_ALL, SubscriptionDialog::OnChkPartition)
EVT_CHECKBOX(ID_CHK_A, SubscriptionDialog::OnChkPartition)
EVT_CHECKBOX(ID_CHK_B, SubscriptionDialog::OnChkPartition)
EVT_CHECKBOX(ID_CHK_C, SubscriptionDialog::OnChkPartition)
EVT_CHECKBOX(ID_CHK_D, SubscriptionDialog::OnChkPartition)
EVT_RADIOBUTTON(ID_CHK_CIRCLE, SubscriptionDialog::OnRadioShapeKind)
EVT_RADIOBUTTON(ID_CHK_TRIANGLE, SubscriptionDialog::OnRadioShapeKind)
EVT_RADIOBUTTON(ID_CHK_SQUARE, SubscriptionDialog::OnRadioShapeKind)
EVT_RADIOBUTTON(ID_CHK_BEST_EFFORT, SubscriptionDialog::OnRadioReliability)
EVT_RADIOBUTTON(ID_CHK_RELIABLE, SubscriptionDialog::OnRadioReliability)
EVT_RADIOBUTTON(ID_CHK_USE_TAKE, SubscriptionDialog::OnRadioGetMethod)
EVT_RADIOBUTTON(ID_CHK_USE_READ, SubscriptionDialog::OnRadioGetMethod)
END_EVENT_TABLE()

SubscriptionDialog::SubscriptionDialog(wxWindow* parent)
	: SubscriptionDialogBase(parent)
	, m_shapeKind(ShapeKind::Circle)
	, m_partition()
	, m_reliabilityKind(ReliabilityQosPolicyKind::BEST_EFFORT_RELIABILITY_QOS)
	, m_useTakeMethod(true)
{

}

ShapeKind SubscriptionDialog::GetShapeKind() const
{
	return m_shapeKind;
}

uint8_t SubscriptionDialog::GetPartition() const
{
	return m_partition;
}

ReliabilityQosPolicyKind SubscriptionDialog::GetReliabilityKind() const
{
	return m_reliabilityKind;
}

bool SubscriptionDialog::GetUseTakeMethod() const
{
	return m_useTakeMethod;
}

void SubscriptionDialog::OnChkPartition(wxCommandEvent& event)
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

void SubscriptionDialog::OnRadioShapeKind(wxCommandEvent& event)
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

void SubscriptionDialog::OnRadioReliability(wxCommandEvent& event)
{
	switch(event.GetId())
	{
	case ID_CHK_BEST_EFFORT:
		m_reliabilityKind = ReliabilityQosPolicyKind::BEST_EFFORT_RELIABILITY_QOS;
		break;
	case ID_CHK_RELIABLE:
		m_reliabilityKind = ReliabilityQosPolicyKind::RELIABLE_RELIABILITY_QOS;
		break;
	}
}

void SubscriptionDialog::OnRadioGetMethod(wxCommandEvent& event)
{
	switch(event.GetId())
	{
	case ID_CHK_USE_TAKE:
		m_useTakeMethod = true;
		break;

	case ID_CHK_USE_READ:
		m_useTakeMethod = false;
		break;
	}
}
