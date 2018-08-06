#pragma once
#include<d2d1.h>
#include<dwrite.h>
#include<string>
namespace LUI {

	enum class LayoutFlag {
		width,
		height,
		isLeft,
		isTop,
		left,
		top,
		right,
		bottom,
		margin,
		padding,
		size,
		weight,
		stretch,
		style,
		family,
		text,
		vertical,
		horizontal,
		type,
		widthWrapContent,
		heightWrapContent,
	};

	enum class DisplayFlag {

		duration,
		transform,
		color,
		radius,
		opacity,
		border,
		background,
		boxShadow,
		cursor
	};

	class Layout {
	public:
		Layout();

		void Union(Layout*);

		struct SzGenerator {
			float solid;
			float percent;
			float childPercent;
		};

		struct  SzRect
		{
			SzGenerator top, left, right, bottom;
		};

		SzGenerator width, height;
		
		bool widthWrapContent;
		bool heightWrapContent;
		bool isLeft;
		bool isTop;

		SzRect margin, padding;

		SzGenerator top, left, right, bottom;

		struct TextParam {
			
			float size;
			DWRITE_FONT_WEIGHT weight;
			DWRITE_FONT_STRETCH stretch;
			DWRITE_FONT_STYLE style;
			std::wstring fontFamily;
			DWRITE_TEXT_ALIGNMENT vertical;
			DWRITE_PARAGRAPH_ALIGNMENT horizontal;
			DWRITE_WORD_WRAPPING wrapping;
		}text;

		enum class DisplayType {
			Inline,
			Block,

		}type;

		unsigned int flag;
	};

	enum class BorderType
	{
		None,
		Left,
		Top,
		Right,
		Bottom,
		All
	};

	enum class CursorType {
		arrow,
		hand,
		wait,
		text
	};

	class Display {
	public:
		Display();

		void Union(Display*);

		float duration;

		struct Transform
		{
			bool use;
			float rotate;
			D2D1_POINT_2F center;
			D2D1_SIZE_F scale;
			D2D1_SIZE_F transform;
			D2D1_SIZE_F skew;
		}transform;
		
		D2D1_COLOR_F color;

		float radius;
		float opacity;

		struct Background {
			enum class Type {
				None,
				Solid,
				Linear,
				Image,
				Radial
			}type;
			std::string imageName;
			int count;
			D2D1_POINT_2F start;
			D2D1_POINT_2F end;
			D2D1_COLOR_F colors[4];
			float stops[4];
		}background;

		struct Border
		{
			D2D1_COLOR_F color;
			BorderType type;
			float size;
		}border;

		struct BoxShadow
		{
			BorderType type;
			int size;
			D2D1_COLOR_F color;
		}boxShadow;

		CursorType cursor;

		unsigned int flag;
	};
}

