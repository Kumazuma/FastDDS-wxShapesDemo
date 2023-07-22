#pragma once
#include "ReaderSampleListDialogBase.h"
#include <fastdds/rtps/common/Guid.h>

class ReaderSampleListModel;
class ReaderSampleListDialog: public ReaderSampleListDialogBase
{
public:
	ReaderSampleListDialog(wxWindow* parent, wxWindowID id, const eprosima::fastrtps::rtps::GUID_t& guid);
	~ReaderSampleListDialog();
	void UpdateList();

private:
	eprosima::fastrtps::rtps::GUID_t m_guid;
	ReaderSampleListModel* m_pViewModel;
};
