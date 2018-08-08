#pragma once
#include<unordered_map>
#include"Style.h"
namespace LUI {
	class Page;
	class UIElement;
	class ID2D1Bitmap;

	bool LoadLayout(std::string&&);
	bool LoadDisplay(std::string&&);
	Page* LoadPage(std::string&&);


	namespace ImageLoader {

		extern IWICImagingFactory* factory;
		IWICFormatConverter * Load(std::string&&);

	};


}