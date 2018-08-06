#pragma once
#include<unordered_set>
#include<unordered_map>
#include <filesystem>

namespace fs = std::experimental::filesystem;

namespace LUI {

	class Window;
	class Layout;
	class Display;
	class UIElement;
	class Page;

	class App
	{
		friend App* const GetApp();

		App();
		~App();

	public:
		int Run();
		Page* GetPage(std::string pageName);

	public:
		std::unordered_map<std::string, Layout*> layouts;
		std::unordered_map<std::string, Display*> displays;
		std::unordered_map<std::string, Display*>hoverDisplays;
		std::unordered_map<std::string, Display*>focusDisplays;
		std::unordered_map<std::string, Display*>captureDisplays;
		std::unordered_map<std::string, IWICFormatConverter*> images;
		std::unordered_map<std::string, Page*> pages;
	private:
		std::unordered_set<Window*> windowSet;
	public:
		bool BuildHoverDisplay(std::string&, Display&);
		bool BuildNormalDisplay(std::string&, Display&);
		bool BuildFocusDisplay(std::string&, Display&);
		bool BuildCaptureDisplay(std::string&, Display&);
		void ApplyStyle(UIElement*);
		inline size_t GetWindowCount() { return windowSet.size(); }
		void AddWindow(Window*window);
		void EraseWindow(Window*window);
	};

	App * const GetApp();
}