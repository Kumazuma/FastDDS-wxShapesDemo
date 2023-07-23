#include "CanvasPanel.h"
#include "ShapesDemo.h"
#include <DirectXMath.h>
#include <wx/dcbuffer.h>

using namespace DirectX;

CanvasPanel::CanvasPanel(wxWindow* parent, wxWindowID winid, const wxPoint& pos, const wxSize& size, long style,
	const wxString& name)
		: wxPanel(parent, winid, pos, size, style, name)
{
	SetBackgroundStyle(wxBG_STYLE_PAINT);
	Bind(wxEVT_PAINT, &CanvasPanel::OnPoint, this);
	Bind(wxEVT_LEFT_DOWN, &CanvasPanel::OnMouseDown, this);
	Bind(wxEVT_LEFT_UP, &CanvasPanel::OnMouseUp, this);
	Bind(wxEVT_MOTION, &CanvasPanel::OnMouseMove, this);
}

inline bool FindBrush(wxBrush& brush, std::unordered_map<wxString, wxBrush>& table, const wxString& str)
{
	auto findIt = table.find(str);
	if(findIt != table.end())
	{
		brush = findIt->second;
	}
	else
	{
		wxColor color;
		if(!wxFromString(str, &color))
		{
			return false;
		}
		
		brush = wxBrush(color);
		table.emplace(str, brush);
	}

	return true;
}

void CanvasPanel::OnPoint(wxPaintEvent&)
{
	wxBufferedPaintDC dc(this);
	dc.Clear();
	dc.SetUserScale(2.f, 2.f);
	auto& app = wxGetApp();
	auto& model = app.GetModel();
	std::unordered_map<wxString, wxBrush> brushTable;
	for(auto& it: model.shapeListTable)
	{
		auto& list = std::get<1>(it.second);
		switch (std::get<0>(it.second))
		{
		case ShapeKind::Circle:
			for(auto& shape: list)
			{
				wxBrush brush;
				if(!FindBrush(brush, brushTable, shape.rgb))
					continue;
				dc.SetBrush(brush);
				dc.DrawCircle((int)shape.pos.x, (int)shape.pos.y, shape.size / 2);
			}

			break;

		case ShapeKind::Square:
			for(auto& shape: list)
			{
				wxBrush brush;
				if(!FindBrush(brush, brushTable, shape.rgb))
					continue;
				dc.SetBrush(brush);
				dc.DrawRectangle((int)shape.pos.x - shape.size / 2, (int)shape.pos.y - shape.size / 2, shape.size, shape.size);

			}
			break;

		case ShapeKind::Triangle:
			for(auto& shape: list)
			{
				wxBrush brush;
				if(!FindBrush(brush, brushTable, shape.rgb))
					continue;
				dc.SetBrush(brush);
				wxPoint points[] {
					{0, shape.size / -2},
					{shape.size / -2, shape.size / 2},
					{shape.size / 2, shape.size / 2}
				};

				dc.DrawPolygon(3, points, (int)shape.pos.x, (int)shape.pos.y);

			}
			break;
		}
	}

	
	wxPen pen(wxColor(0, 0, 0), 3);
	for(auto& it: model.sendingShapes)
	{
		auto& shape = std::get<1>(it.second);
		wxBrush brush;
		if(!FindBrush(brush, brushTable, shape.rgb))
			continue;

		dc.SetBrush(brush);
		switch(std::get<0>(it.second))
		{
		case ShapeKind::Circle:
			dc.DrawCircle((int)shape.pos.x, (int)shape.pos.y, shape.size / 2);
			break;

		case ShapeKind::Square:
			dc.DrawRectangle((int)shape.pos.x - shape.size / 2, (int)shape.pos.y - shape.size / 2, shape.size, shape.size);
			break;

		case ShapeKind::Triangle:
			{
				wxPoint points[] {
					{0, shape.size / -2},
					{shape.size / -2, shape.size / 2},
					{shape.size / 2, shape.size / 2}
				};

				dc.DrawPolygon(3, points, (int)shape.pos.x, (int)shape.pos.y);
			}
			break;
		}
	}
}

void CanvasPanel::OnMouseDown(wxMouseEvent& event)
{
	auto& shapeTable = wxGetApp().GetModel().sendingShapes;
	auto mousePosition = event.GetPosition() / 2;
	m_origin = mousePosition;
	for(auto it = shapeTable.rbegin(); it != shapeTable.rend(); ++it)
	{
		auto& tuple = it->second;
		auto& shape = std::get<1>(tuple);
		bool isGrab = false;
		switch (std::get<0>(tuple))
		{
		case ShapeKind::Circle:
			{
				int dx = mousePosition.x - (int)shape.pos.x;
				int dy = mousePosition.y - (int)shape.pos.y;
				dx = dx * dx;
				dy = dy * dy;
				isGrab = !(dx + dy > (shape.size * shape.size) / 4);
			}
			break;
		case ShapeKind::Triangle:
			{
				
				const auto size = shape.size;
				XMINT2 points[3]
				{
					{(int)shape.pos.x + 0, (int)shape.pos.y - size / 2},
					{(int)shape.pos.x - shape.size / 2, (int)shape.pos.y + size / 2},
					{(int)shape.pos.x + shape.size / 2, (int)shape.pos.y + size / 2},
				};

				constexpr auto A = 0;
				constexpr auto B = 1;
				constexpr auto C = 2;
				// Vectors
				XMVECTOR vectors[3]
				{
					XMLoadSInt2(points + B) - XMLoadSInt2(points + A),
					XMLoadSInt2(points + C) - XMLoadSInt2(points + B),
					XMLoadSInt2(points + A) - XMLoadSInt2(points + C),
				};

				// Normalize
				vectors[0] = XMVector2Normalize(vectors[0]);
				vectors[1] = XMVector2Normalize(vectors[1]);
				vectors[2] = XMVector2Normalize(vectors[2]);

				// to otho
				vectors[0] = XMVectorSwizzle(vectors[0], 1, 0, 2, 3);
				vectors[1] = XMVectorSwizzle(vectors[1], 1, 0, 2, 3);
				vectors[2] = XMVectorSwizzle(vectors[2], 1, 0, 2, 3);

				vectors[0] = XMVectorMultiply(vectors[0], XMVectorSet(-1.f, 1.f, 0.f, 0.f));
				vectors[1] = XMVectorMultiply(vectors[1], XMVectorSet(-1.f, 1.f, 0.f, 0.f));
				vectors[2] = XMVectorMultiply(vectors[2], XMVectorSet(-1.f, 1.f, 0.f, 0.f));

				XMVECTOR pos = XMVectorSet(mousePosition.x, mousePosition.y, 0.f, 0.f);
				XMVECTOR ret[3]
				{
					XMVector2Dot(vectors[0], pos - XMLoadSInt2(points + A)),
					XMVector2Dot(vectors[1], pos - XMLoadSInt2(points + B)),
					XMVector2Dot(vectors[2], pos - XMLoadSInt2(points + C))
				};

				XMVECTOR r = XMVectorSelect(ret[0], ret[1], XMVectorSelectControl(0, 1, 0, 0));
				r = XMVectorSelect(r, ret[2], XMVectorSelectControl(0, 0, 1, 0));
				isGrab = XMVector3LessOrEqual(r, XMVectorSet(0, 0, 0, 0));
			}
			break;
		case ShapeKind::Square:
			{
				wxRect rc((int)shape.pos.x - shape.size / 2, (int)shape.pos.y - shape.size / 2, shape.size, shape.size);
				isGrab = rc.Contains(mousePosition);
			}
			
			break;
		}


		if(!isGrab)
			continue;

		auto& model = wxGetApp().GetModel();
		model.grabbedShapeGuid = it->first;
		m_basePosition = shape.pos;
		break;
	}
}

void CanvasPanel::OnMouseUp(wxMouseEvent& event)
{
	auto& model = wxGetApp().GetModel();
	auto& shapeTable = model.sendingShapes;
	auto mousePosition = event.GetPosition() / 2;
	auto grabbedShapeGuid = model.grabbedShapeGuid;
	model.grabbedShapeGuid = {};
	auto it = shapeTable.find(grabbedShapeGuid);
	if(it != shapeTable.end())
	{
		auto& shape = std::get<1>(it->second);
		int diffX = mousePosition.x - m_origin.x;
		int diffY = mousePosition.y - m_origin.y;
		XMVECTOR basePosition = XMLoadFloat2(&m_basePosition);
		XMVECTOR pos = XMVectorAdd(basePosition, XMVectorSet(diffX, diffY, 0.f, 0.f));
		XMStoreFloat2(&shape.pos, pos);
	}
	
}

void CanvasPanel::OnMouseMove(wxMouseEvent& event)
{
	auto& model = wxGetApp().GetModel();
	auto& grabbedShapeGhid = model.grabbedShapeGuid;
	if(!event.LeftIsDown() || grabbedShapeGhid == eprosima::fastrtps::rtps::GUID_t::unknown())
		return;

	auto& shapeTable = model.sendingShapes;
	auto mousePosition = event.GetPosition() / 2;
	auto it = shapeTable.find(grabbedShapeGhid);
	if(it == shapeTable.end())
	{
		grabbedShapeGhid = eprosima::fastrtps::rtps::GUID_t::unknown();
		return;
	}

	auto& shape = std::get<1>(it->second);
	int diffX = mousePosition.x - m_origin.x;
	int diffY = mousePosition.y - m_origin.y;
	XMVECTOR basePosition = XMLoadFloat2(&m_basePosition);
	XMVECTOR pos = XMVectorAdd(basePosition, XMVectorSet(diffX, diffY, 0.f, 0.f));
	XMStoreFloat2(&shape.pos, pos);
}
