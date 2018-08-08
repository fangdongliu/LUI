#include "stdafx.h"
#include "UILinear.h"
#include"Window.h"
#include"Page.h"
#include<iostream>
using namespace LUI;
using namespace std;


UILinear::UILinear()
{
	linearFloat = LinearFloat::None;
}


UILinear::~UILinear()
{
}

UIElement* UILinear::CopyData() {

	auto n = new UILinear;

	n->linearFloat = linearFloat;

	return n;
}

void UILinear::OnLayout() {

	needLayout = false;
	needFindVisible = true;

	contentRect.Clear();
	lineHeights.clear();
	lineBreaks.clear();

	if (child.size() == 0)
		return;

	switch (linearFloat)
	{
	case LUI::UILinear::LinearFloat::None:
		LayoutFloatNone();
		break;
	case LUI::UILinear::LinearFloat::Left:
		LayoutFloatLeft();
		break;
	case LUI::UILinear::LinearFloat::Right:
		LayoutFloatRight();
		break;
	default:
		break;
	}

	maxScrollX = max(contentRect.Width - rect.Width, 0);
	maxScrollY = max(contentRect.Height - rect.Height, 0);
	if (scrollX > maxScrollX)scrollX = maxScrollX;
	if (scrollY > maxScrollY)scrollY = maxScrollY;

}

void UILinear::LayoutFloatNone() {

	lineHeights.resize(child.size() + 1);
	lineHeights[0] = 0.0f;
	lineBreaks.resize(child.size() + 1);

	for (int i = 0; i != child.size(); i++) {

		auto ch = child[i];

		if (ch->isHidden)
			continue;

		if (ch->needMeasure)
			ch->OnMeasure(rect.Width,rect.Height);

		float heightNeed = ch->margin.top + ch->margin.bottom + ch->rect.Height + ch->padding.top + ch->padding.bottom;

		lineHeights[i + 1] = lineHeights[i] + heightNeed;
		ch->relativePos.X = ch->margin.left + ch->padding.left;
		ch->relativePos.Y = ch->margin.top + ch->padding.top+lineHeights[i];

		lineBreaks[i] = i;

		LRectF r = { ch->relativePos.X - ch->margin.left - ch->padding.left,
			ch->relativePos.Y - ch->margin.top - ch->padding.top
			,ch->rect.Width + ch->margin.left + ch->margin.right + ch->padding.left + ch->padding.right
			,heightNeed };

		contentRect.Union(r);

	}

}

void UILinear::AlignToCenter(float maxHeight, int start, int count) {


	for (int i = start; i > start - count; i--) {

		auto ch = child[i];

		if (ch->isHidden)
			continue;

		float heightNeed = ch->rect.Height + ch->margin.top + ch->margin.bottom + ch->padding.top + ch->padding.bottom;

		ch->relativePos.Y += (maxHeight - heightNeed) / 2.0f + ch->margin.top + ch->padding.top;
	}

}
void UILinear::LayoutFloatLeft() {

	float currentInlineWidth = 0.0f;
	float maxHeightInline = 0.0f;
	int objCount = 0;

	lineHeights.push_back(0.0f);
	lineBreaks.push_back(0);

	for (int i = 0; i != child.size(); ++i) {

		auto ch = child[i];

		if (ch->isHidden) {
			continue;
			objCount++;
		}

		if (ch->needMeasure)
			ch->OnMeasure(rect.Width, rect.Height);

		float widthNeed = ch->rect.Width + ch->margin.left + ch->margin.right + ch->padding.left + ch->padding.right;
		float heightNeed = ch->rect.Height + ch->margin.top + ch->margin.bottom + ch->padding.top + ch->padding.bottom;

		if (widthNeed < 0.0f)
			widthNeed = 0.0f;

		if (heightNeed < 0.0f)
			heightNeed = 0.0f;

		switch (ch->layout.type) {

		case Layout::DisplayType::Block:
			if (objCount != 0) {
				contentRect.Width = max(contentRect.Width, currentInlineWidth);
				AlignToCenter(maxHeightInline, i - 1, objCount);
				lineHeights.push_back(lineHeights.back() + maxHeightInline);
				objCount = 0;
				lineBreaks.push_back(i - 1);
			}

			currentInlineWidth = 0.0f;
			maxHeightInline = 0.0f;

			ch->relativePos.X = ch->margin.left + ch->padding.left;
			ch->relativePos.Y = lineHeights.back() + ch->margin.top + ch->padding.top;
			lineHeights.push_back(lineHeights.back() + heightNeed);


			contentRect.Width = max(widthNeed, contentRect.Width);

			lineBreaks.push_back(i);
			break;

		case Layout::DisplayType::Inline:
			if (currentInlineWidth + widthNeed > rect.Width) {
				contentRect.Width = max(contentRect.Width, currentInlineWidth);

				if (objCount != 0) {
					AlignToCenter(maxHeightInline, i - 1, objCount);
					lineHeights.push_back(lineHeights.back() + maxHeightInline);
					objCount = 0;
					lineBreaks.push_back(i - 1);
				}
				currentInlineWidth = 0.0f;
				maxHeightInline = 0.0f;

			}
			ch->relativePos.X = currentInlineWidth + ch->margin.left + ch->padding.left;
			ch->relativePos.Y = lineHeights.back();
			currentInlineWidth += widthNeed;
			objCount++;
			if (maxHeightInline < heightNeed)
				maxHeightInline = heightNeed;

			break;
		}

	}

	AlignToCenter(maxHeightInline, (int)child.size() - 1, objCount);
	lineBreaks.push_back((int)child.size() - 1);
	lineHeights.push_back(lineHeights.back() + maxHeightInline);
	contentRect.Width = max(contentRect.Width, currentInlineWidth);
	contentRect.Height = lineHeights.back();
}

void UILinear::LayoutFloatRight() {

	float currentInlineWidth = 0.0f;
	float maxHeightInline = 0.0f;
	int objCount = 0;

	lineHeights.push_back(0.0f);
	lineBreaks.push_back(0);
	contentRect.Clear();
	contentRect.X = rect.Width;

	for (int i = 0; i != child.size(); ++i) {

		auto ch = child[i];

		if (ch->isHidden) {
			objCount++;
			continue;
		}
		if (ch->needMeasure)
			ch->OnMeasure(rect.Width, rect.Height);

		float widthNeed = ch->rect.Width + ch->margin.left + ch->margin.right + ch->padding.left + ch->padding.right;
		float heightNeed = ch->rect.Height + ch->margin.top + ch->margin.bottom + ch->padding.top + ch->padding.bottom;

		if (widthNeed < 0.0f)
			widthNeed = 0.0f;

		if (heightNeed < 0.0f)
			heightNeed = 0.0f;

		switch (layout.type) {
		case Layout::DisplayType::Block:

			if (objCount != 0) {
				AlignToCenter(maxHeightInline, i - 1, objCount);
				lineHeights.push_back(lineHeights.back() + maxHeightInline);
				objCount = 0;
				lineBreaks.push_back(i - 1);
			}

			currentInlineWidth = 0.0f;
			maxHeightInline = 0.0f;

			ch->relativePos.X = rect.Width - widthNeed + ch->margin.left + ch->padding.left;
			ch->relativePos.Y = lineHeights.back() + ch->margin.top+ch->padding.top;
			lineHeights.push_back(lineHeights.back() + heightNeed);

			contentRect.X = min(contentRect.X, rect.Width - currentInlineWidth);

			lineBreaks.push_back(i);
			break;

		case Layout::DisplayType::Inline:
			if (currentInlineWidth + widthNeed > rect.Width) {
				if (objCount != 0) {
					AlignToCenter(maxHeightInline, i - 1, objCount);
					lineHeights.push_back(lineHeights.back() + maxHeightInline);
					objCount = 0;
					lineBreaks.push_back(i - 1);
					contentRect.X = min(contentRect.X, rect.Width - currentInlineWidth);
				}
				currentInlineWidth = 0.0f;
				maxHeightInline = 0.0f;
			}

			currentInlineWidth += widthNeed;
			ch->relativePos.X = rect.Width - currentInlineWidth + ch->margin.left + ch->padding.left;
			ch->relativePos.Y = lineHeights.back();

			objCount++;

			if (maxHeightInline < heightNeed)
				maxHeightInline = heightNeed;

			break;
		}

	}

	AlignToCenter(maxHeightInline, (int)child.size() - 1, objCount);
	contentRect.X = min(contentRect.X, rect.Width - currentInlineWidth);
	lineBreaks.push_back((int)child.size() - 1);
	lineHeights.push_back(lineHeights.back() + maxHeightInline);
	contentRect.Height = lineHeights.back();
	contentRect.Width = rect.Width - contentRect.X;
	contentRect.X = 0;

	if (layout.widthWrapContent) {
		for (auto i : child) {
			i->relativePos.X -= rect.Width-contentRect.Width;
		}
	}

}

void UILinear::FindVisible() {
	
	needFindVisible = false;

	first = 0;
	last = (int)child.size();

	if (child.size() == 0)
		return;

	int lowerY = 0;

	if (child.size() > 20) {

		if (scrollY != 0.0f) {

			lowerY = (int)(std::lower_bound(lineHeights.data(), lineHeights.data() + lineHeights.size(), scrollY-10) - lineHeights.data());

			if (lowerY == lineHeights.size())
				return;


			if (lowerY != 0) {
				lowerY--;
				first = lineBreaks[lowerY];
			}

		}

	}

	float relativeBottom = scrollY + rect.Height + 10;
	int cur = first;


	for (int line = lowerY; line != lineBreaks.size(); line++) {

		if (line > 1 && lineHeights[line - 2] > relativeBottom) {
			last = cur;
			break;
		}

		for (; cur <= lineBreaks[line]; cur++) {

			auto ch = child[cur];

			if (ch->isHidden)
				continue;

			ch->needFindVisible = true;
			ch->rect.X = ch->relativePos.X - scrollX + rect.X;
			ch->rect.Y = ch->relativePos.Y - scrollY + rect.Y;

		}

	}

}

UIElement* UILinear::SelectObject(float x, float y) {
	for (auto i = last-1; i >= first; i--) {
		
		auto r = (child[i])->rect;
		auto &padding = (child[i])->padding;
		r.X -= padding.left;
		r.Y -= padding.top;

		r.Width += padding.left + padding.right;
		r.Height += padding.top + padding.bottom;
		if (r.Contains(x, y) && !child[i]->isHidden&&child[i]->display.opacity!=0.0f&&!child[i]->disabled) {
			return child[i];
		}
	}
	return nullptr;
}


void UILinear::SetAttr(std::string&attr, std::string&value) {

	enum class Attr {
		Error,
		flo,
	};

	static map<string, Attr>m = {
	{"float",Attr::flo},
	};


	switch (m[attr])
	{
	case Attr::flo:
		if (value == "left") {
			linearFloat = LinearFloat::Left;
		}
		else if (value == "right") {
			linearFloat = LinearFloat::Right;
		}
		else linearFloat = LinearFloat::None;
		break;
	
	default:
		__super::SetAttr(attr, value);
		break;
	}

}