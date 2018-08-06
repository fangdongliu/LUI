#include "stdafx.h"
#include "UIButton.h"
#include"Painter.h"
#include<iostream>
using namespace LUI;
using namespace std;

UIButton::UIButton()
{
	capturable = true;
	animType = 0;
	startTime = 0;
	startTime2 = 0;
}


UIButton::~UIButton()
{

}

bool UIButton::OnPaint(Painter*painter) {

	float percent = -1.0f;

	switch (animType)
	{
	case 1: 
		percent = (globalTimer.TotalTime() - startTime)/3.0f;
		break;
	case 2:
		percent= (startTime2 - startTime) / 3.0f+(globalTimer.TotalTime()-startTime2)/0.3f;
		break;
	default:
		break;
	}

	if (percent < 0)
		return false;

	if (percent > 1.0f) 
		percent = 1.0f;
	float radius = hypot(rect.Width, rect.Height) * percent;
	
	painter->brush->SetColor(D2D1::ColorF(D2D1::ColorF::Blue, 1.0-percent));

	painter->target->FillEllipse(D2D1::Ellipse(
		D2D1::Point2F(p.x, p.y), radius, radius),painter->brush);

	if (percent == 1.0f) {
		animType = 0;
		return false;
	}

	return true;
}

bool UIButton::OnLButtonDown(float x, float y) {
	animType = 1;
	p.x = x;
	p.y = y;
	startTime = globalTimer.TotalTime();
	return true;
}

bool UIButton::OnClick(float x, float y) {

	animType = 2;
	startTime2 = globalTimer.TotalTime();

	return true;
}

UIElement* UIButton::Copy() {

	auto n = new UIButton;

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
	n->hint = hint;

	n->focusable = focusable;
	n->capturable = capturable;
	n->disabled = disabled;

	scrollY = 0.0f;
	scrollX = 0.0f;

	return n;
}