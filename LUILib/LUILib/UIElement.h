#pragma once
#include<d2d1.h>
#include "Style.h"
#include <vector>
#include"LUITypes.h"
#include"Animation.h"
namespace LUI {

	class Window;
	class Painter;

	class UIElement
	{
	public:
		UIElement();
		virtual ~UIElement();

		UIElement *parent;
		std::vector<UIElement*>child;

	public:

		virtual UIElement * Copy();
		Animation anim;
		LRectF rect;
		LPointF relativePos;
		LRectF contentRect;
		D2D1_RECT_F margin;
		D2D1_RECT_F padding;

		Layout layout;
		Display display;

		int first;
		int last;

		bool needMeasure;
		bool needLayout;
		bool needFindVisible;
		bool isHidden;
		bool focusable;
		bool capturable;
		bool disabled;


		bool isHover;
		bool isFocus;
		bool isCapture;

		float scrollX;
		float scrollY;
		float maxScrollX;
		float maxScrollY;

		std::wstring hint;
		std::string name;
		std::string id;
		std::string layoutClass;
		std::string displayClass;
	public:
		void NeedMeasure();
		
		virtual void SetAttr(std::string&, std::string&);

		void OnMeasure(float width, float height);


	public:
		virtual bool OnChar(wchar_t) { return false; }
		virtual bool OnKey(UINT32) { return false; }
		virtual void OnHover(float x,float y) {};
		virtual bool OnMouseMove(float x, float y) { return false; };
		virtual bool OnLButtonDown(float x, float y) { return false; };
		virtual bool OnClick(float x, float y) { return false; };
		virtual bool OnRightClick(float x, float y) { return false; };
		virtual bool OnPaint(Painter*) { return false; }
		virtual void OnLayout();
		virtual void FindVisible();
		virtual UIElement* SelectObject(float x, float y);
	};

}