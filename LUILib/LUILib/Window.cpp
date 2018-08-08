#include "stdafx.h"
#include "Window.h"
#include"UIElement.h"
#include"App.h"
#include"Page.h"
#include"UIText.h"
#include<iostream>
using namespace LUI;
using namespace std;
using namespace D2D1;

#define OtherValid(v) other.flag&(1<<(int)v)
#define UPDATEIF(f) if(f)InvalidateRect(hWnd,0,0);

#define IDT_CARETUPDATE 1

auto hand = LoadCursor(0, IDC_ARROW);

namespace {
	App *app = GetApp();

	HCURSOR cHand = LoadCursor(0, IDC_HAND);
	HCURSOR cArrow = LoadCursor(0, IDC_ARROW);
	HCURSOR cText = LoadCursor(0, IDC_IBEAM);
	HCURSOR cWait = LoadCursor(0, IDC_WAIT);
}

bool Window::Register() {

	WNDCLASSW wc = { 0 };
	wc.hInstance = HINST_THISCOMPONET;
	wc.lpszClassName = L"fdwindow1";
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.cbWndExtra = sizeof(void*);
	wc.lpfnWndProc = [](HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)->LRESULT {
		Window *window = static_cast<Window*>(reinterpret_cast<void*>(GetWindowLongPtr(hWnd, 0)));
		if (window) {
			return window->MessageProc(message, wParam, lParam);
		}
		else {
			if (message == WM_CREATE) {
				LPCREATESTRUCT cs = reinterpret_cast<LPCREATESTRUCT>(lParam);
				window = static_cast<Window*>(cs->lpCreateParams);

				SetWindowLongPtr(hWnd, 0, reinterpret_cast<LONG_PTR>(window));
				window->hWnd = hWnd;
				return window->MessageProc(message, wParam, lParam);
			}
			else return DefWindowProc(hWnd, message, wParam, lParam);
		}
	};

	if (!RegisterClassW(&wc))
		return false;

	wc.lpszClassName = L"fdwindow2";
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_DROPSHADOW;

	if (!RegisterClassW(&wc))
		return false;



}

Window::Window(WindowSetting&setting)
{
	painter.window = this;
	state = WindowState::normal;
	lastClickTime = -1;
	startTime = 0;
	resizeable = setting.resizable;
	hint = nullptr;
	isDeleted = false;
	page = GetApp()->GetPage(setting.pageName);

	minWidth = setting.minWidth;
	minHeight = setting.minHeight;

	capture = nullptr;
	hover = nullptr;
	focus = nullptr;

	if (!setting.parent) {
		parent = nullptr;
		GetApp()->AddWindow(this);
	}
	else
		setting.parent->AddChild(this);

	if (setting.overlap) {
		CreateWindowW(L"fdwindow1", setting.windowText.c_str(),
			WS_OVERLAPPEDWINDOW, setting.x, setting.y, setting.width, setting.height, 0, 0, HINST_THISCOMPONET, this);
	}
	else {
		CreateWindowW(L"fdwindow2", setting.windowText.c_str(), WS_POPUP, setting.x, setting.y, setting.width, setting.height, 0, 0, HINST_THISCOMPONET, this);

	}




}

void Window::MapOnClick(std::string&&s, std::function<void(Window *)>func) {
	onclick[s] = func;
}

void Window::MapOnChar(std::string&&s, std::function<void(Window *,UINT32)>func) {
	onchar[s] = func;
}

void Window::MapOnKey(std::string&&s, std::function<void(Window *,KeyEvent&)>func) {
	onkey[s] = func;
}

void Window::Center() {
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	MoveWindow(hWnd, (screenWidth - width) / 2, (screenHeight - height) / 2, width, height, 0);
}

Window::~Window() {

	isDeleted = true;


	if (hWnd)
		DestroyWindow(hWnd);


	if (parent == nullptr)
		GetApp()->EraseWindow(this);
	else
		parent->childs.erase(this);

}

void Window::AddChild(Window*child) {

	child->parent = this;
	childs.insert(child);

}

void Window::Close() {
	PostMessage(hWnd, WM_SYSMESSAGE, (WPARAM)SysMessageType::close, 0);
}

void Window::Minimize() {

	ShowWindow(hWnd, SW_SHOWMINIMIZED);

}

void Window::Show() {

	::ShowWindow(hWnd, SW_SHOW);
	::UpdateWindow(hWnd);

	for (auto i : childs) {
		i->Show();
	}

}

//void Window::Painter::Paint() {
//	auto root = window->root;
//	auto &rect = window->rect;
//	target->BeginDraw();
//	target->Clear(D2D1::ColorF(0xffaaff, 1.0f));
//	target->SetTransform(D2D1::Matrix3x2F::Identity());
//	if (root.needMeasure) {
//		for (auto i : root.childs) {
//
//		}
//	}
//	if (root) {
//		if (root->needCaculate) {
//			///.....
//		}
//		if (root->needMeasure) {
//			root->CaculateSize(rect.right-rect.left, rect.bottom-rect.top);
//			root->OnMeasure();
//		}
//		root->OnPaint();
//	}
//	target->EndDraw();
//}
//
//void Window::Painter::OnPaint() {
//	static TimeMeasurer m;
//	static int count = 0;
//	HRESULT hr = S_OK;
//	hr = CreateDeviceResource();
//	if (SUCCEEDED(hr)) {
//		if (count == 0) {
//			m.Start();
//		}
//		count++;
//		if (count == 60) {
//			printf("frame %lf\n", m.End());
//			count = 0;
//		}
//		if (window->setting.isTransparent) {
//			auto& rect = window->rect;
//			SIZE size = { rect.right - rect.left, rect.bottom - rect.top };
//			BLENDFUNCTION _Blend = { AC_SRC_OVER,0,255,AC_SRC_ALPHA };
//			PAINTSTRUCT ps;
//			BeginPaint(window->hWnd, &ps);
//			HDC mdc = CreateCompatibleDC(ps.hdc);
//			HBITMAP hbitmap = CreateCompatibleBitmap(ps.hdc, size.cx, size.cy);
//			SelectObject(mdc, hbitmap);
//			RECT rc = { 0,0, size.cx,size.cy };
//			dcRenderTarget->BindDC(mdc, &rc);
//			Paint();
//			DWORD dwExStyle = GetWindowLong(window->hWnd, GWL_EXSTYLE);
//			if (!(dwExStyle & 0x80000))
//				SetWindowLong(window->hWnd, GWL_EXSTYLE, dwExStyle ^ 0x80000);
//			POINT p = { 0,0 };
//
//			UpdateLayeredWindow(window->hWnd, ps.hdc, nullptr, &size, mdc, &p, RGB(0, 0, 0), &_Blend, ULW_ALPHA);
//			DeleteDC(mdc);
//			DeleteObject(hbitmap);
//			EndPaint(window->hWnd, &ps);
//		}
//		else {
//			Paint();
//			ValidateRect(window->hWnd, 0);
//		}
//	}
//	if (hr == D2DERR_RECREATE_TARGET) {
//		DiscardDeviceResource();
//	}
//}

//void Window::OnResize(WPARAM wp, short width, short height) {
//	switch (wp) {
//	case SIZE_MINIMIZED:
//		break;
//	case SIZE_MAXIMIZED:
//		break;
//	case SIZE_RESTORED:
//		break;
//	}
//	rect.top = 0;
//	rect.left = 0;
//	rect.right = width;
//	rect.bottom = height;
//	//	root.rect.right = width;
//	//	root.rect.bottom = height;
//}

bool Window::ShowHint(UIElement*e) {


	if (!e || !e->hint.length())
		return false;

	startTime = globalTimer.TotalTime();

	IDWriteTextFormat*textFormat = nullptr;

	UIText::factory->CreateTextFormat(L"", 0,
		DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 16.0f, L"", &textFormat);

	SafeRelease(&hint);

	UIText::factory->CreateTextLayout(
		e->hint.c_str(),
		static_cast<UINT32>(e->hint.length()),
		textFormat,
		(float)(rect.right - rect.left - 10), (float)(rect.bottom - rect.top - 4),
		&hint
	);


	if (hint) {
		hint->SetTextAlignment(DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_LEADING);
		hint->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
		hint->SetWordWrapping(DWRITE_WORD_WRAPPING_WHOLE_WORD);
	}
	SafeRelease<IDWriteTextFormat>(&textFormat);


	return true;
}

void Window::Maxmize() {

	ShowWindow(hWnd, SW_SHOWMAXIMIZED);

}

LRESULT Window::MessageProc(UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_KEYDOWN:
		globalTimer.Tick();
		globalTimer.mLastEventTime = globalTimer.TotalTime();
		UPDATEIF(OnKey((int)wp));

		return 0;
	case WM_CHAR:
		globalTimer.Tick();
		globalTimer.mLastEventTime = globalTimer.TotalTime();
		UPDATEIF(OnChar((wchar_t)wp));
		return 0;
	case WM_MOUSELEAVE:
		globalTimer.Tick();
		UPDATEIF(SetHoverElement(nullptr));
		return 0;
	case WM_MOUSEWHEEL:
		globalTimer.Tick();
		UPDATEIF(OnScroll((short)HIWORD(wp)));
		return 0;
	case WM_MOUSEHWHEEL:
		globalTimer.Tick();
		UPDATEIF(OnHScroll((short)HIWORD(wp)));
		return 0;
	case WM_MOUSEMOVE:
		globalTimer.Tick();
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.dwFlags = TME_LEAVE | TME_HOVER;
		tme.hwndTrack = hWnd;
		tme.dwHoverTime = 100;

		{
			UIElement* lastHover = hover;
			UPDATEIF(OnMouseMove((float)(short)LOWORD(lp), (float)(short)HIWORD(lp)));
			UIElement* currHover = hover;
			if (currHover&&lastHover != currHover) {
				if (lastHover) {
					HideHint();
				}
			}

			TrackMouseEvent(&tme);
		}
		return 0;
	case WM_MOUSEHOVER:
		globalTimer.Tick();
		hintPos.x = LOWORD(lp);
		hintPos.y = (float)(HIWORD(lp) + 20);
		UPDATEIF(ShowHint(hover));
		return 0;
	case WM_LBUTTONDOWN:
		globalTimer.Tick();
		globalTimer.mLastEventTime = globalTimer.TotalTime();
		SetFocus(hWnd);
		SetCapture(hWnd);
		UPDATEIF(OnLBtnDown((float)LOWORD(lp), (float)HIWORD(lp)));
		return 0;
	case WM_LBUTTONUP:
		globalTimer.Tick();
		globalTimer.mLastEventTime = globalTimer.TotalTime();
		ReleaseCapture();
		UPDATEIF(OnLBtnUp((float)LOWORD(lp), (float)HIWORD(lp)));
		return 0;
	case WM_NCHITTEST:
	{
		const LONG border_width = 5; //in pixels
		RECT winrect;
		GetWindowRect(hWnd, &winrect);

		long x = (short)LOWORD(lp);
		long y = (short)HIWORD(lp);
		if (x >= winrect.left && x < winrect.left + border_width &&
			y < winrect.bottom && y >= winrect.bottom - border_width)
			return HTBOTTOMLEFT;
		else if (x < winrect.right && x >= winrect.right - border_width &&
			y < winrect.bottom && y >= winrect.bottom - border_width)
			return HTBOTTOMRIGHT;
		else if (x >= winrect.left && x < winrect.left + border_width &&
			y >= winrect.top && y < winrect.top + border_width)
			return HTTOPLEFT;
		else if (x < winrect.right && x >= winrect.right - border_width &&
			y >= winrect.top && y < winrect.top + border_width)
			return HTTOPRIGHT;
		else if (x >= winrect.left && x < winrect.left + border_width)
			return HTLEFT;
		else if (x < winrect.right && x >= winrect.right - border_width)
			return HTRIGHT;
		else if (y < winrect.bottom && y >= winrect.bottom - border_width)
			return HTBOTTOM;
		else if (y >= winrect.top && y < winrect.top + border_width)
			return HTTOP;

		else return HTCLIENT;
	}
	case WM_RBUTTONDOWN:
		globalTimer.Tick();
		SetFocus(hWnd);
		SetCapture(hWnd);
		UPDATEIF(OnRBtnDown((float)LOWORD(lp), (float)HIWORD(lp)));
		return 0;
	case WM_RBUTTONUP:
		globalTimer.Tick();
		ReleaseCapture();
		UPDATEIF(OnRBtnUp((float)LOWORD(lp), (float)HIWORD(lp)));
		return 0;
	case WM_KILLFOCUS:
		globalTimer.Tick();
		KillTimer(hWnd, IDT_CARETUPDATE);
		OnKillFocus();
		return 0;
	case WM_PAINT:
	{
		HRESULT hr = S_OK;
		hr = painter.CreateDeviceResource(this);
		if (hr == S_OK) {

			painter.target->BeginDraw();

			painter.target->Clear(D2D1::ColorF(1, 1, 1, 1));

			painter.target->SetTransform(D2D1::Matrix3x2F::Identity());

			if (page&&page->root) {

				globalTimer.Tick();

				painter.rect.X = 0;
				painter.rect.Y = 0;
				painter.rect.Width = (float)(rect.right - rect.left);
				painter.rect.Height = (float)(rect.bottom - rect.top);

				if (page->root->needMeasure)
					page->root->OnMeasure(painter.rect.Width, painter.rect.Height);


				painter.rect.Inflate(10, 10);

				bool b = painter.Paint(page->root);

				b |= painter.PaintHint(hint, globalTimer.TotalTime() - startTime, hintPos);

				if (!b)
					ValidateRect(hWnd, 0);

				if (hint&&globalTimer.TotalTime() - startTime > 3.3f) {
					SafeRelease(&hint);
				}
			}
			else
				ValidateRect(hWnd, 0);

			painter.target->EndDraw();

		}
		else if (hr == D2DERR_RECREATE_TARGET)
			painter.Release();
	}
	return 0;

	case WM_SETFOCUS:
		SetTimer(hWnd, IDT_CARETUPDATE, 200, 0);
		OnFocus();
		return 0;
	case WM_TIMER:
		if (wp == IDT_CARETUPDATE) {
			Invalidate();
		}
		else OnTimer();
		return 0;

	case WM_SIZE:

		if (wp == SIZE_MINIMIZED) {
			state = WindowState::minimize;
			return 0;
		}
		if (page&&page->root)
			page->root->needMeasure = true;
		if (wp == SIZE_MAXIMIZED) {
			state = WindowState::maxmize;
		}
		else {
			state = WindowState::normal;
		}

		OnResize(wp == SIZE_MAXIMIZED, (short)LOWORD(lp), (short)HIWORD(lp));

		GetClientRect(hWnd, &rect);
		if (painter.target) {
			painter.target->Resize(D2D1::SizeU(rect.right - rect.left, rect.bottom - rect.top));
		}
		Invalidate();

		return 0;

	case WM_DESTROY:

		KillTimer(hWnd, IDT_CARETUPDATE);

		for (auto i = childs.begin(); i != childs.end();)
			delete *i++;

		painter.Release();

		if (!isDeleted)
			delete this;

		return 0;
	case WM_USERMESSAGE:
		OnMessage((UINT)wp, (void*)lp);
		return 0;
	case WM_SYSMESSAGE:
		OnSysMessage(wp, lp);
		return 0;
	case WM_GETMINMAXINFO:
	{
		auto p = (MINMAXINFO*)lp;
		p->ptMinTrackSize.x = minWidth;
		p->ptMinTrackSize.y = minHeight;
		return 0;
	}
	}

	return DefWindowProc(hWnd, msg, wp, lp);
}

void Window::OnSysMessage(WPARAM wp, LPARAM lp) {

	switch (SysMessageType(wp))
	{
	case LUI::SysMessageType::close:
		delete this;
		break;
	default:
		break;
	}

}


bool Window::HideHint() {
	if (hint) {
		SafeRelease(&hint);
		return true;
	}
	return false;
}


bool Window::OnKey(int vk) {

	bool heldShift = (GetKeyState(VK_SHIFT) & 0x80) != 0;
	bool heldControl = (GetKeyState(VK_CONTROL) & 0x80) != 0;
	bool heldAlt = (GetKeyState(VK_MENU) & 0x80) != 0;

	KeyEvent keyEvent(vk, heldShift, heldControl, heldAlt);

	if (focus) {
		auto e = focus;
		bool b=e->OnKey(keyEvent, this);
		if (e->onkey.length()) {
			try
			{
				(onkey.at(e->onkey))(this,keyEvent);
			}
			catch (const std::exception&)
			{

			}
		}
		return b;
	}


	return false;
}

bool Window::OnChar(wchar_t c) {

	if (focus) {
		auto e = focus;
		bool b= e->OnChar(c, this);
		if (e->onchar.length()) {
			try
			{
				(onchar.at(e->onchar))(this,c);
			}
			catch (const std::exception&)
			{

			}
		}
		return b;
	}
	return false;
}

bool Window::SetCaptureElement(UIElement*e) {

	if (e != capture) {

		if (capture) {

			capture->isCapture = false;

			Display ds;


			if (capture->isFocus) {
				if (app->BuildFocusDisplay(capture->name, ds) | app->BuildFocusDisplay(capture->displayClass, ds))
					MakeAnim(capture, ds);
			}
			else if (capture->isHover) {
				if (app->BuildHoverDisplay(capture->name, ds) | app->BuildHoverDisplay(capture->displayClass, ds))
					MakeAnim(capture, ds);
			}
			else
				if (app->BuildNormalDisplay(capture->name, ds) | app->BuildNormalDisplay(capture->displayClass, ds))
					MakeAnim(capture, ds);


		}

		if (e) {

			e->isCapture = true;

			Display ds;

			if (app->BuildCaptureDisplay(e->name, ds) | app->BuildCaptureDisplay(e->displayClass, ds)) {
				MakeAnim(e, ds);
			}

		}

		capture = e;

		return true;

	}

	return false;

}

bool Window::SetFocusElement(UIElement*e) {


	if (focus != e) {
		if (focus) {
			focus->isFocus = false;
			Display ds;

			if (!focus->isCapture) {

				if (focus->isHover) {
					if (app->BuildHoverDisplay(focus->name, ds) | app->BuildHoverDisplay(focus->displayClass, ds))
						MakeAnim(focus, ds);
				}
				else if (app->BuildNormalDisplay(focus->name, ds) | app->BuildNormalDisplay(focus->displayClass, ds))
					MakeAnim(focus, ds);

			}



		}

		if (e) {
			e->isFocus = true;

			if (!e->isCapture) {
				Display ds;
				if (app->BuildFocusDisplay(e->name, ds) | app->BuildFocusDisplay(e->displayClass, ds))
					MakeAnim(e, ds);
			}


		}
		focus = e;

		return true;
	}

	return false;

}

bool Window::SetHoverElement(UIElement*e) {
	if (hover != e) {

		if (hover) {
			auto r = hover;

			do {
				r->isHover = false;
				if (!r->isCapture && !r->isFocus) {
					Display ds;

					if (app->BuildNormalDisplay(r->name, ds) | app->BuildNormalDisplay(r->displayClass, ds))
						MakeAnim(r, ds);
				}
			} while (r = r->parent);

		}
		if (e) {
			switch (e->display.cursor)
			{
			case CursorType::arrow:
				SetCursor(cArrow);
				break;
			case CursorType::hand:
				SetCursor(cHand);
				break;
			case CursorType::text:
				SetCursor(cText);
				break;
			case CursorType::wait:
				SetCursor(cWait);
				break;
			default:
				break;
			}
			auto r = e;
			do {
				r->isHover = true;
				if (!r->isCapture && !r->isFocus) {
					Display ds;
					if (app->BuildHoverDisplay(r->name, ds) | app->BuildHoverDisplay(r->displayClass, ds))
						MakeAnim(r, ds);

				}
			} while (r = r->parent);

		}
		hover = e;

		return true;

	}

	return false;
}

bool Window::OnScroll(short delta) {

	UIElement* e;

	if (capture)
		e = capture;
	else if (hover)
		e = hover;
	else
		return false;

	do {
		float curr = e->scrollY - delta;

		if (curr < 0)
			curr = 0;
		else if (curr > e->maxScrollY)
			curr = e->maxScrollY;

		if (curr != e->scrollY) {
			e->scrollY = curr;
			e->needFindVisible = true;
			return true;
		}
		e = e->parent;
	} while (e);

	return false;

}

bool Window::OnHScroll(short delta) {

	UIElement* e;


	if (capture)
		e = capture;
	else if (hover)
		e = hover;
	else
		return false;

	do {
		float curr = e->scrollX + delta;

		if (curr < 0)
			curr = 0;
		else if (curr > e->maxScrollX)
			curr = e->maxScrollX;

		if (curr != e->scrollX) {
			e->scrollX = curr;
			e->needFindVisible = true;
			return true;
		}

		e = e->parent;
	} while (e);

	return false;

}

bool Window::OnLBtnDown(float x, float y) {

	if (!page)
		return false;

	bool b = false;

	vector<UIElement*>els;
	UIElement *e = page->root;

	if (!e)
		return false;

	do {
		els.push_back(e);
		e = e->SelectObject(x, y);
	} while (e);

	for (auto i = els.rbegin(); i != els.rend(); i++) {
		if ((*i)->capturable) {
			b |= SetCaptureElement(*i);
			b |= SetHoverElement(*i);
			b |= (*i)->OnLButtonDown(x, y, this);
			break;
		}
	}
	for (auto i = els.rbegin(); i != els.rend(); i++) {
		if ((*i)->focusable) {
			b |= SetFocusElement(*i);
			break;
		}
	}

	return b;
}

bool Window::OnLBtnUp(float x, float y) {

	bool b = false;

	float curTime = globalTimer.TotalTime();

	if (lastPoint.X == x && lastPoint.Y == y && curTime - lastClickTime < 0.3f) {
		b |= capture->OnDoubleClick(x, y, this);
	}
	else if (capture) {
		auto r = capture->rect;
		r.X -= capture->padding.left;
		r.Y -= capture->padding.top;
		r.Height += capture->padding.top + capture->padding.bottom;
		r.Width += capture->padding.left + capture->padding.right;
		if (r.Contains(x, y)) {
			auto e = capture;
			b |= e->OnClick(x, y, this);
			if (e->onclick.length()) {
				try
				{
					(onclick.at(e->onclick))(this);
				}
				catch (const std::exception&)
				{

				}
			}

		}

	}


	b |= SetCaptureElement(nullptr);

	lastPoint = { x,y };
	lastClickTime = curTime;

	return b;
}

UIElement * Window::GetElement(std::string&&id)
{
	if (page)
		return page->ids[id]; 
	else return nullptr;
}

bool Window::OnRBtnUp(float x, float y) {

	bool b = false;

	if (capture&&capture->rect.Contains(x, y))
		b |= capture->OnRightClick(x, y, this);

	b |= SetCaptureElement(nullptr);

	return b;
}

bool Window::OnRBtnDown(float x, float y) {

	if (!page)
		return false;

	bool b = false;

	vector<UIElement*>els;
	UIElement *e = page->root;

	if (!e) {
		return false;
	}

	do {
		els.push_back(e);
		e = e->SelectObject(x, y);
	} while (e);

	for (auto i = els.rbegin(); i != els.rend(); i++) {
		if ((*i)->capturable) {
			b |= SetCaptureElement(*i);
			b |= SetHoverElement(*i);
			break;
		}
	}
	for (auto i = els.rbegin(); i != els.rend(); i++) {
		if ((*i)->focusable) {
			b |= SetFocusElement(*i);
			break;
		}
	}

	return b;
}



bool Window::OnMouseMove(float x, float y) {

	if (!page||!page->root) {
		return false;
	}

	if (capture) {

		bool b = capture->OnMouseMove(x, y, this);

		if (capture->rect.Contains(x, y))
			return b | SetHoverElement(capture);
		else return b | SetHoverElement(nullptr);

	}
	else {
		vector<UIElement*>els;
		UIElement *e = page->root;

		do {
			els.push_back(e);
			e = e->SelectObject(x, y);
		} while (e);

		return SetHoverElement(els.back()) | els.back()->OnMouseMove(x, y, this);
	}

	return false;

}

void Window::MakeAnim(UIElement* e, Display&other) {

	auto& anim = e->anim;

	if (other.duration > 0.01) {
		e->anim.Release();
		e->anim.startTime = globalTimer.TotalTime();
		e->anim.duration = other.duration;
		if (OtherValid(DisplayFlag::background)) {
			if (e->display.background.type == other.background.type) {
				auto& start = e->display.background;
				auto& end = other.background;
				for (auto i = 0; i < end.count; i++) {
					anim.Add(start.colors[i], start.colors[i], end.colors[i]);
					anim.Add(start.stops[i], start.stops[i], end.stops[i]);
				}
				anim.Add(start.start, start.start, end.start);
				anim.Add(start.end, start.end, end.end);

				start.imageName = end.imageName;
				start.count = end.count;
			}
			else {
				e->display.background = other.background;
			}
		}
		else {
			e->display.background = other.background;
		}

		if (OtherValid(DisplayFlag::border)) {
			auto& start = e->display.border;
			auto& end = other.border;
			if (end.type == start.type) {
				anim.Add(start.color, start.color, end.color);
				anim.Add(start.size, start.size, end.size);
			}
			else {
				start = end;
			}
		}
		else e->display.border = other.border;

		if (OtherValid(DisplayFlag::color)) {

			anim.Add(e->display.color, e->display.color, other.color);

		}
		else e->display.color = other.color;

		if (OtherValid(DisplayFlag::opacity)) {

			anim.Add(e->display.opacity, e->display.opacity, other.opacity);
		}
		else e->display.opacity = other.opacity;

		if (OtherValid(DisplayFlag::radius)) {
			anim.Add(e->display.radius, e->display.radius, other.radius);
		}
		else e->display.radius = other.radius;

		if (OtherValid(DisplayFlag::transform)) {

			auto& start = e->display.transform;
			auto& end = other.transform;
			if (end.use) {
				anim.Add(start.center, start.center, end.center);
				anim.Add(start.rotate, start.rotate, end.rotate);
				anim.Add(start.skew, start.skew, end.skew);
				anim.Add(start.scale, start.scale, end.scale);
				anim.Add(start.transform, start.transform, end.transform);
			}
			start.use = end.use;
		}
		else e->display.transform = other.transform;

		if (OtherValid(DisplayFlag::boxShadow)) {

			auto& start = e->display.boxShadow;
			auto& end = other.boxShadow;
			if (start.type == end.type) {
				anim.Add(start.color, start.color, end.color);
				anim.Add(start.size, start.size, end.size);
			}
			else {
				start = end;
			}

		}
		else e->display.boxShadow = other.boxShadow;

	}
	else {
		e->anim.Release();
		e->display = other;
	}

	e->display.cursor = other.cursor;

}

void Window::ToggleSize() {

	if (state == WindowState::normal) {
		ShowWindow(hWnd, SW_SHOWMAXIMIZED);
	}
	else if (state == WindowState::maxmize) {
		ShowWindow(hWnd, SW_SHOWNORMAL);
	}

}