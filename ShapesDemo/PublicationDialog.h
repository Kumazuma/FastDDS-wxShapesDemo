#pragma once
#include "PublicationDialogBase.h"
#include <fastdds/dds/core/policy/QosPolicies.hpp>
using ReliabilityQosPolicyKind = eprosima::fastdds::dds::ReliabilityQosPolicyKind;
enum class ShapeKind;
class PublicationDialog: public PublicationDialogBase
{
	DECLARE_EVENT_TABLE();
public:
	PublicationDialog(wxWindow* parent);
	wxString GetColor() const;
	ShapeKind GetShapeKind() const;
	uint8_t GetPartition() const;
	ReliabilityQosPolicyKind GetReliabilityKind() const;
protected:
	void OnRadioColor(wxCommandEvent&);
	void OnChkPartition(wxCommandEvent&);
	void OnRadioShapeKind(wxCommandEvent&);
	void OnRadioReliability(wxCommandEvent&);
private:
	wxString m_selectedColor;
	uint8_t m_partition;
	ShapeKind m_shapeKind;
	ReliabilityQosPolicyKind m_reliabilityKind;
};
