#include "stdafx.h"
#include "UIButton.h"
#include"Painter.h"
#include<iostream>
#include"Page.h"
#include "Window.h"
#include"Painter.h"
using namespace LUI;
using namespace std;

UIButton::UIButton()
{
	capturable = true;
	animType = 0;
	startTime = 0;
	color = D2D1::ColorF::LightBlue;
	startTime2 = 0;
}


UIButton::~UIButton()
{

}

void UIButton::SetAttr(std::string&attr, std::string&value) {

	enum class Attr {
		Error,
		color
	};

	static unordered_map<string, Attr>m = {
	{"color",Attr::color}
	};


	switch (m[attr])
	{
	case Attr::color:

		if (value[0] == '#') {
			sscanf_s(&value[1], "%x", &color);
		}

		break;
	default:
		__super::SetAttr(attr, value);
		break;
	}

}

bool UIButton::OnPaint(Painter*painter) {

	float percent = -1.0f;

	switch (animType)
	{
	case 1:
		percent = (globalTimer.TotalTime() - startTime) / 2.0f;
		break;
	case 2:
		percent = (startTime2 - startTime) / 2.0f + (globalTimer.TotalTime() - startTime2) / 0.3f;
		break;
	default:
		break;
	}

	if (percent < 0)
		return false;

	if (percent > 1.0f)
		percent = 1.0f;
	float radius = hypot(rect.Width + padding.left + padding.right, rect.Height + padding.top + padding.bottom) * percent;

	painter->brush->SetColor(D2D1::ColorF(color, 1.0f - percent));

	painter->target->FillEllipse(D2D1::Ellipse(
		D2D1::Point2F(p.x, p.y), radius, radius), painter->brush);

	if (percent == 1.0f) {
		animType = 0;
		return false;
	}

	return true;
}

bool UIButton::OnLButtonDown(float x, float y, Window*) {
	animType = 1;
	p.x = x;
	p.y = y;
	startTime = globalTimer.TotalTime();
	return true;
}

bool UIButton::OnClick(float x, float y, Window*) {

	animType = 2;
	startTime2 = globalTimer.TotalTime();

	return true;
}

UIElement* UIButton::CopyData() {

	auto n = new UIButton;

	n->color = color;

	return n;
}

bool CloseButton::OnPaint(Painter*painter) {

	bool b = __super::OnPaint(painter);

	painter->brush->SetColor(display.color);

	painter->target->DrawLine(D2D1::Point2F(rect.X + rect.Width*0.2f, rect.Y + rect.Height*0.2f),
		D2D1::Point2F(rect.X + rect.Width*0.8f, rect.Y + rect.Height*0.8f), painter->brush);

	painter->target->DrawLine(D2D1::Point2F(rect.X + rect.Width*0.2f, rect.Y + rect.Height*0.8f),
		D2D1::Point2F(rect.X + rect.Width*0.8f, rect.Y + rect.Height*0.2f), painter->brush);

	return b;

}

bool CloseButton::OnClick(float x, float y, Window*window) {

	__super::OnClick(x, y, window);

	window->Close();

	return true;
}

UIElement* CloseButton::CopyData() {

	auto n = new CloseButton;

	n->color = color;

	return n;
}

bool MiniButton::OnPaint(Painter*painter) {

	bool b = __super::OnPaint(painter);

	painter->brush->SetColor(display.color);

	painter->target->DrawLine(D2D1::Point2F(rect.X + rect.Width*0.2f, rect.Y + rect.Height*0.6f),
		D2D1::Point2F(rect.X + rect.Width*0.8f, rect.Y + rect.Height*0.6f), painter->brush, 2.f);

	return b;

}

bool MiniButton::OnClick(float x, float y, Window*window) {

	__super::OnClick(x, y, window);

	window->Minimize();

	window->SetCaptureElement(nullptr);

	return true;
}

UIElement* MiniButton::CopyData() {

	auto n = new MiniButton;

	n->color = color;

	return n;
}

bool MaxButton::OnPaint(Painter*painter) {

	bool b = __super::OnPaint(painter);

	painter->brush->SetColor(display.color);

	if (painter->window->state == WindowState::normal) {

		painter->target->DrawRectangle(D2D1::RectF(rect.X + rect.Width*0.2f, rect.Y + rect.Height*0.2f, rect.X + rect.Width*0.8f, rect.Y + rect.Height*0.8f), painter->brush);

	}
	else if (painter->window->state == WindowState::maxmize) {

		painter->target->DrawRectangle(D2D1::RectF(rect.X + rect.Width*0.2f, rect.Y + rect.Height*0.4f, rect.X + rect.Width*0.6f, rect.Y + rect.Height*0.8f), painter->brush);

		painter->target->DrawLine(D2D1::Point2F(rect.X + rect.Width*0.8f, rect.Y + rect.Height*0.2f),
			D2D1::Point2F(rect.X + rect.Width*0.4f, rect.Y + rect.Height*0.2f), painter->brush);

		painter->target->DrawLine(D2D1::Point2F(rect.X + rect.Width*0.8f, rect.Y + rect.Height*0.2f),
			D2D1::Point2F(rect.X + rect.Width*0.8f, rect.Y + rect.Height*0.6f), painter->brush);
		painter->target->DrawLine(D2D1::Point2F(rect.X + rect.Width*0.4f, rect.Y + rect.Height*0.4f),
			D2D1::Point2F(rect.X + rect.Width*0.4f, rect.Y + rect.Height*0.2f), painter->brush);
		painter->target->DrawLine(D2D1::Point2F(rect.X + rect.Width*0.8f, rect.Y + rect.Height*0.6f),
			D2D1::Point2F(rect.X + rect.Width*0.6f, rect.Y + rect.Height*0.6f), painter->brush);

	}



	return b;

}

bool MaxButton::OnClick(float x, float y, Window*window) {

	__super::OnClick(x, y, window);

	window->ToggleSize();

	window->SetCaptureElement(nullptr);

	return true;
}

UIElement* MaxButton::CopyData() {

	auto n = new MaxButton;

	n->color = color;

	return n;
}

Radio::Radio() {
	selectedColor = 0x008800;
	normalColor = 0x888888;
	radioSize = 20.0f;
	checked = false;
	capturable = true;
	animated = false;
	startTime = 0.0f;
}

void Radio::OnLayout() {

	needLayout = false;
	needFindVisible = true;

	contentRect = { 0,0,radioSize * 2,radioSize };

	if (child.size()) {
		auto c = child[0];
		if (c->needMeasure)
			c->OnMeasure(rect.Width, rect.Height);

		c->relativePos.X = radioSize * 2 + c->margin.left + c->padding.left;
		c->relativePos.Y = c->margin.top + c->padding.top;

		LRectF other = { c->relativePos.X - c->padding.left - c->margin.left,c->relativePos.Y - c->padding.top - c->margin.top,c->rect.Width + c->relativePos.X + c->padding.right + c->margin.right,c->rect.Height + c->relativePos.Y + c->padding.bottom + c->margin.bottom };
		contentRect.Union(other);
	}

	maxScrollX = max(contentRect.Width - rect.Width, 0);
	maxScrollY = max(contentRect.Height - rect.Height, 0);
	if (scrollX > maxScrollX)scrollX = maxScrollX;
	if (scrollY > maxScrollY)scrollY = maxScrollY;

}

void Radio::FindVisible() {
	first = 0;
	if (child.size()) {
		child[0]->needFindVisible = true;
		child[0]->rect.X = child[0]->relativePos.X + rect.X - scrollX;
		child[0]->rect.Y = child[0]->relativePos.Y + rect.Y - scrollY;
		last = 1;
	}
	else last = 0;

	needFindVisible = false;
}

bool Radio::OnClick(float, float, Window*) {

	if (checked)
		return false;

	checked = true;
	animated = true;

	startTime = globalTimer.TotalTime();

	if (parent&&parent->name == "radiogroup") {
		for (int i = 0; i < parent->child.size(); i++) {
			auto r = (Radio*)parent->child[i];
			if (r == this) {
				((RadioGroup*)parent)->selectedIndex = i;
			}
			else {
				if (r->checked == true) {
					r->animated = true;
					r->startTime = globalTimer.TotalTime();
					r->checked = false;
				}
			}
		}
	}

	return true;
}

bool Radio::OnPaint(Painter*painter) {


	if (animated) {
		auto color1 = D2D1::ColorF(normalColor);
		auto color2 = D2D1::ColorF(selectedColor);
		float radius;
		float percent = (globalTimer.TotalTime() - startTime) / 0.2f;
		if (percent > 1.0f)
			percent = 1.0f;
		else {
			percent = (cos(percent*3.1415926f) - 1) / -2.0f;
		}
		if (checked) {
			color1.a += (color2.a - color1.a)*percent;
			color1.b += (color2.b - color1.b)*percent;
			color1.r += (color2.r - color1.r)*percent;
			color1.g += (color2.g - color1.g)*percent;
			radius = percent * radioSize / 3;
			painter->brush->SetColor(color1);
		}
		else {
			color2.a += (color1.a - color2.a)*percent;
			color2.b += (color1.b - color2.b)*percent;
			color2.r += (color1.r - color2.r)*percent;
			color2.g += (color1.g - color2.g)*percent;

			radius = (1.0 - percent)*radioSize / 3;
			painter->brush->SetColor(color2);
		}


		painter->target->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(rect.X + 0.7f*radioSize, rect.Y + (rect.Height) / 2.f), radioSize / 2, radioSize / 2), painter->brush, 2);

		painter->target->FillEllipse(D2D1::Ellipse(D2D1::Point2F(rect.X + 0.7f*radioSize, rect.Y + (rect.Height) / 2.f), radius, radius), painter->brush);
		if (percent == 1.0f) {
			animated = false;
			return false;
		}
		else {
			return true;
		}
	}
	else {
		if (checked) {
			painter->brush->SetColor(D2D1::ColorF(selectedColor));
		}
		else {
			painter->brush->SetColor(D2D1::ColorF(normalColor));
		}

		painter->target->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(rect.X + 0.7f*radioSize, rect.Y + (rect.Height) / 2.f), radioSize / 2, radioSize / 2), painter->brush, 2);

		if (checked) {
			painter->target->FillEllipse(D2D1::Ellipse(D2D1::Point2F(rect.X + 0.7f*radioSize, rect.Y + (rect.Height) / 2.f), radioSize / 3, radioSize / 3), painter->brush);
		}
	}
	return false;

}

UIElement* Radio::CopyData() {

	auto e = new Radio;
	e->normalColor = normalColor;
	e->selectedColor = selectedColor;
	e->radioSize = radioSize;
	return e;
}

void Radio::SetAttr(std::string&attr, std::string&value) {

	enum class Attr {
		Error,
		radioSize,
		selectedColor,
		normalColor,
	};

	static unordered_map<string, Attr>m = {
		{ "size",Attr::radioSize },
	{ "selectedColor",Attr::selectedColor },
	{ "color",Attr::normalColor }
	};


	switch (m[attr])
	{
	case Attr::normalColor:
		if (value[0] == '#') {
			sscanf_s(&value[1], "%x", &normalColor);
		}
		break;
	case Attr::selectedColor:
		if (value[0] == '#') {
			sscanf_s(&value[1], "%x", &selectedColor);
		}
		break;
	case Attr::radioSize:
		sscanf_s(&value[0], "%f", &radioSize);
		break;
	default:
		__super::SetAttr(attr, value);
		break;
	}

}

RadioGroup::RadioGroup() {
	selectedIndex = -1;
}

UIElement* RadioGroup::CopyData() {
	auto e = new RadioGroup;

	e->linearFloat = linearFloat;

	return e;
}

void RadioGroup::SetChecked(int index) {

	if (index < child.size()) {


		for (auto i : child) {
			auto *r = (Radio*)i;
			if (r->checked == true) {
				r->animated = true;
				r->startTime = globalTimer.TotalTime();
				r->checked = false;
			}
		}
		if (index >= 0) {
			((Radio*)child[index])->checked = true;
		}

	}
}

CheckBox::CheckBox() {
	selectedColor = 0x008800;
	normalColor = 0x888888;
	radioSize = 20.0f;
	checked = false;
	capturable = true;
	animated = false;
	startTime = 0.0f;
}

void CheckBox::OnLayout() {

	needLayout = false;
	needFindVisible = true;

	contentRect = { 0,0,radioSize * 2,radioSize };

	if (child.size()) {
		auto c = child[0];
		if (c->needMeasure)
			c->OnMeasure(rect.Width, rect.Height);

		c->relativePos.X = radioSize * 2 + c->margin.left + c->padding.left;
		c->relativePos.Y = c->margin.top + c->padding.top;

		LRectF other = { c->relativePos.X - c->padding.left - c->margin.left,c->relativePos.Y - c->padding.top - c->margin.top,c->rect.Width + c->relativePos.X + c->padding.right + c->margin.right,c->rect.Height + c->relativePos.Y + c->padding.bottom + c->margin.bottom };
		contentRect.Union(other);
	}

	maxScrollX = max(contentRect.Width - rect.Width, 0);
	maxScrollY = max(contentRect.Height - rect.Height, 0);
	if (scrollX > maxScrollX)scrollX = maxScrollX;
	if (scrollY > maxScrollY)scrollY = maxScrollY;

}

void CheckBox::FindVisible() {
	first = 0;
	if (child.size()) {
		child[0]->needFindVisible = true;
		child[0]->rect.X = child[0]->relativePos.X + rect.X - scrollX;
		child[0]->rect.Y = child[0]->relativePos.Y + rect.Y - scrollY;
		last = 1;
	}
	else last = 0;

	needFindVisible = false;
}

bool CheckBox::OnClick(float, float, Window*) {


	checked = !checked;
	animated = true;

	startTime = globalTimer.TotalTime();

	return true;
}

bool CheckBox::OnPaint(Painter*painter) {


	if (animated) {
		auto color1 = D2D1::ColorF(normalColor);
		auto color2 = D2D1::ColorF(selectedColor);
		float radius;
		float percent = (globalTimer.TotalTime() - startTime) / 0.2f;
		if (percent > 1.0f)
			percent = 1.0f;
		else {
			percent = (cos(percent*3.1415926f) - 1) / -2.0f;
		}
		if (checked) {
			color1.a += (color2.a - color1.a)*percent;
			color1.b += (color2.b - color1.b)*percent;
			color1.r += (color2.r - color1.r)*percent;
			color1.g += (color2.g - color1.g)*percent;
			radius = percent * radioSize / 3;
			painter->brush->SetColor(color1);
		}
		else {
			color2.a += (color1.a - color2.a)*percent;
			color2.b += (color1.b - color2.b)*percent;
			color2.r += (color1.r - color2.r)*percent;
			color2.g += (color1.g - color2.g)*percent;

			radius = (1.0 - percent)*radioSize / 3;
			painter->brush->SetColor(color2);
		}

		float left = rect.X + 0.7f*radioSize - radioSize / 2.f;
		float top = rect.Y + (rect.Height - radioSize) / 2.f;

		painter->target->DrawRectangle(D2D1::RectF(left,top,left+ radioSize ,top+ radioSize), painter->brush,2);

		left = rect.X + 0.7f*radioSize - radius;
		top = rect.Y + (rect.Height) / 2.f -radius;

		painter->target->FillRectangle(D2D1::RectF(left, top, left + 2*radius, top + 2*radius), painter->brush);
		if (percent == 1.0f) {
			animated = false;
			return false;
		}
		else {
			return true;
		}
	}
	else {
		if (checked) {
			painter->brush->SetColor(D2D1::ColorF(selectedColor));
		}
		else {
			painter->brush->SetColor(D2D1::ColorF(normalColor));
		}

		float left = rect.X + 0.7f*radioSize - radioSize / 2.f;
		float top = rect.Y + (rect.Height - radioSize) / 2.f;

		painter->target->DrawRectangle(D2D1::RectF(left, top, left + radioSize, top + radioSize), painter->brush, 2);

		if (checked) {

			float radius = radioSize / 3;
			left = rect.X + 0.7f*radioSize - radius;
			top = rect.Y + (rect.Height) / 2.f - radius;
			painter->target->FillRectangle(D2D1::RectF(left, top, left + 2 * radius, top + 2 * radius), painter->brush);
		
		}
	}
	return false;

}

UIElement* CheckBox::CopyData() {

	auto e = new CheckBox;
	e->normalColor = normalColor;
	e->selectedColor = selectedColor;
	e->radioSize = radioSize;
	return e;
}

void CheckBox::SetAttr(std::string&attr, std::string&value) {

	enum class Attr {
		Error,
		radioSize,
		selectedColor,
		normalColor,
	};

	static unordered_map<string, Attr>m = {
		{ "size",Attr::radioSize },
	{ "selectedColor",Attr::selectedColor },
	{ "color",Attr::normalColor }
	};


	switch (m[attr])
	{
	case Attr::normalColor:
		if (value[0] == '#') {
			sscanf_s(&value[1], "%x", &normalColor);
		}
		break;
	case Attr::selectedColor:
		if (value[0] == '#') {
			sscanf_s(&value[1], "%x", &selectedColor);
		}
		break;
	case Attr::radioSize:
		sscanf_s(&value[0], "%f", &radioSize);
		break;
	default:
		__super::SetAttr(attr, value);
		break;
	}

}

Switch::Switch() {
	selectedColor = 0x008800;
	normalColor = 0x888888;
	radioSize = 20.0f;
	checked = false;
	capturable = true;
	animated = false;
	startTime = 0.0f;
}

void Switch::OnLayout() {

	needLayout = false;
	needFindVisible = true;

	contentRect = { 0,0,radioSize * 2,radioSize };

	maxScrollX = 0;
	maxScrollY = 0;
	scrollX = 0;
	scrollY = 0;

}

void Switch::FindVisible() {
	first = 0;
	last = 0;

	needFindVisible = false;
}

bool Switch::OnClick(float, float, Window*) {


	checked = !checked;
	animated = true;

	startTime = globalTimer.TotalTime();

	return true;
}

bool Switch::OnPaint(Painter*painter) {


	if (animated) {
		auto color1 = D2D1::ColorF(normalColor);
		auto color2 = D2D1::ColorF(selectedColor);
		float percent = (globalTimer.TotalTime() - startTime) / 0.2f;
		if (percent > 1.0f)
			percent = 1.0f;
		else {
			percent = (cos(percent*3.1415926f) - 1) / -2.0f;
		}
		if (checked) {
			color1.a += (color2.a - color1.a)*percent;
			color1.b += (color2.b - color1.b)*percent;
			color1.r += (color2.r - color1.r)*percent;
			color1.g += (color2.g - color1.g)*percent;
			painter->brush->SetColor(color1);
		}
		else {
			color2.a += (color1.a - color2.a)*percent;
			color2.b += (color1.b - color2.b)*percent;
			color2.r += (color1.r - color2.r)*percent;
			color2.g += (color1.g - color2.g)*percent;

			painter->brush->SetColor(color2);
		}

		painter->target->DrawLine(D2D1::Point2F(rect.X + (rect.Width) / 2.f - radioSize, rect.Y + rect.Height / 2),D2D1::Point2F(rect.X + (rect.Width ) / 2.f + radioSize, rect.Y+rect.Height/2),painter->brush,2);


		if (checked) {
			painter->target->FillEllipse(D2D1::Ellipse(D2D1::Point2F(rect.X + (rect.Width - radioSize) / 2.f + percent * radioSize, rect.Y + (rect.Height) / 2.f), radioSize / 2, radioSize / 2), painter->brush);
		}
		else {
			painter->target->FillEllipse(D2D1::Ellipse(D2D1::Point2F(rect.X + (rect.Width + radioSize) / 2.f - percent * radioSize, rect.Y + (rect.Height) / 2.f), radioSize / 2, radioSize / 2), painter->brush);

		}
		if (percent == 1.0f) {
			animated = false;
			return false;
		}
		else {
			return true;
		}
	}
	else {
		if (checked) {
			painter->brush->SetColor(D2D1::ColorF(selectedColor));
		}
		else {
			painter->brush->SetColor(D2D1::ColorF(normalColor));
		}

		float left = rect.X + 0.7f*radioSize - radioSize / 2.f;
		float top = rect.Y + (rect.Height - radioSize) / 2.f;

		painter->target->DrawLine(D2D1::Point2F(rect.X + (rect.Width ) / 2.f - radioSize, rect.Y + rect.Height / 2), D2D1::Point2F(rect.X + (rect.Width) / 2.f + radioSize, rect.Y + rect.Height / 2), painter->brush, 2);


		if(checked)
			painter->target->FillEllipse(D2D1::Ellipse(D2D1::Point2F(rect.X + (rect.Width +radioSize)/ 2.f, rect.Y + (rect.Height) / 2.f), radioSize / 2, radioSize / 2), painter->brush);
		else
			painter->target->FillEllipse(D2D1::Ellipse(D2D1::Point2F(rect.X + (rect.Width -radioSize)/ 2.f, rect.Y + (rect.Height) / 2.f), radioSize / 2, radioSize / 2), painter->brush);

	}
	return false;

}

UIElement* Switch::CopyData() {

	auto e = new Switch;
	e->normalColor = normalColor;
	e->selectedColor = selectedColor;
	e->radioSize = radioSize;
	return e;
}

void Switch::SetAttr(std::string&attr, std::string&value) {

	enum class Attr {
		Error,
		radioSize,
		selectedColor,
		normalColor,
	};

	static unordered_map<string, Attr>m = {
		{ "size",Attr::radioSize },
	{ "selectedColor",Attr::selectedColor },
	{ "color",Attr::normalColor }
	};


	switch (m[attr])
	{
	case Attr::normalColor:
		if (value[0] == '#') {
			sscanf_s(&value[1], "%x", &normalColor);
		}
		break;
	case Attr::selectedColor:
		if (value[0] == '#') {
			sscanf_s(&value[1], "%x", &selectedColor);
		}
		break;
	case Attr::radioSize:
		sscanf_s(&value[0], "%f", &radioSize);
		break;
	default:
		__super::SetAttr(attr, value);
		break;
	}

}