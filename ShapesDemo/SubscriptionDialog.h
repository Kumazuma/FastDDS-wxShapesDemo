#pragma once
#include "SubscriptionDialogBase.h"
#include <fastdds/dds/core/policy/QosPolicies.hpp>

enum class ShapeKind;
using ReliabilityQosPolicyKind = eprosima::fastdds::dds::ReliabilityQosPolicyKind;

class SubscriptionDialog: public SubscriptionDialogBase
{
	DECLARE_EVENT_TABLE()
public:
	SubscriptionDialog(wxWindow* parent);
	ShapeKind GetShapeKind() const;
	uint8_t GetPartition() const;
	ReliabilityQosPolicyKind GetReliabilityKind() const;
	bool GetUseTakeMethod() const;
protected:
	void OnChkPartition(wxCommandEvent&);
	void OnRadioShapeKind(wxCommandEvent&);
	void OnRadioReliability(wxCommandEvent&);
	void OnRadioGetMethod(wxCommandEvent&);
private:
	wxString m_selectedColor;
	uint8_t m_partition;
	ShapeKind m_shapeKind;
	ReliabilityQosPolicyKind  m_reliabilityKind;
	bool m_useTakeMethod;
};
