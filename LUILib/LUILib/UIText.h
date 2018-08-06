#pragma once
#include"UIElement.h"
#include<dwrite.h>
namespace LUI {
	class UIText :public UIElement
	{
	public:
		static IDWriteFactory *factory;
		UIText();
		virtual ~UIText();

		bool selectable;
		
		std::wstring text;
	protected:

		IDWriteTextLayout *textLayout;

	public:

		UIElement * Copy()override;
		
		void SetAttr(std::string&, std::string&)override;

		void OnLayout()override;

		void FindVisible()override { needFindVisible = false; first = 0; last = 0; }

		UIElement*SelectObject(float x, float y) { return nullptr; }

		bool OnPaint(Painter*)override;

	};

}