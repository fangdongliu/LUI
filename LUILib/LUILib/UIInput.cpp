#include "stdafx.h"
#include "UIInput.h"
#include"UIText.h"
#include"Painter.h"
#include<iostream>

#include"Timer.h"
using namespace LUI;
using namespace std;

namespace {
	inline int RoundToInt(float x)
	{
		return static_cast<int>(floor(x + .5));
	}
	inline bool IsHighSurrogate(UINT32 ch) throw()
	{
		// 0xD800 <= ch <= 0xDBFF
		return (ch & 0xFC00) == 0xD800;
	}


	inline bool IsLowSurrogate(UINT32 ch) throw()
	{
		// 0xDC00 <= ch <= 0xDFFF
		return (ch & 0xFC00) == 0xDC00;
	}
}


UIInput::UIInput()
{
	caretPosition = 0;
	caretAnchor = 0;
	textLayout = nullptr;
	placeLayout = nullptr;

	focusable = true;
	capturable = true;
}


UIInput::~UIInput()
{
	SafeRelease(&textLayout);
	SafeRelease(&placeLayout);
}

void UIInput::CopyToClipboard()
{
	// Copies selected text to clipboard.

	DWRITE_TEXT_RANGE selectionRange = GetSelectionRange();
	if (selectionRange.length <= 0)
		return;

	// Open and empty existing contents.
	if (OpenClipboard(0))
	{
		if (EmptyClipboard())
		{
			// Allocate room for the text
			size_t byteSize = sizeof(wchar_t) * (selectionRange.length + 1);
			HGLOBAL hClipboardData = GlobalAlloc(GMEM_DDESHARE | GMEM_ZEROINIT, byteSize);

			if (hClipboardData != NULL)
			{
				void* memory = GlobalLock(hClipboardData);  // [byteSize] in bytes

				if (memory != NULL)
				{
					// Copy text to memory block.
					const wchar_t* text = value.c_str();
					memcpy(memory, &text[selectionRange.startPosition], byteSize);
					GlobalUnlock(hClipboardData);

					if (SetClipboardData(CF_UNICODETEXT, hClipboardData) != NULL)
					{
						hClipboardData = NULL; // system now owns the clipboard, so don't touch it.
					}
				}
				GlobalFree(hClipboardData); // free if failed
			}
		}
		CloseClipboard();
	}
}

void UIInput::PasteFromClipboard()
{
	// Pastes text from clipboard at current caret position.

	DeleteSelection();

	UINT32 characterCount = 0;

	// Copy Unicode text from clipboard.

	if (OpenClipboard(0))
	{
		HGLOBAL hClipboardData = GetClipboardData(CF_UNICODETEXT);

		if (hClipboardData != NULL)
		{
			// Get text and size of text.
			size_t byteSize = GlobalSize(hClipboardData);
			void* memory = GlobalLock(hClipboardData); // [byteSize] in bytes
			const wchar_t* text = reinterpret_cast<const wchar_t*>(memory);
			characterCount = static_cast<UINT32>(wcsnlen(text, byteSize / sizeof(wchar_t)));

			if (memory != NULL)
			{
				// Insert the text at the current position.
				value.insert(
					caretPosition,
					text,
					characterCount
				);
				GlobalUnlock(hClipboardData);
				
			}
		}
		CloseClipboard();
	}

	caretPosition += characterCount;
	caretAnchor = caretPosition;

	NeedMeasure();
}



void UIInput::Select(SelectMode mode, bool moveAnchor, int pos) {


	switch (mode)
	{
	case LUI::SelectMode::up:
	case LUI::SelectMode::head:
		caretPosition = 0;
		break;
	case LUI::SelectMode::bottom:
	case LUI::SelectMode::tile:
		caretPosition = value.length();
		break;
	case LUI::SelectMode::lastChar:
		if (caretPosition < pos)
			caretPosition = 0;
		else caretPosition -= pos;
		break;
	case LUI::SelectMode::nextChar:
		if (caretPosition > value.length())
			caretPosition = value.length();
		else caretPosition += pos;
		break;
	case LUI::SelectMode::lastWord:
	case LUI::SelectMode::nextWord: {
		std::vector<DWRITE_CLUSTER_METRICS> clusterMetrics;
		UINT32 clusterCount;
		textLayout->GetClusterMetrics(NULL, 0, &clusterCount);
		if (clusterCount == 0)
			break;

		clusterMetrics.resize(clusterCount);
		textLayout->GetClusterMetrics(&clusterMetrics.front(), clusterCount, &clusterCount);

		UINT32 clusterPosition = 0;
		UINT32 oldCaretPosition = caretPosition;

		if (mode == SelectMode::lastWord) {

			caretPosition = 0;
			for (UINT32 cluster = 0; cluster < clusterCount; ++cluster) {

				clusterPosition += clusterMetrics[cluster].length;
				if (clusterMetrics[cluster].canWrapLineAfter) {
					if (clusterPosition >= oldCaretPosition)
						break;

					caretPosition = clusterPosition;
				}

			}

		}
		else {
			for (UINT32 cluster = 0; cluster < clusterCount; ++cluster) {
				UINT32 clusterLength = clusterMetrics[cluster].length;

				if (clusterPosition+clusterMetrics[cluster].length > oldCaretPosition && clusterMetrics[cluster].canWrapLineAfter) {
					caretPosition = clusterPosition + clusterMetrics[cluster].length;
					break;

				}
				clusterPosition += clusterLength;
				caretPosition = clusterPosition;
			}
		}
		break;
	}
	case LUI::SelectMode::absoluteLeading:{
		DWRITE_HIT_TEST_METRICS hitTestMetrics;
		float caretX, caretY;

		// Get the size of the following cluster.
		textLayout->HitTestTextPosition(
			caretPosition,
			false,
			&caretX,
			&caretY,
			&hitTestMetrics
		);

		caretPosition = hitTestMetrics.textPosition;

		break;
	}
	case LUI::SelectMode::absoluteTrailing: {
		DWRITE_HIT_TEST_METRICS hitTestMetrics;
		float caretX, caretY;

		// Get the size of the following cluster.
		textLayout->HitTestTextPosition(
			caretPosition,
			true,
			&caretX,
			&caretY,
			&hitTestMetrics
		);

		caretPosition = hitTestMetrics.textPosition + hitTestMetrics.length;
		break;
	}
	case LUI::SelectMode::all:
		caretAnchor = 0;
		caretPosition = value.length();
		return;
	default:
		break;
	}


	if (moveAnchor)
		caretAnchor = caretPosition;

		// update the caret formatting attributes
}

void UIInput::DeleteSelection() {

	DWRITE_TEXT_RANGE range=GetSelectionRange();

	if (range.length <= 0)
		return;

	cout << range.startPosition <<' '<<range.length<<endl;

	value.erase(range.startPosition, range.length);
	
	caretPosition = range.startPosition;
	caretAnchor = caretPosition;

	NeedMeasure();

}

DWRITE_TEXT_RANGE UIInput::GetSelectionRange() {

	UINT32 caretBegin = caretAnchor;
	UINT32 caretEnd = caretPosition;
	if (caretBegin > caretEnd)
		std::swap(caretBegin, caretEnd);

	UINT32 textLength = (UINT32)(value.length());

	if (caretBegin > textLength)
		caretBegin = textLength;

	if (caretEnd > textLength)
		caretEnd = textLength;

	return { caretBegin,caretEnd - caretBegin };

}

bool UIInput::OnKey(UINT32 keyCode) {

	bool heldShift = (GetKeyState(VK_SHIFT) & 0x80) != 0;
	bool heldControl = (GetKeyState(VK_CONTROL) & 0x80) != 0;

	switch (keyCode)
	{
	case VK_BACK:
		if (caretPosition != caretAnchor)
		{
			DeleteSelection();
		}
		else if (caretPosition > 0)
		{
			UINT32 count = 1;
			if (caretPosition >= 2
				&& caretPosition <= value.length())
			{
				wchar_t charBackOne = value[caretPosition - 1];
				wchar_t charBackTwo = value[caretPosition - 2];
				if ((IsLowSurrogate(charBackOne) && IsHighSurrogate(charBackTwo))
					|| (charBackOne == '\n' && charBackTwo == '\r'))
				{
					count = 2;
				}
			}

			caretPosition -= count;
			caretAnchor = caretPosition;

			value.erase(caretPosition, count);
			
			NeedMeasure();
			
		}
		break;
	case VK_DELETE:
		if (caretPosition != caretAnchor) {
			DeleteSelection();
		}
		else {
			DWRITE_HIT_TEST_METRICS hitTestMetrics;
			float caretX, caretY;

			// Get the size of the following cluster.
			textLayout->HitTestTextPosition(
				caretPosition,
				false,
				&caretX,
				&caretY,
				&hitTestMetrics
			);

			value.erase(hitTestMetrics.textPosition, hitTestMetrics.length);
			NeedMeasure();
		}
		break;
	case VK_TAB:
		break;
	case VK_LEFT:

		if (!heldControl)
		{
			if (caretPosition > 0) {
				UINT32 count = 1;
				if (caretPosition >= 2
					&& caretPosition <= value.length())
				{
					wchar_t charBackOne = value[caretPosition - 1];
					wchar_t charBackTwo = value[caretPosition - 2];
					if ((IsLowSurrogate(charBackOne) && IsHighSurrogate(charBackTwo))
						|| (charBackOne == '\n' && charBackTwo == '\r'))
					{
						count = 2;
					}
				}

				Select(SelectMode::lastChar, !heldShift, count);

				NeedMeasure();
			}
		}
		else Select(SelectMode::lastWord, !heldShift,1);
		break;

	case VK_RIGHT: 



		if (!heldControl)
		{
			if (caretPosition < value.length()) {
				UINT32 count = 1;
				if (caretPosition >= 0
					&& caretPosition <= value.length() - 2)
				{
					wchar_t charBackOne = value[caretPosition];
					wchar_t charBackTwo = value[caretPosition + 1];
					if ((IsLowSurrogate(charBackOne) && IsHighSurrogate(charBackTwo))
						|| (charBackOne == '\n' && charBackTwo == '\r'))
					{
						count = 2;
					}
				}

				Select(SelectMode::nextChar, !heldShift, count);

				NeedMeasure();
			}
		}
		else
			Select(SelectMode::nextWord, !heldShift, 1);
		break;
	case VK_UP:
	case VK_HOME:
		Select(SelectMode::head);
		break;
	case VK_DOWN:
	case VK_END:
		Select(SelectMode::tile);
		break;
	case 'C':
		if (heldControl)
			CopyToClipboard();
		break;
	case VK_INSERT:
		if (heldControl)
			CopyToClipboard();
		else if (heldShift)
			PasteFromClipboard();
		break;
	case 'V':
		if (heldControl)
			PasteFromClipboard();
		break;
	case 'X':
		if (heldControl) {
			CopyToClipboard();
			DeleteSelection();
		}
		break;
	case 'A':
		if (heldControl)
			Select(SelectMode::all);
		break;
	default:
		return false;
	}

	return true;

}


bool UIInput::OnChar(wchar_t c) {

	

	if (c >= 0x20 || c == 9)
	{
		// Replace any existing selection.
		cout << c << endl;

		DeleteSelection();

		// Convert the UTF32 character code from the Window message to UTF16,
		// yielding 1-2 code-units. Then advance the caret position by how
		// many code-units were inserted.

		UINT32 charsLength = 1;
		wchar_t chars[2] = { static_cast<wchar_t>(c), 0 };

		// If above the basic multi-lingual plane, split into
		// leading and trailing surrogates.
		if (c > 0xFFFF)
		{
			// From http://unicode.org/faq/utf_bom.html#35
			chars[0] = wchar_t(0xD800 + (c >> 10) - (0x10000 >> 10));
			chars[1] = wchar_t(0xDC00 + (c & 0x3FF));
			charsLength++;
		}

		value.insert(caretPosition, chars, charsLength);

		caretPosition += charsLength;
		caretAnchor = caretPosition;
		
		NeedMeasure();

		return true;
	}

	return false;
}

void UIInput::OnLayout() {

	IDWriteTextFormat*textFormat = nullptr;

	UIText::factory->CreateTextFormat(layout.text.fontFamily.c_str(), 0,
		layout.text.weight, layout.text.style, layout.text.stretch, layout.text.size, L"", &textFormat);

	SafeRelease(&textLayout);




	UIText::factory->CreateTextLayout(
		value.c_str(),
		static_cast<UINT32>(value.length()),
		textFormat,
		rect.Width, rect.Height,
		&textLayout
	);

	DWRITE_TEXT_METRICS metrics;

	if (textLayout) {
		needLayout = false;
		needFindVisible = true;
		DWRITE_TEXT_RANGE range = { 0,-1 };


		textLayout->SetTextAlignment(layout.text.vertical);
		textLayout->SetParagraphAlignment(layout.text.horizontal);
		textLayout->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
		textLayout->GetMetrics(&metrics);

		contentRect.Clear();
		contentRect.Width = metrics.width;
		contentRect.Height = metrics.height;

		maxScrollX = max(contentRect.Width - rect.Width, 0);
		maxScrollY = max(contentRect.Height - rect.Height, 0);

	}

	SafeRelease(&placeLayout);

	UIText::factory->CreateTextLayout(placeHolder.c_str(), static_cast<UINT32>(placeHolder.length()), textFormat, rect.Width, rect.Height, &placeLayout);

	if (placeLayout) {

		placeLayout->SetTextAlignment(layout.text.vertical);
		placeLayout->SetParagraphAlignment(layout.text.horizontal);
		placeLayout->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

	}

	SafeRelease<IDWriteTextFormat>(&textFormat);
}

bool UIInput::OnPaint(Painter*painter) {
	if (textLayout) {
		float offsetX=0;
		float offsetY=0;
		if (isFocus) {
			auto range = GetSelectionRange();
			cout << range.startPosition << range.length<<endl;
			UINT32 actualHitTestCount = 0;
			if (range.length > 0) {
				textLayout->HitTestTextRange(range.startPosition, range.length, 0, 0, 0, 0, &actualHitTestCount);
				std::vector<DWRITE_HIT_TEST_METRICS>hitTestMetrics(actualHitTestCount);
				textLayout->HitTestTextRange(range.startPosition, range.length, rect.X-scrollX, rect.Y, &hitTestMetrics[0], static_cast<UINT32>(hitTestMetrics.size()), &actualHitTestCount);
				painter->brush->SetColor(D2D1::ColorF(D2D1::ColorF::LightSkyBlue));
				for (int i = 0; i < actualHitTestCount; i++) {
					const DWRITE_HIT_TEST_METRICS& htm = hitTestMetrics[i];
					D2D1_RECT_F highlightRect = {
						htm.left,
						htm.top,
						(htm.left + htm.width),
						(htm.top + htm.height)
					};
					painter->target->FillRectangle(highlightRect, painter->brush);
				}
			}

			DWRITE_HIT_TEST_METRICS caretMetrics;
			float caretX, caretY;
			textLayout->HitTestTextPosition(caretPosition,  false , &caretX, &caretY, &caretMetrics);
			if (caretX - scrollX > rect.Width) {
				scrollX = caretX - rect.Width;
			}
			else if (caretX - scrollX < 0) {
				scrollX = caretX;
			}
			
			if (sin((globalTimer.TotalTime() - globalTimer.mLastEventTime)*6.28f) > -0.1) {
				if (caretPosition > caretAnchor) {
					painter->brush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
					painter->target->DrawLine(D2D1::Point2F(caretX + rect.X - scrollX , caretY+rect.Y)
						, D2D1::Point2F(caretX+rect.X -scrollX, caretY+caretMetrics.height+rect.Y), painter->brush);
				}
				else {
					painter->brush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
					painter->target->DrawLine(D2D1::Point2F(caretX + rect.X - scrollX , caretY+rect.Y)
						, D2D1::Point2F(caretX + rect.X - scrollX, caretY + caretMetrics.height+rect.Y), painter->brush);

				}
			}
		}

		if (value.length() == 0&&placeLayout) {
			painter->brush->SetColor(D2D1::ColorF(display.color.r, display.color.g, display.color.b, display.color.a*0.6));
			painter->target->DrawTextLayout(D2D1::Point2F(rect.X, rect.Y), placeLayout, painter->brush);
		}

		painter->brush->SetColor(display.color);
		painter->target->DrawTextLayout(D2D1::Point2F(rect.X-scrollX, rect.Y), textLayout, painter->brush);
	}
	return false;

}

void UIInput::SetAttr(string&attr, string&value) {

	enum class Attr {
		Error,
		layout,
		display,
		hide,
		focusable,
		capturable,
		disabled,
		text,
		placeHolder,
		hint
	};

	static unordered_map<string, Attr>m = {
		{ "layout",Attr::layout },
	{ "display",Attr::display },
	{ "hide",Attr::hide },
	{ "focusable",Attr::focusable },
	{ "capturable",Attr::capturable },
	{ "disabled",Attr::disabled },
	{ "value",Attr::text },
	{ "hint",Attr::hint },
	{"placeHolder",Attr::placeHolder}
	};


	switch (m[attr])
	{
	case Attr::placeHolder:
		UTF8ToUnicode(value.c_str(), placeHolder);
		break;
	case Attr::layout:
		layoutClass = value;
		break;
	case Attr::display:
		displayClass = value;
		break;
	case Attr::hide:
		isHidden = value != "false";

		break;
	case Attr::hint:
		UTF8ToUnicode(value.c_str(), hint);
		break;
	case Attr::focusable:
		focusable = value != "false";
		break;
	case Attr::capturable:
		capturable = value != "false";
		break;
	case Attr::text:
		UTF8ToUnicode(value.c_str(), this->value);
		break;
	case Attr::disabled:
		disabled = value != "false";
		break;
	default:
		break;
	}



}

UIElement* UIInput::Copy() {

	auto n = new UIInput;

	for (auto i : child) {
		auto e = i->Copy();
		e->parent = n;
		n->child.push_back(e);
	}

	needMeasure = true;
	n->isHidden = isHidden;
	n->layout = layout;
	n->display = display;
	n->layoutClass = layoutClass;
	n->displayClass = displayClass;
	n->name = name;
	n->value = value;
	n->hint = hint;
	n->placeHolder = placeHolder;

	n->focusable = focusable;
	n->capturable = capturable;
	n->disabled = disabled;

	scrollY = 0.0f;
	scrollX = 0.0f;

	return n;
}


bool UIInput::OnMouseMove(float x, float y) {

	if (isCapture) {

		cout << x <<' '<< y<<endl;

		SetSelectionFromPoint(x + scrollX, y , false);

		return true;

	}

	return false;

}

bool UIInput::OnLButtonDown(float x, float y) {

	SetSelectionFromPoint(x + scrollX, y, (GetKeyState(VK_SHIFT) & 0x80) == 0);

	return true;
}

void UIInput::SetSelectionFromPoint(float x, float y, bool moveAnchor) {

	BOOL isTrailingHit;
	BOOL isInside;
	DWRITE_HIT_TEST_METRICS caretMetrics;


	textLayout->HitTestPoint(
		x-rect.X,
		y-rect.Y,
		&isTrailingHit,
		&isInside,
		&caretMetrics
	);

	if (isTrailingHit) {
		caretPosition = caretMetrics.textPosition + caretMetrics.length;
	}
	else {
		caretPosition = caretMetrics.textPosition;
	}

	if (moveAnchor)
		caretAnchor = caretPosition;

	return;

}