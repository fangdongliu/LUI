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


	/*

	class PageLoader {
	public:

		enum class Token {
			Error,
			Small,
			Great,
			Div,
			Str,
			String,
			Equal,
			End
		};
		
		char *s;
		Token token;

		std::string str;
		void NextToken();
		Page * LoadPage(std::string&&);

		UIElement*NewElement();

		bool LoadElement(UIElement*parent);

	};
*/
	namespace ImageLoader {

		extern IWICImagingFactory* factory;
		IWICFormatConverter * Load(std::string&&);

	};


}