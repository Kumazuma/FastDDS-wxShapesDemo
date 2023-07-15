#pragma once
#include <wx/wx.h>
#include <fastdds/rtps/common/Guid.h>
#include <DirectXMath.h>

class CanvasPanel: public wxPanel
{
public:
	CanvasPanel(wxWindow *parent,
		wxWindowID winid = wxID_ANY,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxTAB_TRAVERSAL | wxNO_BORDER,
		const wxString& name = wxASCII_STR(wxPanelNameStr));

protected:
	void OnPoint(wxPaintEvent&);
	void OnMouseDown(wxMouseEvent&);
	void OnMouseUp(wxMouseEvent&);
	void OnMouseMove(wxMouseEvent&);

private:
	wxPoint m_origin;
	DirectX::XMFLOAT2 m_basePosition;
	DirectX::XMFLOAT2 m_prevPosition;
};
