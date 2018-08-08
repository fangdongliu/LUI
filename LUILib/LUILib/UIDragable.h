#pragma once
#include"UIElement.h"
namespace LUI {

	class UIDragable :public UIElement
	{
	public:
		UIDragable();
		virtual ~UIDragable();

		D2D1_POINT_2F lastP;
		float lastTime;

		bool OnLButtonDown(float x, float y, Window*)override;

		UIElement* CopyData()override;

	};

}