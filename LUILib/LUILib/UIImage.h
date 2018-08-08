#pragma once
#include"UIElement.h"
namespace LUI {
	class UIImage:public UIElement
	{
	public:
		UIImage();
		virtual ~UIImage();

		ID2D1Bitmap *bitmap;
		std::string src;

		void SetAttr(std::string&, std::string&)override;

		bool OnPaint(Painter*)override;

		void OnLayout()override;

		UIElement* CopyData()override;

	};


}