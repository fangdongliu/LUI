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

#define UPDATEIF(f) if(f)InvalidateRect(hWnd,0,0);

#define IDT_CARETUPDATE 1

auto hand = LoadCursor(0, IDC_ARROW);

namespace {
	App *app = GetApp();

	void PressStyle(UIElement*e) {

		const char *d = " \r\n";
		char *p;
		char *buf;
		p = strtok_s(&e->displayClass[0], d, &buf);
		Display display;
		while (p)
		{

			printf("%s\n", p);
			//			p = strtok_s(NULL, d,p);
		}
	}
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


Window::Window(wstring windowText, string pageName,Window* parent_, int x, int y, int width, int height, bool overlap)
{

	startTime = 0;
	hint = nullptr;
	isDeleted = false;

	if (!parent_) {
		parent = nullptr;
		GetApp()->AddWindow(this);
	}
	else
		parent_->AddChild(this);

	if (overlap) {
		CreateWindowW(L"fdwindow1", windowText.c_str(), WS_OVERLAPPEDWINDOW, x, y, width, height, 0, 0, HINST_THISCOMPONET, this);
	}
	else {
		CreateWindowW(L"fdwindow2", windowText.c_str(), WS_POPUP, x, y, width, height, 0, 0, HINST_THISCOMPONET, this);

	}
	page = GetApp()->GetPage(pageName);
	

	dealer.root = page->root;



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
	delete this;
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
	cout << "showHint\n";

	IDWriteTextFormat*textFormat = nullptr;

	UIText::factory->CreateTextFormat(L"", 0,
		DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,DWRITE_FONT_STRETCH_NORMAL, 16.0f, L"", &textFormat);

	SafeRelease(&hint);

	UIText::factory->CreateTextLayout(
		e->hint.c_str(),
		static_cast<UINT32>(e->hint.length()),
		textFormat,
		rect.right-rect.left-10, rect.bottom-rect.top-4,
		&hint
	);

	DWRITE_TEXT_METRICS metrics;

	if (hint) {
		hint->SetTextAlignment(DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_LEADING);
		hint->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
		hint->SetWordWrapping(DWRITE_WORD_WRAPPING_WHOLE_WORD);
	}
	SafeRelease<IDWriteTextFormat>(&textFormat);


	return true;
}

LRESULT Window::MessageProc(UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_KEYDOWN:
		globalTimer.Tick();
		globalTimer.mLastEventTime = globalTimer.TotalTime();
		OnKey(wp);
		UPDATEIF(dealer.OnKey(wp));
		return 0;
	case WM_CHAR:
		globalTimer.Tick();
		globalTimer.mLastEventTime = globalTimer.TotalTime();
		UPDATEIF(dealer.OnChar(wp));
		return 0;
	case WM_MOUSELEAVE:
		globalTimer.Tick();
		UPDATEIF(dealer.SetHoverElement(nullptr));
		return 0;
	case WM_MOUSEWHEEL:
		globalTimer.Tick();
		UPDATEIF(dealer.OnScroll((short)HIWORD(wp)));
		return 0;
	case WM_MOUSEHWHEEL:
		globalTimer.Tick();
		UPDATEIF(dealer.OnHScroll((short)HIWORD(wp)));
		return 0;
	case WM_MOUSEMOVE:
		globalTimer.Tick();
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.dwFlags = TME_LEAVE | TME_HOVER;
		tme.hwndTrack = hWnd;
		tme.dwHoverTime = 100;
		
		{
			UIElement* lastHover = dealer.hover;
			UPDATEIF(dealer.OnMouseMove((float)(short)LOWORD(lp), (float)(short)HIWORD(lp)));
			UIElement* currHover = dealer.hover;
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
		hintPos.y = HIWORD(lp)+20;
		cout << "mouse hover\n";
		UPDATEIF(ShowHint(dealer.hover)|dealer.OnHover((float)LOWORD(lp), (float)HIWORD(lp)));
		return 0;
	case WM_LBUTTONDOWN:
		globalTimer.Tick();
		globalTimer.mLastEventTime = globalTimer.TotalTime();
		SetFocus(hWnd);
		SetCapture(hWnd);
		UPDATEIF(dealer.OnLBtnDown((float)LOWORD(lp), (float)HIWORD(lp)));
		return 0;
	case WM_LBUTTONUP:
		globalTimer.Tick();
		globalTimer.mLastEventTime = globalTimer.TotalTime();
		ReleaseCapture();
		UPDATEIF(dealer.OnLBtnUp((float)LOWORD(lp), (float)HIWORD(lp)));
		return 0;
	case WM_RBUTTONDOWN:
		globalTimer.Tick();
		SetFocus(hWnd);
		SetCapture(hWnd);
		UPDATEIF(dealer.OnRBtnDown((float)LOWORD(lp), (float)HIWORD(lp)));
		return 0;
	case WM_RBUTTONUP:
		globalTimer.Tick();
		ReleaseCapture();
		UPDATEIF(dealer.OnRBtnUp((float)LOWORD(lp), (float)HIWORD(lp)));
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

			if (page->root) {

				globalTimer.Tick();


				painter.target->BeginDraw();

				painter.target->Clear(D2D1::ColorF(1, 1, 1, 0));

				painter.target->SetTransform(D2D1::Matrix3x2F::Identity());

			

				painter.rect.X = 0;
				painter.rect.Y = 0;
				painter.rect.Width = rect.right - rect.left;
				painter.rect.Height = rect.bottom - rect.top;

				if (page->root->needMeasure)
					page->root->OnMeasure(painter.rect.Width, painter.rect.Height);


				painter.rect.Inflate(10, 10);



				if (!painter.Paint(page->root) & !painter.PaintHint(hint,globalTimer.TotalTime()-startTime,hintPos))
					ValidateRect(hWnd, 0);

				if (hint&&globalTimer.TotalTime() - startTime > 3.3f) {
					SafeRelease(&hint);
				}

				painter.target->EndDraw();
			}
			else
				ValidateRect(hWnd, 0);
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

		if (wp == SIZE_MINIMIZED)
			return 0;

		if(page->root)
			page->root->needMeasure = true;
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
		OnMessage(wp, (void*)lp);
		return 0;
	case WM_SYSMESSAGE:
		OnSysMessage(wp, lp);
		return 0;
	}

	return DefWindowProc(hWnd, msg, wp, lp);
}



bool Window::HideHint() {
	if (hint) {
		SafeRelease(&hint);
		return true;
	}
	return false;
}