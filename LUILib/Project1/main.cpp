#include<Window.h>
#include<iostream>
#include<Page.h>
#include<UIElement.h>
#include<UILinear.h>
#include<UIText.h>
#include<functional>

using namespace LUI;
using namespace std;

void LUIMain() {

	WindowSetting st;
	st.windowText = L"hh";
	st.overlap = false;
	st.resizable = true;
	auto w=new Window(st);
	w->Show();
	w->Center();

	w->MapOnClick("alert", [](Window*window)->void{
		auto e=((UIText*)(window->GetElement("gg")->before));
		e->text = L"1234";
		e->NeedMeasure();
	});

	w->MapOnKey("alert", [](Window*w, KeyEvent& e) {
		if (e.vk == VK_ESCAPE) {
			w->Close();
		}

	});

	w->MapOnChar("alert", [](Window*w, UINT32 c) {
		auto e = ((UIText*)(w->GetElement("gg")->before));
		e->text += c;
		e->NeedMeasure();
	});

}
