#pragma once
#include "SubscriptionDialogBase.h"
enum class ShapeKind;

class SubscriptionDialog: public SubscriptionDialogBase
{
	DECLARE_EVENT_TABLE()
public:
	SubscriptionDialog(wxWindow* parent);
	ShapeKind GetShapeKind() const;
	uint8_t GetPartition() const;
protected:
	void OnChkPartition(wxCommandEvent&);
	void OnRadioShapeKind(wxCommandEvent&);
private:
	wxString m_selectedColor;
	uint8_t m_partition;
	ShapeKind m_shapeKind;
};
