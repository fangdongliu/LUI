#include "stdafx.h"
#include "Dealer.h"
#include"UIElement.h"
#include "App.h"
#include"Timer.h"
using namespace LUI;
using namespace std;


#define OtherValid(v) other.flag&(1<<(int)v)


namespace {
	App* app = GetApp();
	HCURSOR cHand = LoadCursor(0, IDC_HAND);

	HCURSOR cArrow = LoadCursor(0, IDC_ARROW);
	HCURSOR cText = LoadCursor(0, IDC_IBEAM);
	HCURSOR cWait = LoadCursor(0, IDC_WAIT);


}

Dealer::Dealer()
{
	root = nullptr;
	capture = nullptr;
	hover = nullptr;
	focus = nullptr;

}


Dealer::~Dealer()
{
}

bool Dealer::OnKey(int vk) {

	if (focus)
		return focus->OnKey(vk);

	return false;
}

bool Dealer::OnChar(wchar_t c) {

	if (focus)
		return focus->OnChar(c);

	return false;
}

bool Dealer::SetCaptureElement(UIElement*e) {

	if (e != capture) {

		if (capture) {

			capture->isCapture = false;

			Display ds;


			if (capture->isFocus) {
				if (app->BuildFocusDisplay(capture->name, ds) | app->BuildFocusDisplay(capture->displayClass, ds))
					MakeAnim(capture, ds);
			}
			else if (capture->isHover) {
				if (app->BuildHoverDisplay(capture->name, ds) | app->BuildHoverDisplay(capture->displayClass, ds))
					MakeAnim(capture, ds);
			}
			else
				if (app->BuildNormalDisplay(capture->name, ds) | app->BuildNormalDisplay(capture->displayClass, ds))
					MakeAnim(capture, ds);


		}

		if (e) {

			e->isCapture = true;

			Display ds;

			if (app->BuildCaptureDisplay(e->name, ds) | app->BuildCaptureDisplay(e->displayClass, ds)) {
				MakeAnim(e, ds);
			}

		}

		capture = e;

		return true;

	}

	return false;

}

bool Dealer::SetFocusElement(UIElement*e) {

	printf("setFocus\n");

	if (focus != e) {
		if (focus) {
			focus->isFocus = false;
			Display ds;

			if (!focus->isCapture) {

				if (focus->isHover) {
					if (app->BuildHoverDisplay(focus->name, ds) | app->BuildHoverDisplay(focus->displayClass, ds))
						MakeAnim(focus, ds);
				}
				else if (app->BuildNormalDisplay(focus->name, ds) | app->BuildNormalDisplay(focus->displayClass, ds))
					MakeAnim(focus, ds);

			}



		}

		if (e) {
			e->isFocus = true;

			if (!e->isCapture) {
				Display ds;
				if (app->BuildFocusDisplay(e->name, ds) | app->BuildFocusDisplay(e->displayClass, ds))
					MakeAnim(e, ds);
			}


		}
		focus = e;

		return true;
	}

	return false;

}
bool Dealer::SetHoverElement(UIElement*e) {
	if (hover != e) {

		if (hover) {
			auto r = hover;

			do {
				r->isHover = false;
				if (!r->isCapture && !r->isFocus) {
					Display ds;
					
					if (app->BuildNormalDisplay(r->name, ds)|app->BuildNormalDisplay(r->displayClass, ds))
						MakeAnim(r, ds);
				}
			} while (r = r->parent);

		}
		if (e) {
			switch (e->display.cursor)
			{
			case CursorType::arrow:
				SetCursor(cArrow);
				break;
			case CursorType::hand:
				SetCursor(cHand);
				break;
			case CursorType::text:
				SetCursor(cText);
				break;
			case CursorType::wait:
				SetCursor(cWait);
				break;
			default:
				break;
			}
			auto r = e;
			do {
				r->isHover = true;
				if (!r->isCapture && !r->isFocus) {
					Display ds;
					if (app->BuildHoverDisplay(r->name,ds)|app->BuildHoverDisplay(r->displayClass, ds))
						MakeAnim(r, ds);

				}
			} while (r = r->parent);

		}
		hover = e;

		return true;

	}

	return false;
}

bool Dealer::OnScroll(short delta) {

	UIElement* e;

	if (capture)
		e = capture;
	else if (hover)
		e = hover;
	else
		return false;

	do {
		float curr = e->scrollY + delta;

		if (curr < 0)
			curr = 0;
		else if (curr > e->maxScrollY)
			curr = e->maxScrollY;

		if (curr != e->scrollY)
			return true;

		e = e->parent;
	} while (e);

	return false;

}

bool Dealer::OnHScroll(short delta) {

	UIElement* e;


	if (capture)
		e = capture;
	else if (hover)
		e = hover;
	else
		return false;

	do {
		float curr = e->scrollX + delta;

		if (curr < 0)
			curr = 0;
		else if (curr > e->maxScrollX)
			curr = e->maxScrollX;

		if (curr != e->scrollX)
			return true;

		e = e->parent;
	} while (e);

	return false;

}

bool Dealer::OnLBtnDown(float x, float y) {

	bool b = false;

	vector<UIElement*>els;
	UIElement *e = root;

	if (!e)
		return false;

	do {
		els.push_back(e);
		e = e->SelectObject(x, y);
	} while (e);

	for (auto i = els.rbegin(); i != els.rend(); i++) {
		if ((*i)->capturable) {
			b |= (*i)->OnLButtonDown(x, y);
			b |= SetCaptureElement(*i);
			b |= SetHoverElement(*i);
			break;
		}
	}
	for (auto i = els.rbegin(); i != els.rend(); i++) {
		if ((*i)->focusable) {
			b |= SetFocusElement(*i);
			break;
		}
	}

	return b;
}

bool Dealer::OnLBtnUp(float x, float y) {

	bool b = false;

	if (capture&&capture->rect.Contains(x, y))
		b |= capture->OnClick(x, y);

	b |= SetCaptureElement(nullptr);

	return b;
}

bool Dealer::OnRBtnUp(float x, float y) {

	bool b = false;

	if (capture&&capture->rect.Contains(x, y))
		b |= capture->OnRightClick(x, y);

	b |= SetCaptureElement(nullptr);

	return b;
}

bool Dealer::OnRBtnDown(float x, float y) {

	bool b = false;

	vector<UIElement*>els;
	UIElement *e = root;

	if (!e) {
		return false;
	}

	do {
		els.push_back(e);
		e = e->SelectObject(x, y);
	} while (e);

	for (auto i = els.rbegin(); i != els.rend(); i++) {
		if ((*i)->capturable) {
			b |= SetCaptureElement(*i);
			b |= SetHoverElement(*i);
			break;
		}
	}
	for (auto i = els.rbegin(); i != els.rend(); i++) {
		if ((*i)->focusable) {
			b |= SetFocusElement(*i);
			break;
		}
	}

	return b;
}

bool Dealer::OnHover(float x, float y) {
	return false;
}

bool Dealer::OnMouseMove(float x, float y) {

	if (!root) {
		return false;
	}

	if (capture) {

		bool b = capture->OnMouseMove(x, y);

		if (capture->rect.Contains(x, y))
			return b | SetHoverElement(capture);
		else return b | SetHoverElement(nullptr);

	}
	else {
		vector<UIElement*>els;
		UIElement *e = root;

		do {
			els.push_back(e);
			e = e->SelectObject(x, y);
		} while (e);

		return SetHoverElement(els.back()) | els.back()->OnMouseMove(x, y);
	}

	return false;

}

void Dealer::MakeAnim(UIElement* e, Display&other) {

	auto& anim = e->anim;

	if (other.duration > 0.01) {
		e->anim.Release();
		e->anim.startTime = globalTimer.TotalTime();
		e->anim.duration = other.duration;
		if (OtherValid(DisplayFlag::background)) {
			if (e->display.background.type == other.background.type) {
				auto& start = e->display.background;
				auto& end = other.background;
				for (auto i = 0; i < end.count; i++) {
					anim.Add(start.colors[i], start.colors[i], end.colors[i]);
					anim.Add(start.stops[i], start.stops[i], end.stops[i]);
				}
				anim.Add(start.start, start.start, end.start);
				anim.Add(start.end, start.end, end.end);

				start.imageName = end.imageName;
				start.count = end.count;
			}
			else {
				e->display.background = other.background;
			}
		}
		else {
			e->display.background = other.background;
		}

		if (OtherValid(DisplayFlag::border)) {
			auto& start = e->display.border;
			auto& end = other.border;
			if (end.type == start.type) {
				anim.Add(start.color, start.color, end.color);
				anim.Add(start.size, start.size, end.size);
			}
			else {
				start = end;
			}
		}
		else e->display.border = other.border;

		if (OtherValid(DisplayFlag::color)) {

			anim.Add(e->display.color, e->display.color, other.color);

		}
		else e->display.color = other.color;

		if (OtherValid(DisplayFlag::opacity)) {

			anim.Add(e->display.opacity, e->display.opacity, other.opacity);
		}
		else e->display.opacity = other.opacity;

		if (OtherValid(DisplayFlag::radius)) {
			anim.Add(e->display.radius, e->display.radius, other.radius);
		}
		else e->display.radius = other.radius;

		if (OtherValid(DisplayFlag::transform)) {

			auto& start = e->display.transform;
			auto& end = other.transform;
			if (end.use) {
				anim.Add(start.center, start.center, end.center);
				anim.Add(start.rotate, start.rotate, end.rotate);
				anim.Add(start.skew, start.skew, end.skew);
				anim.Add(start.scale, start.scale, end.scale);
				anim.Add(start.transform, start.transform, end.transform);
			}
			start.use = end.use;
		}
		else e->display.transform = other.transform;

		if (OtherValid(DisplayFlag::boxShadow)) {

			auto& start = e->display.boxShadow;
			auto& end = other.boxShadow;
			if (start.type == end.type) {
				anim.Add(start.color, start.color, end.color);
				anim.Add(start.size, start.size, end.size);
			}
			else {
				start = end;
			}

		}
		else e->display.boxShadow = other.boxShadow;

	}
	else {
		e->anim.Release();
		e->display = other;
	}

	e->display.cursor = other.cursor;

}