#pragma once
#include<unordered_set>
#include<d2d1.h>
#include"LUITypes.h"
#include"Painter.h"
#include"Timer.h"
#include<dwrite.h>
#include<functional>

namespace LUI {

	enum class SysMessageType {

		close,
		minimize,
		maximize,
		show,
		focus,
		center,
		normal

	};

	enum class WindowState {
		maxmize,
		minimize,
		normal,
	};

	class UIElement;
	class App;
	class Page;
	class Window;

	class WindowSetting {
	public:
		WindowSetting(int width = 800, int height = 600, int x = 0, int y = 0) {

			minWidth = 300;
			minHeight = 300;
			this->width = width;
			this->height = height;
			this->x = x;
			this->y = y;
			overlap = true;
			resizable = true;
			parent = nullptr;
			pageName = "index";

		}

		int minWidth;
		int minHeight;
		int width;
		int height;
		int x;
		int y;
		bool overlap;
		bool resizable;
		std::wstring windowText;
		std::string pageName;
		Window* parent;
	};

	

	class Window
	{
		friend App;
	public:
		static bool Register();
		Window(WindowSetting&setting);
		virtual ~Window();

	public:
		RECT rect;
		WindowState state;
	protected:
		Painter painter;


		Window * parent;
		std::unordered_set<Window*>childs;
		Page *page;
		bool isDeleted;
		bool resizeable;
		int minWidth;
		int minHeight;
		LPointF lastPoint;
		float lastClickTime;
		IDWriteTextLayout*hint;
		D2D1_POINT_2F hintPos;

		float startTime;

		UIElement* hover;
		UIElement* focus;
		UIElement* capture;


	public:
		void AddChild(Window*);

		bool HideHint();
		bool ShowHint(UIElement*);

	private:
		//内部调用
		void Invalidate() { InvalidateRect(hWnd, 0, 0); }
		void OnSysMessage(WPARAM wp, LPARAM lp);
		LRESULT MessageProc(UINT, WPARAM, LPARAM);
		HWND hWnd;

	public:

		bool OnKey(int);
		bool OnChar(wchar_t);
		bool OnMouseMove(float x, float y);
		bool OnLBtnDown(float x, float y);
		bool OnRBtnDown(float x, float y);
		bool OnLBtnUp(float x, float y);
		bool OnRBtnUp(float x, float y);
		bool OnScroll(short delta);
		bool OnHScroll(short delta);

		bool SetHoverElement(UIElement*);
		bool SetFocusElement(UIElement*);
		bool SetCaptureElement(UIElement*);
		void MakeAnim(UIElement*, Display&);

		//生命周期
		virtual void OnHide() {}
		virtual void OnFocus() {}
		virtual void OnKillFocus() {}
		virtual void OnUserMessage() {}
		virtual void OnResize(bool isMaxmize, short width, short height) {}
		virtual void OnTimer() {}
		virtual void OnMessage(UINT msg, void* param) {}

		void MapOnClick(std::string&&, std::function<void(Window*)>);
		void MapOnChar(std::string&&, std::function<void(Window*, UINT32)>);
		void MapOnKey(std::string&&, std::function<void(Window*, KeyEvent&)>);
	public://函数池
		std::unordered_map<std::string, std::function<void(Window*)>>onclick;
		std::unordered_map<std::string, std::function<void(Window*,UINT32)>>onchar;
		std::unordered_map<std::string, std::function<void(Window*,KeyEvent&)>>onkey;
		UIElement * GetElement(std::string&&id);
		inline HWND GetHWnd() { return hWnd; }
		void Maxmize();
		void ToggleSize();
		void Minimize();
		void Close();
		void Normal();
		void Focus() { SetFocus(hWnd); }
		void Show();
		void Hide();
		void Center();
		void Refresh();
		void SetIcon(HICON icon);

	};

}