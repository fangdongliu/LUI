#include "stdafx.h"
#include "UIImage.h"
#include"App.h"
#include"Painter.h"
using namespace LUI;
using namespace std;

namespace {
	App* app = GetApp();
}

UIImage::UIImage()
{
	bitmap = nullptr;
	layout.widthWrapContent = true;
	layout.heightWrapContent = true;
}


UIImage::~UIImage()
{
	SafeRelease(&bitmap);
}


void UIImage::OnLayout() {

	needLayout = false;
	needFindVisible = true;

	contentRect.Clear();

	SafeRelease(&bitmap);

	if (layout.widthWrapContent) {

		if (!bitmap) {
			if (auto img = app->images[src]) {
				UINT width, height;
				img->GetSize(&width,&height);
				rect.Width = width;
				rect.Height = height;
			}
			else {

			}
		}
		else {
			auto size=bitmap->GetSize();
			rect.Width = size.width;
			rect.Height = size.height;
		}
	}
	else if(layout.heightWrapContent){
		if (!bitmap) {
			if (auto img = app->images[src]) {
				UINT width, height;
				img->GetSize(&width, &height);
				rect.Height = rect.Width* height /width;
			}
			else {

			}
		}
		else {
			auto size = bitmap->GetSize();
			rect.Height = rect.Width* size.height / size.width;
		}
	}
	

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

	}

	maxScrollX = 0;
	maxScrollY = 0;
	scrollY = 0;
	scrollX = 0;
	contentRect = rect;

}

UIElement* UIImage::CopyData() {

	auto e = new UIImage;
	e->src = src;
	return e;

}

bool UIImage::OnPaint(Painter*painter) {

	if (bitmap) {
	}
	else if (auto img = app->images[src]) {
		painter->target->CreateBitmapFromWicBitmap(img, &bitmap);
	}
	else {
		return false;
	}

	auto size = bitmap->GetSize();

	painter->target->DrawBitmap(bitmap, D2D1::RectF(rect.X,rect.Y,rect.X+rect.Width,rect.Y+rect.Height), 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, D2D1::RectF(0,0,size.width,size.height));

	return false;

}

void UIImage::SetAttr(std::string&attr, std::string&value) {
	enum class Attr {
		Error,
		Src
	};

	static map<string, Attr>m = {
		{ "src",Attr::Src },
	};


	switch (m[attr])
	{
	case Attr::Src:
		src = value;
		break;
	default:
		__super::SetAttr(attr, value);
		break;
	}

}