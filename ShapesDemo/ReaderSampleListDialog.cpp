#include "ReaderSampleListDialog.h"
#include "ShapesDemo.h"
#include <map>
#include <wx/dataview.h>

class ReaderSampleListModel: public wxDataViewModel
{
	enum class ItemKind;
	struct InternalSampleItem;
public:
	ReaderSampleListModel(const eprosima::fastrtps::rtps::GUID_t& guid);
	~ReaderSampleListModel() override;
	unsigned GetChildren(const wxDataViewItem& item, wxDataViewItemArray& children) const override;
	wxDataViewItem GetParent(const wxDataViewItem& item) const override;
	void GetValue(wxVariant& variant, const wxDataViewItem& item, unsigned col) const override;
	bool IsContainer(const wxDataViewItem& item) const override;
	bool SetValue(const wxVariant& variant, const wxDataViewItem& item, unsigned col) override {return false;}
	void Update();
private:
	const eprosima::fastrtps::rtps::GUID_t m_guid;
	std::vector<InternalSampleItem*> m_table;
};

struct ReaderSampleListModel::InternalSampleItem
{
	eprosima::fastrtps::rtps::GUID_t guid;
	wxString color;
	int x;
	int y;
	int itemSize;
};

ReaderSampleListModel::ReaderSampleListModel(const eprosima::fastrtps::rtps::GUID_t& guid)
	: m_guid(guid)
{

}

ReaderSampleListModel::~ReaderSampleListModel()
{
	for(auto item : m_table)
	{
		delete item;
	}
}

unsigned ReaderSampleListModel::GetChildren(const wxDataViewItem& item, wxDataViewItemArray& children) const
{
	eprosima::fastrtps::rtps::GUID_t publicationGuid;
	if(item.IsOk())
	{
		return 0;
	}
	
	for(auto it: m_table)
	{
		children.push_back(wxDataViewItem{it});
	}

	return children.size();
}

wxDataViewItem ReaderSampleListModel::GetParent(const wxDataViewItem& item) const
{
	return {};
}

void ReaderSampleListModel::GetValue(wxVariant& variant, const wxDataViewItem& item, unsigned col) const
{
	if(!item.IsOk())
		return;

	auto sampleItem = static_cast<InternalSampleItem*>(item.GetID());
	switch(col)
	{
	case 0:
		{
			wxString guidStr;
			for(int i = 0; i < 12; ++i)
			{
				guidStr += wxString::Format(wxS("%02X"), sampleItem->guid.guidPrefix.value[i]);
				if(i % 4 == 3)
					guidStr += wxS('-');
			}

			for(int i = 0; i < 4; ++i)
			{
				guidStr += wxString::Format(wxS("%02X"), sampleItem->guid.entityId.value[i]);
			}

			variant = guidStr;
		}
		break;
	case 1:
		variant = sampleItem->color;
		break;

	case 2:
		variant = wxString::Format(wxS("%d"), sampleItem->itemSize);
		break;

	case 3:
		variant = wxString::Format(wxS("%d"), sampleItem->x);
		break;

	case 4:
		variant = wxString::Format(wxS("%d"), sampleItem->y);
		break;
	}
}

bool ReaderSampleListModel::IsContainer(const wxDataViewItem& item) const
{
	return !item.IsOk();
}

void ReaderSampleListModel::Update()
{
	auto& model = wxGetApp().GetModel();
	const std::vector<ReceivedShape>* pReceivedShapes;
	if(auto it = model.shapeListTable.find(m_guid); it != model.shapeListTable.end())
	{
		pReceivedShapes = &std::get<1>(it->second);
	}
	else
	{
		return;
	}

	wxDataViewItemArray items;
	const auto minCount = std::min(m_table.size(), pReceivedShapes->size());
	if(m_table.size() > pReceivedShapes->size())
	{
		auto it = m_table.begin() + pReceivedShapes->size();
		while(it != m_table.end())
		{
			auto item = *it;
			delete item;
			items.push_back(wxDataViewItem{item});
			it = m_table.erase(it);
		}

		ItemsDeleted({}, items);
		items.clear();
	}
	else if(m_table.size() < pReceivedShapes->size())
	{
		auto it = pReceivedShapes->begin() + m_table.size();
		auto end = pReceivedShapes->end();
		for(; it != end; ++it)
		{
			const auto& shape = *it;
			auto newItem = new InternalSampleItem{};
			newItem->color = shape.rgb;
			newItem->x = shape.pos.x;
			newItem->y = shape.pos.y;
			newItem->itemSize = shape.size;
			newItem->guid = shape.publicationGuid;
			m_table.push_back(newItem);
			items.push_back(wxDataViewItem{newItem});
		}

		ItemsAdded({}, items);
		items.clear();
	}

	for(auto i = 0; i < minCount; ++i)
	{
		auto* item = m_table[i];
		auto& shape = (*pReceivedShapes)[i];
		item->color = shape.rgb;
		item->x = shape.pos.x;
		item->y = shape.pos.y;
		item->itemSize = shape.size;
		item->guid = shape.publicationGuid;
		items.push_back(wxDataViewItem{item});
	}

	ItemsChanged(items);
}

ReaderSampleListDialog::ReaderSampleListDialog(wxWindow* parent, wxWindowID id,
                                               const eprosima::fastrtps::rtps::GUID_t& guid)
		: ReaderSampleListDialogBase(parent, id)
, m_guid(guid)
, m_pViewModel(new ReaderSampleListModel(guid))
{
	m_dataViewCtrl->AssociateModel(m_pViewModel);
}

ReaderSampleListDialog::~ReaderSampleListDialog()
{
	m_pViewModel->DecRef();
}

void ReaderSampleListDialog::UpdateList()
{
	m_dataViewCtrl->Freeze();
	m_pViewModel->Update();
	m_dataViewCtrl->Thaw();
}
