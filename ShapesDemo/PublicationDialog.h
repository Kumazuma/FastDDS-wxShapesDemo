#pragma once
#include "PublicationDialogBase.h"

enum class ShapeKind;
class PublicationDialog: public PublicationDialogBase
{
	DECLARE_EVENT_TABLE();
public:
	PublicationDialog(wxWindow* parent);
	wxString GetColor() const;
	ShapeKind GetShapeKind() const;
	uint8_t GetPartition() const;
protected:
	void OnRadioColor(wxCommandEvent&);
	void OnChkPartition(wxCommandEvent&);
	void OnRadioShapeKind(wxCommandEvent&);
private:
	wxString m_selectedColor;
	uint8_t m_partition;
	ShapeKind m_shapeKind;
};
