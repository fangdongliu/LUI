#pragma once
namespace LUI {
	class Window;
	class UIElement;
	class Display;
	class Dealer
	{
	public:
		Dealer();
		~Dealer();

		UIElement* hover;
		UIElement* focus;
		UIElement* capture;
		UIElement* root;
	public:
		bool OnKey(int);
		bool OnChar(wchar_t);
		bool OnHover(float x, float y);
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
	};

}