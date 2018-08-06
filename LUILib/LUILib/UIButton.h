#pragma once
#include"UIElement.h"
namespace LUI {
	class UIButton :public UIElement
	{
	public:
		UIButton();
		~UIButton();

		D2D1_POINT_2F p;
		float startTime;
		float startTime2;
		int animType;

		bool OnPaint(Painter*)override;

		bool OnLButtonDown(float x, float y)override;
		bool OnClick(float x, float y)override;

		UIElement* Copy()override;

	};

}