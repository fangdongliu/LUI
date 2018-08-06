#pragma once
#include<d2d1.h>
#include<unordered_map>
#include"LUITypes.h"
namespace LUI {
	class Window;
	class UIElement;
	class Display;
	class Painter
	{
	public:
		static ID2D1Factory* factory;
	public:
		Painter();
		~Painter();

		HRESULT CreateDeviceResource(Window*);
		void Release();
		void PushLayer(D2D1_LAYER_PARAMETERS&&);
		void PopLayer();
		ID2D1Bitmap* GetImage(std::string&);

		ID2D1HwndRenderTarget * target;
		ID2D1SolidColorBrush * brush;
		ID2D1BitmapBrush * bmpBrush;
		LRectF rect;
		bool Paint(UIElement*);
		bool PaintHint(IDWriteTextLayout*, float,D2D1_POINT_2F&);
	private:
		
		int currentLayer;
		ID2D1Layer * layers[100];
		std::unordered_map<std::string, ID2D1Bitmap*>images;

		
		void PaintBackground(UIElement*);
		void PaintBorder(UIElement*);
		void PaintShadow(UIElement*);
		void MakeAnim(UIElement*,Display&);

	};

}