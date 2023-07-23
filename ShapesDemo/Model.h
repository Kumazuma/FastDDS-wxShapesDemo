#pragma once
#include <wx/wx.h>
#include <vector>
#include <memory>
#include <fastdds/dds/common/InstanceHandle.hpp>
#include <fastdds/rtps/common/InstanceHandle.h>
#include <unordered_map>
#include <map>
#include <DirectXMath.h>

enum class ShapeKind
{
	Circle,
	Triangle,
	Square
};

struct Shape
{
	wxString rgb;
	int size;
	DirectX::XMFLOAT2 pos;
};

struct SendingShape: public Shape
{
	DirectX::XMFLOAT2 power;
};

struct ReceivedShape: public Shape
{
	eprosima::fastrtps::rtps::GUID_t publicationGuid;
};

struct EntityInfo
{
	wxString entityType;
	eprosima::fastrtps::rtps::GUID_t guid;
	wxString topic;
	wxString color;
	wxString reliability;
};

class Model
{
public:
	std::map<eprosima::fastrtps::rtps::GUID_t, std::tuple<ShapeKind, std::vector<ReceivedShape>>> shapeListTable;
	std::map<eprosima::fastrtps::rtps::GUID_t, std::vector<ReceivedShape>> receivedShapeListTable;
	std::map<eprosima::fastrtps::rtps::GUID_t, std::tuple<ShapeKind, SendingShape>> sendingShapes;
	std::map<eprosima::fastrtps::rtps::GUID_t, wxRect> contentFilterAreaRects;
	std::map<eprosima::fastrtps::rtps::GUID_t, EntityInfo> entityInfoTable;
	eprosima::fastrtps::rtps::GUID_t grabbedShapeGuid;
	DirectX::XMFLOAT2 prevPosition;
};
