#include "stdafx.h"
#include "Painter.h"
#include"Window.h"
#include"UIElement.h"
#include"App.h"
#include"UIText.h"
#include <iostream>

using namespace LUI;
using namespace D2D1;
using namespace std;


#define OtherValid(v) other.flag&(1<<(int)v)

namespace {
	App* app = GetApp();
}

ID2D1Factory *Painter::factory = nullptr;

Painter::Painter()
{
	target = nullptr;
	brush = nullptr;
	bmpBrush = nullptr;
	memset(&layers, 0, sizeof layers);
	currentLayer = 0;
}


Painter::~Painter()
{
	Release();
}

void Painter::Release() {
	SafeRelease(&brush);
	SafeRelease(&bmpBrush);
	SafeRelease(&target);

	for (auto& i : images) {
		SafeRelease(&i.second);
	}

	for (int i = 0; layers[i]; i++) {
		SafeRelease(&layers[i]);
	}

}

HRESULT Painter::CreateDeviceResource(Window*window) {

	if (target)
		return S_OK;

	HRESULT hr = S_OK;

	auto& rect = window->rect;
	D2D1_SIZE_U size = SizeU(rect.right - rect.left, rect.bottom - rect.top);

	hr = factory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(window->GetHWnd(), size), &target);

	auto i = GetDpiForWindow(window->GetHWnd());
	target->SetDpi((float)i, (float)i);

	if (FAILED(hr))
		return hr;

	target->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE);
	target->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
	

	hr = target->CreateBitmapBrush(0, &bmpBrush);

	if (FAILED(hr))
		return hr;
	
	hr = target->CreateSolidColorBrush(D2D1::ColorF(0xffffff, 1.0f), &brush);

	return hr;

}

void Painter::PushLayer(D2D1_LAYER_PARAMETERS&& param) {

	if (!layers[currentLayer]) {
		target->CreateLayer(layers + currentLayer);
	}
	target->PushLayer(param, layers[currentLayer]);
	currentLayer++;

}

void Painter::PopLayer() {

	target->PopLayer();
	currentLayer--;

}

bool Painter::PaintHint(IDWriteTextLayout*text, float delta,D2D1_POINT_2F &hintPos) {
	if (!text) {
		return false;
	}
	float alpha = 0;
	if (delta < 0.3f) {
		alpha = delta/0.3f;
	}
	else if (delta > 3.0f) {
		alpha = (3.3f - delta) / 0.3f;
	}
	else alpha = 1.0f;



	DWRITE_TEXT_METRICS metrics;

	text->GetMetrics(&metrics);

	rect.Width -= 20;
	rect.Height -= 20;

	metrics.width += 10;
	metrics.height += 4;

	if (metrics.width + hintPos.x > rect.Width) {
		hintPos.x = rect.Width - metrics.width;
	}
	if (metrics.height + hintPos.y > rect.Height) {
		hintPos.y = rect.Height - metrics.height;
	}

	
	brush->SetColor(ColorF(0xf0f0f0,alpha));

	auto rc = D2D1::RectF(hintPos.x, hintPos.y, hintPos.x + metrics.width, hintPos.y + metrics.height);

	target->FillRectangle(rc, brush);


	brush->SetColor(ColorF(0, 0, 0, 0.3*alpha));

	target->DrawRectangle(rc, brush);


	brush->SetColor(ColorF(0, 0, 0, alpha));

	target->DrawTextLayout(D2D1::Point2F(hintPos.x+5,hintPos.y+2), text, brush);

	return true;
}

ID2D1Bitmap* Painter::GetImage(std::string&name) {

	return nullptr;

}

bool Painter::Paint(UIElement*e) {

	bool b = false;

	



	if (e->anim.animInt.size() || e->anim.animFloat.size()) {
		e->anim.Update(globalTimer.TotalTime());
		b = true;
	}

	if (e->display.opacity == 0.0f) {
		return false;
	}

	if (e->needLayout)
		e->OnLayout();

	D2D1_MATRIX_3X2_F lastTransform;


	if (e->display.transform.use) {
		target->GetTransform(&lastTransform);
		auto transform = e->display.transform;
		D2D1_POINT_2F center = { e->rect.X + e->rect.Width*transform.center.x,e->rect.Y + e->rect.Height*transform.center.y };

		target->SetTransform(lastTransform*
			Matrix3x2F::Scale(transform.scale, center)*
			Matrix3x2F::Rotation(transform.rotate, center)*
			Matrix3x2F::Skew(transform.skew.width, transform.skew.height, center)*
			Matrix3x2F::Translation(transform.transform));
	}

	

	PaintBackground(e);

	if (e->display.opacity == 1.0) {

		target->PushAxisAlignedClip(D2D1::RectF(e->rect.X, e->rect.Y, e->rect.X + e->rect.Width, e->rect.Y + e->rect.Height), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
	}
	else {


		PushLayer(D2D1::LayerParameters(D2D1::RectF(e->rect.X, e->rect.Y, e->rect.X + e->rect.Width, e->rect.Y + e->rect.Height), 0, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
			Matrix3x2F::Identity(), e->display.opacity));
	}
	



	b|=e->OnPaint(this);



	if (e->needFindVisible)
		e->FindVisible();

	for (int i = e->first; i != e->last; ++i) {
		if (rect.IntersectsWith(e->child[i]->rect))
			b |= Paint(e->child[i]);
	}

	if (e->display.opacity == 1.0) {
		target->PopAxisAlignedClip();
	}
	else PopLayer();

	PaintShadow(e);

	PaintBorder(e);


	if (e->display.transform.use) {
		target->SetTransform(lastTransform);
	}

	return b;
}

void Painter::PaintBackground(UIElement*e) {
	auto& rect = e->rect;
	auto&padding = e->padding;
	auto &display = e->display;

	D2D1_RECT_F rc = { rect.X - padding.left,rect.Y - padding.top,
		rect.X + rect.Width + padding.right,rect.Y + rect.Height + padding.bottom };

	switch (display.background.type)
	{
	case Display::Background::Type::None:
		break;
	case Display::Background::Type::Solid:

	{
		brush->SetColor(ColorF(display.background.colors[0].r,
			display.background.colors[0].g,
			display.background.colors[0].b,
			display.background.colors[0].a*display.opacity
		));
		if (display.radius >= 0.1f) {
			target->FillRoundedRectangle(D2D1::RoundedRect(rc, display.radius, display.radius), brush);
		}
		else {
			target->FillRectangle(rc, brush);
		}
	}
		break;
	case Display::Background::Type::Image: {

		auto &bmp = images[display.background.imageName];

		if (!bmp) {
			if (auto img = app->images[display.background.imageName]) {
				if (FAILED(target->CreateBitmapFromWicBitmap(img, &bmp)))
					return;
			}
			else return;

		}

		bmpBrush->SetBitmap(bmp);
		auto size = bmp->GetSize();
		size.height = rect.Height / size.height;
		size.width = rect.Width / size.width;
		bmpBrush->SetTransform(D2D1::Matrix3x2F::Scale(size)*D2D1::Matrix3x2F::Translation(D2D1::SizeF(rect.X, rect.Y)));
		if (display.radius >= 0.1f) {
			target->FillRoundedRectangle(D2D1::RoundedRect(rc, display.radius, display.radius), bmpBrush);
		}
		else {
			target->FillRectangle(rc, bmpBrush);
		}
	}

										   break;
	case Display::Background::Type::Linear:
	{
		ID2D1LinearGradientBrush *brush = nullptr;
		ID2D1GradientStopCollection *cll = nullptr;
		D2D1_GRADIENT_STOP stops[4];
		for (int i = 0; i < display.background.count; i++) {
			stops[i] = { display.background.stops[i],ColorF(display.background.colors[i].r,
				display.background.colors[i].g,
				display.background.colors[i].b,
				display.background.colors[i].a*display.opacity
			) };
		}

		if (FAILED(target->CreateGradientStopCollection(stops, display.background.count,D2D1_GAMMA_2_2,D2D1_EXTEND_MODE_MIRROR,&cll))) {
			return;
		}
		if (FAILED(
			target->CreateLinearGradientBrush(D2D1::LinearGradientBrushProperties(
				D2D1::Point2F(display.background.start.x*(rc.right - rc.left) + rc.left, display.background.start.y*(rc.bottom - rc.top) + rc.top),
				D2D1::Point2F(display.background.end.x*(rc.right - rc.left) + rc.left, display.background.end.y*(rc.bottom - rc.top) + rc.top)), cll, &brush))) {
			return;
		}

		if (display.radius >= 0.1f) {
			target->FillRoundedRectangle(D2D1::RoundedRect(rc, display.radius, display.radius), brush);
		}
		else {
			target->FillRectangle(rc, brush);
		}

		SafeRelease(&cll);
		SafeRelease(&brush);

		break;
	}
	case Display::Background::Type::Radial:
	{
		ID2D1RadialGradientBrush * brush = nullptr;
		ID2D1GradientStopCollection *cll = nullptr;
		D2D1_GRADIENT_STOP stops[4];
		for (int i = 0; i < display.background.count; i++) {
			stops[i] = { display.background.stops[i],ColorF(display.background.colors[i].r,
				display.background.colors[i].g,
				display.background.colors[i].b,
				display.background.colors[i].a*display.opacity
			) };
		}

		if (FAILED(target->CreateGradientStopCollection(stops, display.background.count, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_MIRROR, &cll))) {
			return;
		}


		if (FAILED(target->CreateRadialGradientBrush(D2D1::RadialGradientBrushProperties(
			D2D1::Point2F((rc.right + rc.left) / 2, (rc.top + rc.bottom) / 2), D2D1::Point2F(0, 0), (rc.right - rc.left) / 2, (rc.bottom - rc.top) / 2), cll, &brush))) {
			return;
		}

		if (display.radius >= 0.1f) {
			target->FillRoundedRectangle(D2D1::RoundedRect(rc, display.radius, display.radius), brush);
		}
		else {
			target->FillRectangle(rc, brush);
		}
		SafeRelease(&cll);
		SafeRelease(&brush);
	}
	break;
	default:
		break;
	}
}

void Painter::PaintBorder(UIElement*e) {

	if (e->display.border.size < 0.1f)
		return;

	brush->SetColor(e->display.border.color);
	auto offset = e->display.border.size / 2;
	auto rectangle = D2D1::RectF(e->rect.X - offset, e->rect.Y - offset, e->rect.X + e->rect.Width + offset, e->rect.Y + e->rect.Height + offset);
	switch (e->display.border.type)
	{
	case BorderType::All:
		if (e->display.radius) {
			target->DrawRoundedRectangle(D2D1::RoundedRect(rectangle,
				e->display.radius + offset, e->display.radius + offset), brush, e->display.border.size);
		}
		else
			target->DrawRectangle(rectangle, brush, e->display.border.size);
		break;
	case BorderType::Left:
		target->DrawLine(D2D1::Point2F(rectangle.left, rectangle.top + offset), D2D1::Point2F(rectangle.left, rectangle.bottom - offset), brush, e->display.border.size);
		break;
	case BorderType::Right:
		target->DrawLine(D2D1::Point2F(rectangle.right, rectangle.top + offset), D2D1::Point2F(rectangle.right, rectangle.bottom - offset), brush, e->display.border.size);
		break;
	case BorderType::Bottom:
		target->DrawLine(D2D1::Point2F(rectangle.left + offset, rectangle.bottom), D2D1::Point2F(rectangle.right - offset, rectangle.bottom), brush, e->display.border.size);
		break;
	case BorderType::Top:
		target->DrawLine(D2D1::Point2F(rectangle.left + offset, rectangle.top), D2D1::Point2F(rectangle.right - offset, rectangle.top), brush, e->display.border.size);
		break;

	default:
		break;
	}

}

void Painter::PaintShadow(UIElement*e) {
	if (e->display.boxShadow.size == 0)
		return;

	auto color = e->display.boxShadow.color;

	float a = color.a;

	float offset = 0.5;

	auto rectangle = D2D1::RectF(e->rect.X, e->rect.Y, e->rect.X + e->rect.Width, e->rect.Y + e->rect.Height);
	switch (e->display.boxShadow.type)
	{
	case BorderType::All:
		if (e->display.radius) {
			for (int i = e->display.boxShadow.size; i > 0; i--) {
				color.a = a * (i) / e->display.boxShadow.size / 3;

				brush->SetColor(color);

				target->DrawRoundedRectangle(D2D1::RoundedRect(rectangle,
					e->display.radius + offset, e->display.radius + offset), brush, 3);

				offset += 0.5;

				rectangle.left -= 1;
				rectangle.top -= 1;
				rectangle.right += 1;
				rectangle.bottom += 1;
			}
		}
		else {
			for (int i = e->display.boxShadow.size; i > 0; i--) {
				color.a = a * (i) / e->display.boxShadow.size / 3;

				brush->SetColor(color);

				target->DrawRectangle(rectangle, brush, 3);

				offset += 0.5;

				rectangle.left -= 1;
				rectangle.top -= 1;
				rectangle.right += 1;
				rectangle.bottom += 1;
			}

		}
		break;
	case BorderType::Left:
		target->DrawLine(D2D1::Point2F(rectangle.left, rectangle.top + offset), D2D1::Point2F(rectangle.left, rectangle.bottom - offset), brush, e->display.border.size);
		break;
	case BorderType::Right:
		target->DrawLine(D2D1::Point2F(rectangle.right, rectangle.top + offset), D2D1::Point2F(rectangle.right, rectangle.bottom - offset), brush, e->display.border.size);
		break;
	case BorderType::Bottom:
		target->DrawLine(D2D1::Point2F(rectangle.left + offset, rectangle.bottom), D2D1::Point2F(rectangle.right - offset, rectangle.bottom), brush, e->display.border.size);
		break;
	case BorderType::Top:
		target->DrawLine(D2D1::Point2F(rectangle.left + offset, rectangle.top), D2D1::Point2F(rectangle.right - offset, rectangle.top), brush, e->display.border.size);
		break;

	default:
		break;
	}
}

void Painter::MakeAnim(UIElement* e, Display&other) {

	auto& anim = e->anim;

	if (other.duration > 0.01) {

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

		if (OtherValid(DisplayFlag::color)) {

			anim.Add(e->display.color, e->display.color, other.color);

		}

		if (OtherValid(DisplayFlag::opacity)) {

			anim.Add(e->display.opacity, e->display.opacity, other.opacity);
		}

		if (OtherValid(DisplayFlag::radius)) {
			anim.Add(e->display.radius, e->display.radius, other.radius);
		}

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
			else {
				start.use = end.use;
			}
		}

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

	}
	else {
		e->display = other;
	}

}