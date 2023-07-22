#include "ReaderSampleListDialog.h"
#include "ShapesDemo.h"
#include <map>
#include <wx/dataview.h>

class ReaderSampleListModel: public wxDataViewModel
{
	enum class ItemKind;
	struct InternalListItem;
	struct InternalWriterGuidItem;
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
	std::vector<InternalListItem*> m_children;
};

enum class ReaderSampleListModel::ItemKind
{
	WRITER_GUID,
	SAMPLE_DATA
};

struct ReaderSampleListModel::InternalListItem
{
	ItemKind kind;
};

struct ReaderSampleListModel::InternalSampleItem: public InternalListItem
{
	eprosima::fastrtps::rtps::GUID_t publications;
	wxString color;
	int x;
	int y;
	int itemSize;
};

struct ReaderSampleListModel::InternalWriterGuidItem: public InternalListItem
{
	eprosima::fastrtps::rtps::GUID_t guid;
};

ReaderSampleListModel::ReaderSampleListModel(const eprosima::fastrtps::rtps::GUID_t& guid)
	: m_guid(guid)
{

}

ReaderSampleListModel::~ReaderSampleListModel()
{
	for(auto& it: m_children)
	{
		delete it;
	}
}

unsigned ReaderSampleListModel::GetChildren(const wxDataViewItem& item, wxDataViewItemArray& children) const
{
	eprosima::fastrtps::rtps::GUID_t publicationGuid;
	auto it = m_children.begin();
	const auto end = m_children.end();
	if(item.IsOk())
	{
		auto listItem = static_cast<InternalListItem*>(item.GetID());
		if(listItem->kind == ItemKind::SAMPLE_DATA)
			return 0;

		publicationGuid = static_cast<InternalWriterGuidItem*>(listItem)->guid;
		it = std::find_if(it, m_children.end(), [publicationGuid](InternalListItem* it)
		{
			if(it->kind == ItemKind::WRITER_GUID)
				return false;

			auto sampleItem = static_cast<InternalSampleItem*>(it);
			if(sampleItem->publications == publicationGuid)
				return true;
		});

		for(; it != end; ++it)
		{
			listItem = *it;
			if(listItem->kind == ItemKind::WRITER_GUID)
				continue;

			auto sampleItem = static_cast<InternalSampleItem*>(listItem);
			if(sampleItem->publications != publicationGuid)
				continue;

			children.push_back(wxDataViewItem{sampleItem});
		}
	}
	else
	{
		for(; it != end; ++it)
		{
			auto listItem = *it;
			if(listItem->kind == ItemKind::SAMPLE_DATA)
				continue;

			children.push_back(wxDataViewItem{listItem});
		}
	}

	return children.size();
}

wxDataViewItem ReaderSampleListModel::GetParent(const wxDataViewItem& item) const
{
	if(!item.IsOk())
		return {};

	const auto listItem = static_cast<InternalListItem*>(item.GetID());
	if(listItem->kind == ItemKind::SAMPLE_DATA)
	{
		const auto sampleItem = static_cast<InternalSampleItem*>(listItem);
		for(auto& it: m_children)
		{
			if(it->kind == ItemKind::SAMPLE_DATA)
				continue;

			const auto writerGuidItem = static_cast<InternalWriterGuidItem*>(it);
			if(writerGuidItem->guid != sampleItem->publications)
				continue;

			return wxDataViewItem{writerGuidItem};
		}
	}

	return {};
}

void ReaderSampleListModel::GetValue(wxVariant& variant, const wxDataViewItem& item, unsigned col) const
{
	if(!item.IsOk())
		return;

	const auto listItem = static_cast<InternalListItem*>(item.GetID());
	switch(listItem->kind)
	{
	case ItemKind::WRITER_GUID:
		{
			auto guidItem = static_cast<InternalWriterGuidItem*>(listItem);
			wxString guidStr;
			for(int i = 0; i < 12; ++i)
			{
				guidStr += wxString::Format(wxS("%02X"), guidItem->guid.guidPrefix.value[i]);
				if(i % 4 == 3)
					guidStr += wxS('-');
			}

			for(int i = 0; i < 4; ++i)
			{
				guidStr += wxString::Format(wxS("%02X"), guidItem->guid.entityId.value[i]);
			}

			variant = guidStr;
		}
		break;
	case ItemKind::SAMPLE_DATA:
		{
			auto sampleItem = static_cast<InternalSampleItem*>(listItem);
			switch(col)
			{
			case 0:
				variant = sampleItem->color;
				break;

			case 1:
				variant = wxString::Format(wxS("%d"), sampleItem->itemSize);
				break;

			case 2:
				variant = wxString::Format(wxS("%d"), sampleItem->x);
				break;

			case 3:
				variant = wxString::Format(wxS("%d"), sampleItem->y);
				break;
			}	
		}
		

		break;
	}
}

bool ReaderSampleListModel::IsContainer(const wxDataViewItem& item) const
{
	if(!item.IsOk())
		return true;

	const auto listItem = static_cast<InternalListItem*>(item.GetID());
	return listItem->kind == ItemKind::WRITER_GUID;
}

void ReaderSampleListModel::Update()
{
	auto& model = wxGetApp().GetModel();
	const std::vector<ReceivedShape>* pReceivedShapes;
	if(auto it = model.receivedShapeTable.find(m_guid); it != model.receivedShapeTable.end())
	{
		pReceivedShapes = &std::get<1>(it->second);
	}
	else
	{
		return;
	}

	// Remove Handle;
	std::vector<InternalWriterGuidItem*> removingItem;
	std::map<eprosima::fastrtps::rtps::GUID_t, std::tuple<InternalWriterGuidItem*, std::vector<InternalSampleItem*>>> numberOfChildrenTable; // old, new

	removingItem.reserve(m_children.size());
	for(auto* item: m_children)
	{
		if(item->kind == ItemKind::SAMPLE_DATA)
			continue;
			

		const auto& writerGuid = static_cast<InternalWriterGuidItem*>(item)->guid;
		auto it = std::find_if(pReceivedShapes->begin(), pReceivedShapes->end(), [writerGuid](const ReceivedShape& shape)
		{
			return shape.publicationGuid == writerGuid;
		});

		if(it != pReceivedShapes->end())
		{
			numberOfChildrenTable.emplace(writerGuid, std::make_tuple(static_cast<InternalWriterGuidItem*>(item), std::vector<InternalSampleItem*>{}));
			continue;
		}

		removingItem.push_back(static_cast<InternalWriterGuidItem*>(item));
	}

	wxDataViewItemArray items;
	for(auto* item: removingItem)
	{
		items.clear();
		wxDataViewItem parent{item};

		auto it = m_children.begin();
		while (it != m_children.end())
		{
			auto pItem = *it;
			bool removed = false;
			if(pItem->kind == ItemKind::WRITER_GUID && static_cast<InternalWriterGuidItem*>(pItem)->guid == item->guid)
			{
				removed = true;
			}
			else if(pItem->kind == ItemKind::SAMPLE_DATA && static_cast<InternalSampleItem*>(pItem)->publications == item->guid)
			{
				removed = true;
				items.push_back(wxDataViewItem{pItem});
			}

			if(removed)
			{
				it = m_children.erase(it);
				delete pItem;
			}
			else
			{
				++it;
			}
		}

		ItemsDeleted(parent, items);
		ItemDeleted({}, parent);
	}

	for(auto* item: m_children)
	{
		if(item->kind == ItemKind::WRITER_GUID)
			continue;

		auto pItem = static_cast<InternalSampleItem*>(item);
		std::get<1>(numberOfChildrenTable[pItem->publications]).push_back(pItem);
	}
	
	removingItem.clear();
	removingItem.shrink_to_fit();
	// Add Items
	items.clear();
	for(auto& shape: *pReceivedShapes)
	{
		InternalWriterGuidItem* parent = nullptr;
		auto it = numberOfChildrenTable.find(shape.publicationGuid);
		if(it == numberOfChildrenTable.end())
		{
			parent = new InternalWriterGuidItem{};
			parent->kind = ItemKind::WRITER_GUID;
			parent->guid = shape.publicationGuid;
			m_children.push_back(parent);
			it = numberOfChildrenTable.emplace(shape.publicationGuid, std::make_tuple(parent, std::vector<InternalSampleItem*>{})).first;
			ItemAdded(wxDataViewItem{}, wxDataViewItem{parent});
		}

		parent = std::get<0>(it->second);
		auto& children = std::get<1>(it->second);
		if(children.empty())
		{
			auto newItem = new InternalSampleItem{};
			newItem->kind = ItemKind::SAMPLE_DATA;
			newItem->color = shape.rgb;
			newItem->x = shape.pos.x;
			newItem->y = shape.pos.y;
			newItem->itemSize = shape.size;
			newItem->publications = shape.publicationGuid;
			m_children.push_back(newItem);
			ItemAdded(wxDataViewItem{parent}, wxDataViewItem{newItem});
		}
		else
		{
			auto item = children.front();
			children.erase(children.begin());
			item->color = shape.rgb;
			item->x = shape.pos.x;
			item->y = shape.pos.y;
			item->itemSize = shape.size;
			items.push_back(wxDataViewItem{item});
		}
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
