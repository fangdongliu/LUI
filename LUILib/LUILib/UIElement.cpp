#include "stdafx.h"
#include "UIElement.h"
#include"Window.h"
#include"App.h"
#include"Page.h"
using namespace LUI;
using namespace std;
using namespace D2D1;



namespace {
	App*app = GetApp();

}


UIElement::UIElement()
{
	parent = nullptr;
	before = nullptr;
	next = nullptr;
	needMeasure = true;
	isHidden = false;

	focusable = false;
	capturable = false;
	disabled = false;

	isHover = false;
	isFocus = false;
	isCapture = false;

	scrollY = 0.0f;
	scrollX = 0.0f;

}

UIElement* UIElement::Copy(Page*page) {

	auto n = CopyData();

	for (auto i : child) {
		auto e = i->Copy(page);
		e->parent = n;
		n->child.push_back(e);
	}

	for (int i = n->child.size() - 1; i > 0; i--) {
		n->child[i]->before = n->child[i - 1];
		n->child[i - 1]->next = n->child[i];
	}

	if (id.length()) {
		n->id = id;
		page->ids[id] = n;
	}

	needMeasure = true;
	n->onchar = onchar;
	n->onkey = onkey;
	n->onclick = onclick;
	n->isHidden = isHidden;
	n->layout = layout;
	n->display = display;
	n->layoutClass = layoutClass;
	n->displayClass = displayClass;
	n->name = name;
	n->hint = hint;
	n->focusable = focusable;
	n->capturable = capturable;
	n->disabled = disabled;
	n->onclick = onclick;

	scrollY = 0.0f;
	scrollX = 0.0f;

	return n;

}

UIElement* UIElement::CopyData() {

	return new UIElement;
}


UIElement::~UIElement()
{
}

void UIElement::OnMeasure(float width, float height) {
	needMeasure = false;

	rect.Width = layout.width.solid + layout.width.percent*width;
	rect.Height = layout.height.solid + layout.height.percent*height;

	if (layout.widthWrapContent || layout.heightWrapContent) {

		OnLayout();

		if (layout.widthWrapContent) {
			rect.Width = contentRect.Width;
			maxScrollX = 0;
		}

		rect.Height += layout.height.childPercent*rect.Width;

		if (layout.heightWrapContent) {
			rect.Height = contentRect.Height;
			maxScrollY = 0;
		}

	}
	else {
		needLayout = true;
		rect.Height += layout.height.childPercent*rect.Width;

		for (auto i : child) {
			i->needMeasure = true;
		}
	}

	margin.left = layout.margin.left.solid + layout.margin.left.percent*width + layout.margin.left.childPercent*rect.Width;
	margin.top = layout.margin.top.solid + layout.margin.top.percent*height + layout.margin.top.childPercent*rect.Height;
	margin.right = layout.margin.right.solid + layout.margin.right.percent*width + layout.margin.right.childPercent*rect.Width;
	margin.bottom = layout.margin.bottom.solid + layout.margin.bottom.percent*height + layout.margin.bottom.childPercent*rect.Height;

	padding.left = layout.padding.left.solid + layout.padding.left.percent*width + layout.padding.left.childPercent*rect.Width;
	padding.top = layout.padding.top.solid + layout.padding.top.percent*height + layout.padding.top.childPercent*rect.Height;
	padding.right = layout.padding.right.solid + layout.padding.right.percent*width + layout.padding.right.childPercent*rect.Width;
	padding.bottom = layout.padding.bottom.solid + layout.padding.bottom.percent*height + layout.padding.bottom.childPercent*rect.Height;


}

void UIElement::OnLayout() {

	needLayout = false;
	needFindVisible = true;

	contentRect.Clear();

	for (auto i : child) {

		if (i->isHidden)
			return;

		if (i->needMeasure)
			i->OnMeasure(rect.Width, rect.Height);

		auto width = i->rect.Width;
		auto height = i->rect.Height;

		if (i->layout.isLeft) {
			auto& left = i->layout.left;
			i->relativePos.X = left.solid + left.percent*rect.Width + left.childPercent*width + i->padding.left;
		}
		else {
			auto& right = i->layout.right;
			i->relativePos.X = rect.Width - width - i->padding.right - right.solid - right.percent*rect.Width - right.childPercent*width;
		}


		if (i->layout.isTop) {
			auto& top = i->layout.top;
			i->relativePos.Y = top.solid + top.percent*rect.Height + top.childPercent*height + i->padding.top;
		}
		else {
			auto& bottom = i->layout.bottom;
			i->relativePos.Y = rect.Height - height - i->padding.bottom - bottom.solid - bottom.percent*rect.Height - bottom.childPercent*height;
		}

		LRectF other = { i->relativePos.X - i->padding.left,i->relativePos.Y - i->padding.top,i->rect.Width + i->padding.left + i->padding.right,i->rect.Height + i->padding.top + i->padding.bottom };
		contentRect.Union(other);
	}

	maxScrollX = max(contentRect.Width - rect.Width, 0);
	maxScrollY = max(contentRect.Height - rect.Height, 0);
	if (scrollX > maxScrollX)scrollX = maxScrollX;
	if (scrollY > maxScrollY)scrollY = maxScrollY;

}

void UIElement::FindVisible() {

	needFindVisible = false;

	first = 0;
	last = child.size();

	for (auto i : child) {

		if (i->isHidden)
			continue;

		i->needFindVisible = true;
		i->rect.X = i->relativePos.X + rect.X - scrollX;
		i->rect.Y = i->relativePos.Y + rect.Y - scrollY;

	}
}

UIElement* UIElement::SelectObject(float x, float y) {

	for (auto i = child.rbegin(); i != child.rend(); i++) {
		auto r = (*i)->rect;
		auto &padding = (*i)->padding;
		r.X -= padding.left;
		r.Y -= padding.top;

		r.Width += padding.left + padding.right;
		r.Height += padding.top + padding.bottom;

		if (r.Contains(x, y)&&!(*i)->isHidden&&(*i)->display.opacity!=0.0f&&!(*i)->disabled)
			return *i;
	}
	return nullptr;

}

void UIElement::NeedMeasure() {
	
	if (layout.widthWrapContent || layout.heightWrapContent) {
		needMeasure = true;
		if (parent) {
			parent->NeedMeasure();
		}
	}
	else {
		needLayout = true;
	}

}

void UIElement::SetAttr(std::string&attr, std::string&value) {

	enum class Attr {
		Error,
		layout,
		display,
		hide,
		focusable,
		capturable,
		disabled,
		id,
		hint,
		onclick,
		onkey,
		onchar,
	};

	static map<string, Attr>m = {
		{"onchar",Attr::onchar},
		{"onkey",Attr::onkey},
		{"layout",Attr::layout},
		{"display",Attr::display},
		{"hide",Attr::hide},
		{"focusable",Attr::focusable},
		{"capturable",Attr::capturable},
		{"disabled",Attr::disabled},
		{"id",Attr::id},
		{"hint",Attr::hint},
		{"onclick",Attr::onclick}
	};


	switch (m[attr])
	{
	case Attr::onchar:
		onchar = value;
		break;
	case Attr::onkey:
		onkey = value;
		break;
	case Attr::onclick:
		onclick = value;
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
	case Attr::focusable:
		focusable = value != "false";
		break;
	case Attr::capturable:
		capturable = value != "false";
		break;
	case Attr::disabled:
		disabled = value != "false";
		break;
	case Attr::id:
		id = value;
		break;
	case Attr::hint:
		UTF8ToUnicode(value.c_str(),hint);
		break;
	default:
		break;
	}

}

