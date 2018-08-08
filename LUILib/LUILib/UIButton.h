#pragma once
#include"UILinear.h"

namespace LUI {
	class UIButton :public UIElement
	{
	public:
		UIButton();
		virtual ~UIButton();

		D2D1_POINT_2F p;
		float startTime;
		float startTime2;
		int animType;
		UINT color;

		void SetAttr(std::string&, std::string&)override;

		bool OnPaint(Painter*)override;

		bool OnLButtonDown(float x, float y, Window*)override;
		bool OnClick(float x, float y, Window*)override;

		UIElement* CopyData()override;

	};

	class CloseButton:public UIButton {
	public:
		CloseButton() {
			capturable = true;
		}
		bool OnPaint(Painter*)override;
		bool OnClick(float x, float y, Window*)override;
		UIElement* CopyData()override;
	};

	class MiniButton :public UIButton {
	public:
		MiniButton() {
			capturable = true;
		}
		bool OnPaint(Painter*)override;
		bool OnClick(float x, float y, Window*)override;
		UIElement* CopyData()override;

	};

	class MaxButton : public UIButton {
	public:
		MaxButton() {
			capturable = true;
		}
		bool OnPaint(Painter*)override;
		bool OnClick(float x, float y, Window*)override;
		UIElement* CopyData()override;

	};

	enum class GroupStyle {
		inlinegroup,
		multigroup
	};



	class Radio :public UIElement {
	public:
		Radio();

		bool animated;
		bool checked;
		float radioSize;
		UINT32 selectedColor;
		UINT32 normalColor;

		float startTime;


		bool OnPaint(Painter*)override;
		UIElement* CopyData()override;
		bool OnClick(float x, float y, Window*)override;
		void OnLayout()override;
		void FindVisible()override;
		void SetAttr(std::string&, std::string&)override;
		UIElement* SelectObject(float x, float y)override { return nullptr; }
	};

	class CheckBox :public UIElement {
	public:
		CheckBox();

		bool animated;
		bool checked;
		float radioSize;
		UINT32 selectedColor;
		UINT32 normalColor;

		float startTime;


		bool OnPaint(Painter*)override;
		UIElement* CopyData()override;
		bool OnClick(float x, float y, Window*)override;
		void OnLayout()override;
		void FindVisible()override;
		void SetAttr(std::string&, std::string&)override;
		UIElement* SelectObject(float x, float y)override { return nullptr; }
	};

	class Switch :public UIElement {
	public:
		Switch();

		bool animated;
		bool checked;
		float radioSize;
		UINT32 selectedColor;
		UINT32 normalColor;

		float startTime;


		bool OnPaint(Painter*)override;
		UIElement* CopyData()override;
		bool OnClick(float x, float y, Window*)override;
		void OnLayout()override;
		void FindVisible()override;
		void SetAttr(std::string&, std::string&)override;
		UIElement* SelectObject(float x, float y)override { return nullptr; }
	};

	class RadioGroup :public UILinear {
	public:
		RadioGroup();

		void SetChecked(int index);

		int selectedIndex;

		UIElement* CopyData()override;

	};


}

