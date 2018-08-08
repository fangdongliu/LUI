#include "stdafx.h"
#include "UIDragable.h"
#include"Window.h"
#include"Page.h"

using namespace LUI;

UIDragable::UIDragable()
{
	capturable = true;
	lastTime = -1;
	lastP = { -1,-1 };
}


UIDragable::~UIDragable()
{
}

bool UIDragable::OnLButtonDown(float x, float y, Window*window) {

	ReleaseCapture();

	float cur = globalTimer.TotalTime();

	if (lastP.x == x && lastP.y == y && cur - lastTime < 0.3f) {
		window->ToggleSize();
		lastP = { -1,-1 };		
	}
	else {
		lastTime = cur;
		lastP = { x,y };
		window->SetCaptureElement(nullptr);
		SendMessage(window->GetHWnd(), WM_NCLBUTTONDOWN, HTCAPTION, 0);

	}
	return true;

}

UIElement* UIDragable::CopyData() {
	return new UIDragable;
}