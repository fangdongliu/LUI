#pragma once
#include"UIElement.h"
#include<dwrite.h>
namespace LUI {

	enum class SelectMode {
		head, tile,
		lastChar, nextChar,
		lastWord, nextWord,
		absoluteLeading, absoluteTrailing,
		lastLine, nextLine,
		all,
		up, bottom,
	};

	class UIInput :public UIElement
	{
	public:
		UIInput();
		virtual ~UIInput();

		UINT32 caretAnchor;
		UINT32 caretPosition;
		
		std::wstring placeHolder;
		std::wstring value;

		IDWriteTextLayout* placeLayout;
		IDWriteTextLayout* textLayout;

		void Select(SelectMode mode, bool moveAnchor = true, int pos = 0);

		DWRITE_TEXT_RANGE GetSelectionRange();

		void CopyToClipboard();
		void PasteFromClipboard();
		void DeleteSelection();
		void SetSelectionFromPoint(float x, float y, bool heldShift);

		bool OnChar(wchar_t, Window*)override;
		bool OnKey(KeyEvent&, Window*)override;
		bool OnMouseMove(float x, float y, Window*)override;
		bool OnLButtonDown(float x, float y, Window*)override;
		bool OnDoubleClick(float x, float y, Window*)override;
		bool OnRightClick(float x, float y, Window*) override { return false; };
		bool OnPaint(Painter*)override;
		void OnLayout()override;
		void FindVisible()override { needFindVisible = false; first = 0; last = 0; }
		UIElement*SelectObject(float x, float y) { return nullptr; }
		void SetAttr(std::string&, std::string&)override;
		UIElement * CopyData()override;
	};

}