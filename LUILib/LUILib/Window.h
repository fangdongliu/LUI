#pragma once
#include<unordered_set>
#include<d2d1.h>
#include"LUITypes.h"
#include"Painter.h"
#include"Dealer.h"
#include"Timer.h"
class IDWriteTextLayout;

namespace LUI {

	class UIElement;
	class App;
	class Page;
	

	class Window
	{
		friend App;
	public:
		static bool Register();
		Window(std::wstring windowText,std::string pageName,Window*parent = nullptr,int x=CW_USEDEFAULT,int y=0,int width=CW_USEDEFAULT,int height=0, bool overlap = true);
		virtual ~Window();

	public:
		RECT rect;

	protected:
		Dealer dealer;
		Painter painter;
		
		
		Window * parent;
		std::unordered_set<Window*>childs;
		Page *page;
		bool isDeleted;

		LPointF lastPoint;
		IDWriteTextLayout*hint;
		D2D1_POINT_2F hintPos;
		
		float startTime;

	public:
		void AddChild(Window*);

		bool HideHint();
		bool ShowHint(UIElement*);

	private:
		//内部调用
		void Invalidate() { InvalidateRect(hWnd, 0, 0); }
		void OnSysMessage(WPARAM wp, LPARAM lp) {}
		LRESULT MessageProc(UINT, WPARAM, LPARAM);
		HWND hWnd;
		
	public:
		//生命周期
		virtual void OnKey(int vk){}
		virtual void OnHide() {}
		virtual void OnFocus() {}
		virtual void OnKillFocus() {}
		virtual void OnUserMessage() {}
		virtual void OnResize(bool isMaxmize, short width, short height) {}
		virtual void OnTimer() {}
		virtual void OnMessage(UINT msg, void* param) {}

	public://函数池
		inline HWND GetHWnd() { return hWnd; }
		void Maxmize();
		void Minimize();
		void Close();
		void Normal();
		void Focus(){SetFocus(hWnd);}
		void Show();
		void Hide();
		void Center();
		void Refresh();
		void SetIcon(HICON icon);

	};

}