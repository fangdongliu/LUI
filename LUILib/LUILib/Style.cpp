#include "stdafx.h"
#include "Style.h"

using namespace LUI;
using namespace D2D1;

#define SetFlag(v) {flag|=(1<<(int)v);}

#define ThisValid(v) flag&(1<<(int)v)

#define OtherValid(v) other->flag&(1<<(int)v)

#define SETATTR(v) if(OtherValid(DisplayFlag::##v)){SetFlag(DisplayFlag::##v);v=other->##v;}

#define SETATTR2(v) if(OtherValid(LayoutFlag::##v)){SetFlag(LayoutFlag::##v);v=other->##v;}

Display::Display() {
	flag = 0;

	cursor = CursorType::arrow;

	duration = 0.0f;
	opacity = 1.0f;

	transform.use = false;
	transform.scale = { 1.0,1.0 };
	transform.skew = { 0,0 };
	transform.transform = { 0,0 };
	transform.rotate = 0;
	transform.center = { 0.5,0.5 };

	background.type = Background::Type::None;
	background.count = 0;
	background.start = { 0,0 };
	background.end = { 1,0 };

	radius = 0;


	border.size = 0.0f;
	border.type = BorderType::None;
	border.color = ColorF(0, 0, 0);

	boxShadow.size = 0;
	boxShadow.type = BorderType::None;
	boxShadow.color = ColorF(0, 0, 0);

	color = D2D1::ColorF(0, 0, 0);

}

Layout::Layout() {
	flag = 0;
	isLeft = true;
	isTop = true;
	width = { 0 };
	height = { 0 };
	widthWrapContent = false;
	heightWrapContent = false;
	margin = { 0 };
	padding = { 0 };
	left = { 0 };
	top = { 0 };
	right = { 0 };
	bottom = { 0 };
	type = DisplayType::Inline;


	text.wrapping = DWRITE_WORD_WRAPPING_NO_WRAP;
	text.size = 16.0f;
	text.stretch = DWRITE_FONT_STRETCH_NORMAL;
	text.style = DWRITE_FONT_STYLE_NORMAL;
	text.weight = DWRITE_FONT_WEIGHT_NORMAL;
	text.vertical = DWRITE_TEXT_ALIGNMENT_LEADING;
	text.horizontal = DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
}

void Display::Union(Display*other) {

	SETATTR(background);

	SETATTR(border);

	SETATTR(color);

	SETATTR(duration);

	SETATTR(radius);

	SETATTR(transform);

	SETATTR(boxShadow);

	SETATTR(opacity);

	SETATTR(cursor);

}

void Layout::Union(Layout*other) {

	SETATTR2(top);

	SETATTR2(left);

	SETATTR2(right);

	SETATTR2(bottom);

	SETATTR2(width);

	SETATTR2(height);

	SETATTR2(isTop);

	SETATTR2(isLeft);

	SETATTR2(margin);

	SETATTR2(padding);

	SETATTR2(widthWrapContent);

	SETATTR2(heightWrapContent);

	SETATTR2(type);

	SETATTR2(text);
}