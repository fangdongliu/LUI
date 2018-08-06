#include "stdafx.h"
#include "App.h"
#include"Window.h"
#include"UIElement.h"
#include"UILinear.h"
#include"UIText.h"
#include"Style.h"
#include"Page.h"
#include<d2d1.h>
#include"ResourceLoader.h"
#include<iostream>



using namespace D2D1;
using namespace LUI;
using namespace std;



extern void LUIMain();
extern void InitPages(unordered_map<string, Page*>&);


namespace {
	App *app = nullptr;

	void LoadFiles() {

		enum class FileType {
			error,
			layout,
			display,
			page,
			image
		};

		unordered_map<std::string, FileType>m = {
			{ "layout",FileType::layout },{ "display",FileType::display },
		{ "lui",FileType::page },{ "jpg",FileType::image },
		{ "bmp",FileType::image },{ "rle",FileType::image },
		{ "dib",FileType::image },{ "jpeg",FileType::image },
		{ "jpe",FileType::image },{ "png",FileType::image },
		{ "icon",FileType::image } };

		for (auto& i : fs::recursive_directory_iterator("app")) {

			if (fs::is_regular_file(i)) {
				int pos = i.path().string().find_last_of('.');
				if (pos != -1) {
					string s = i.path().string().substr(pos + 1);

					transform(s.begin(), s.end(), s.begin(), ::tolower);

					switch (m[s])
					{
					case FileType::layout:
						FDASSERT(LoadLayout(i.path().string()));
						break;
					case FileType::display:
						FDASSERT(LoadDisplay(i.path().string()));
						break;
					case FileType::page: {

						auto p = LoadPage(i.path().u8string());
						FDASSERT(p);
						if (p)
							app->pages[i.path().stem().u8string()] = p;

					}
						break;
					case FileType::image: {
						auto img = ImageLoader::Load(i.path().u8string());
						if (img)
							app->images[i.path().filename().u8string()] = img;
						FDASSERT(img);
					}
	   				    break;
					default:
						break;
					}

				}
			}

		}
	}

	void Init() {

		GetApp();

		CoInitialize(0);

		HRESULT hr = S_OK;

		if (!Window::Register())
			throw exception();

		if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &Painter::factory)))
			throw exception();


		if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)&UIText::factory)))
			throw exception();

		if (FAILED(CoCreateInstance(
			CLSID_WICImagingFactory,
			nullptr,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&ImageLoader::factory)
		)))
			throw exception("");

		LoadFiles();
	}

}

App* const LUI::GetApp() {
	if (!app)
		app = new App;
	return app;
}


App::App() {}

App::~App() {
	UnregisterClassW(L"fdwindow1", HINST_THISCOMPONET);
	UnregisterClassW(L"fdwindow2", HINST_THISCOMPONET);
	SafeRelease(&Painter::factory);
	CoUninitialize();
}



bool App::BuildHoverDisplay(std::string&name, Display&display) {

	string other;
	int start = 0;
	bool b = false;
	
	if (NextString(name, other, start)) {
		auto i = hoverDisplays[other];
		if (i) {
			b = true;
			display.Union(i);
		}
		else if (i = displays[other]) {
			display.Union(i);
		}
		while (NextString(name, other, start)) {
			i = hoverDisplays[other];
			if (i) {
				b = true;
				display.Union(i);
			}
			else if (i = displays[other]) {
				display.Union(i);
			}
		}
	}
	return b;


}
bool App::BuildNormalDisplay(std::string&name, Display&display) {

	string other;
	int start = 0;
	bool b = false;
	if (NextString(name, other, start)) {
		auto i = displays[other];
		if (i) {
			b = true;
			display.Union(i);
		}
		while (NextString(name, other, start)) {
			i = displays[other];
			if (i) {
				b = true;
				display.Union(i);
			}
		}
	}
	return b;


}
bool App::BuildFocusDisplay(std::string&name, Display&display) {

	string other;
	int start = 0;
	bool b = false;
	if (NextString(name, other, start)) {
		auto i = focusDisplays[other];
		if (i) {
			b = true;
			display.Union(i);
		}
		else if (i = displays[other]) {
			display.Union(i);
		}
		while (NextString(name, other, start)) {
			i = focusDisplays[other];
			if (i) {
				b = true;
				display.Union(i);
			}
			else if (i = displays[other]) {
				display.Union(i);
			}
		}
	}
	return b;


}
bool App::BuildCaptureDisplay(std::string&name, Display&display) {

	string other;
	int start = 0;
	bool b = false;
	if (NextString(name, other, start)) {
		auto i = captureDisplays[other];
		if (i) {
			b = true;
			display.Union(i);
		}
		else if (i = displays[other]) {
			display.Union(i);
		}
		while (NextString(name, other, start)) {
			i = captureDisplays[other];
			if (i) {
				b = true;
				display.Union(i);
			}
			else if (i = displays[other]) {
				display.Union(i);
			}
		}
	}
	return b;

}

void App::ApplyStyle(UIElement*e) {

	string other;
	int start = 0;

	if (auto i = displays[e->name]) {
		e->display.Union(i);
	}

	if (NextString(e->displayClass, other, start)) {
		auto i = displays[other];
		if (i)
			e->display.Union(i);

		while (NextString(e->displayClass, other, start)) {
			i = displays[other];
			if (i)
				e->display.Union(i);
		}

	}
	start = 0;

	if (auto i = layouts[e->name]) {
		e->layout.Union(i);
	}

	if (NextString(e->layoutClass, other, start)) {

		auto i = layouts[other];
		if (i)
			e->layout.Union(i);

		while (NextString(e->layoutClass, other, start)) {
			i = layouts[other];
			if (i)
				e->layout.Union(i);
		}
	}


	for (auto i : e->child) {
		ApplyStyle(i);
	}

}

int App::Run() {
	MSG msg = { 0 };
	while (GetMessage(&msg, 0, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

void App::AddWindow(Window*window) {
	windowSet.insert(window);
}

void App::EraseWindow(Window*window) {
	windowSet.erase(window);
	if (windowSet.size() == 0) {
		PostQuitMessage(0);
	}
}

Page* App::GetPage(std::string pageName) {

	Page* page = new Page;

	auto i = pages[pageName];

	if (!i) {
		return page;
	}

	page->root = i->root->Copy();
	page->name = i->name;
	page->binds = i->binds;

	for (auto& e : i->dialogs) {
		auto& s = page->dialogs[e.first];
		s->root = e.second->root->Copy();
		s->name = e.second->name;
		s->binds = e.second->binds;
	}

	return page;

}

int main() {
	try {
		Init();
		InitPages(app->pages);
		for (auto& i : app->pages) {
			app->ApplyStyle(i.second->root);
		}
		LUIMain();
	}
	catch (exception&e) {
		MessageBoxA(0, e.what(), "error", MB_OK);
		return -1;
	}

	if (app->GetWindowCount()) {
		app->Run();
	}
	return 0;

}
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {

	try {
		Init();
		InitPages(app->pages);
		for (auto& i : app->pages) {
			app->ApplyStyle(i.second->root);
		}
		LUIMain();
	}
	catch (exception&e) {
		MessageBoxA(0, e.what(), "error", MB_OK);
		return -1;
	}

	if (app->GetWindowCount()) {
		app->Run();
	}
	return 0;
}