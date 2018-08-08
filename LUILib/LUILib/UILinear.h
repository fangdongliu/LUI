#pragma once
#include"UIElement.h"
namespace LUI {
	class UILinear :public UIElement
	{
	public:
		UILinear();
		virtual ~UILinear();

		enum class LinearFloat
		{
			None,
			Left,
			Right
		};

	public:

		LinearFloat linearFloat;
		std::vector<float>lineHeights;
		std::vector<int>lineBreaks;

		UIElement * CopyData()override;
		void FindVisible()override;
		void OnLayout()override;
		UIElement* SelectObject(float x, float y)override;

	private:

		virtual void SetAttr(std::string&, std::string&);

		void			AlignToCenter(float maxHeight, int start, int count);
		void			LayoutFloatLeft();
		void			LayoutFloatRight();
		void			LayoutFloatNone();

	};

}