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
END_EVENT_TABLE()

SubscriptionDialog::SubscriptionDialog(wxWindow* parent)
	: SubscriptionDialogBase(parent)
	, m_shapeKind(ShapeKind::Circle)
	, m_partition()
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
