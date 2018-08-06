#pragma once
#include<unordered_map>
namespace LUI {
	
	class UIElement;

	class Page
	{
	public:
		UIElement * root;
		std::string name;
		std::unordered_map<std::string, Page*>dialogs;
		std::vector<std::pair<std::string, std::pair<std::string,std::string>>>binds;

		Page();
		Page(Page& other);
		~Page();
	};

}