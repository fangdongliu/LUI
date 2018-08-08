#include "stdafx.h"
#include "UIText.h"
#include "Window.h"
#include"Page.h"

using namespace std;
using namespace LUI;

IDWriteFactory *UIText::factory = nullptr;

UIText::UIText()
{
	textLayout = nullptr;
	layout.widthWrapContent = true;
	layout.heightWrapContent = true;
}


UIText::~UIText()
{
	SafeRelease(&textLayout);
}


void UIText::OnLayout() {

	IDWriteTextFormat*textFormat = nullptr;

	factory->CreateTextFormat(layout.text.fontFamily.c_str(), 0,
		layout.text.weight, layout.text.style, layout.text.stretch, layout.text.size, L"", &textFormat);

	SafeRelease(&textLayout);

	factory->CreateTextLayout(
		text.c_str(),
		static_cast<UINT32>(text.length()),
		textFormat,
		rect.Width, rect.Height,
		&textLayout
	);

	DWRITE_TEXT_METRICS metrics;

	if (textLayout) {
		needLayout = false;
		needFindVisible = true;

		{
			IDWriteTypography* typoFeature = NULL;
			factory->CreateTypography(&typoFeature);
			DWRITE_FONT_FEATURE feature = { DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_6, 1 };
			typoFeature->AddFontFeature(feature);
			DWRITE_TEXT_RANGE range = { 0,-1 };
			textLayout->SetTypography(typoFeature, range);
			SafeRelease(&typoFeature);
		}

		textLayout->SetTextAlignment(layout.text.vertical);
		textLayout->SetParagraphAlignment(layout.text.horizontal);
		textLayout->SetWordWrapping(layout.text.wrapping);
		textLayout->GetMetrics(&metrics);

		contentRect.Clear();
		contentRect.Width = metrics.width;
		contentRect.Height = metrics.height;

	}
	SafeRelease<IDWriteTextFormat>(&textFormat);
}

bool UIText::OnPaint(Painter*painter) {

	if (textLayout) {
		painter->brush->SetColor(display.color);
		painter->target->DrawTextLayout(D2D1::Point2F(rect.X, rect.Y), textLayout,painter->brush);
	}

	return false;
}

void UIText::SetAttr(string&attr, string&value) {

	enum class Attr {
		Error,
		text,
	};

	static unordered_map<string, Attr>m = {
	{"value",Attr::text},
	};


	switch (m[attr])
	{
	case Attr::text:
		UTF8ToUnicode(value.c_str(), text);
		break;
	default:
		__super::SetAttr(attr, value);
		break;
	}



}

UIElement* UIText::CopyData() {

	auto n = new UIText;

	n->text = text;

	return n;
}