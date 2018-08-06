#pragma once
#include"UIElement.h"
class IDWriteTextLayout;
namespace LUI {

	enum class SelectMode {
		head, tile,
		lastChar, nextChar,
		lastWord, nextWord,
		absoluteLeading, absoluteTrailing,
		lastLine,nextLine,
		all,
		up,bottom,
	};

	class UIInput:public UIElement
	{
	public:
		UIInput();
		~UIInput();

		UINT32 caretAnchor;
		UINT32 caretPosition;
		std::wstring placeHolder;
		std::wstring value;

		IDWriteTextLayout* placeLayout;
		IDWriteTextLayout* textLayout;

		void Select(SelectMode mode, bool moveAnchor=true,int pos=0);

		DWRITE_TEXT_RANGE GetSelectionRange();

		void CopyToClipboard();
		void PasteFromClipboard();
		void DeleteSelection();
		void SetSelectionFromPoint(float x, float y, bool heldShift);

		bool OnChar(wchar_t)override;
		bool OnKey(UINT32 keyCode)override;
		virtual bool OnMouseMove(float x, float y);
		virtual bool OnLButtonDown(float x, float y);
		virtual bool OnRightClick(float x, float y) { return false; };
		virtual bool OnPaint(Painter*);
		virtual void OnLayout();
		void FindVisible()override { needFindVisible = false; first = 0; last = 0; }
		UIElement*SelectObject(float x, float y) { return nullptr; }
		void SetAttr(std::string&, std::string&)override;
		virtual UIElement * Copy();
	};

}