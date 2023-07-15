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

class Model
{
public:
	std::map<eprosima::fastrtps::rtps::GUID_t, std::tuple<ShapeKind, std::vector<ReceivedShape>>> receivedShapeTable;
	std::map<eprosima::fastrtps::rtps::GUID_t, std::tuple<ShapeKind, SendingShape>> sendingShapes;
	eprosima::fastrtps::rtps::GUID_t grabbedShapeGuid;
	DirectX::XMFLOAT2 prevPosition;
};
