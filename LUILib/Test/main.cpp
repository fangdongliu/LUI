#include"stdafx.h"
#include<Window.h>
#include<iostream>
#include<Page.h>
#include<UIElement.h>
#include<UILinear.h>
using namespace LUI;
using namespace std;

void InitPages(unordered_map<string, Page*>&pages) {

	Page *page = new Page;
	page->name = "helloPage";

	{
		UILinear*root = new UILinear;
		root->linearFloat=UILinear::LinearFloat::Left;
		page->root = root;
		root->capturable = true;
		root->focusable = true;


		root->displayClass = "display3";
		root->layoutClass = "layout";


		{
			UIElement * c1 = new UIElement;
			root->child.push_back(c1);
			c1->capturable = true;
			c1->focusable = true;
			c1->parent = root;
			c1->displayClass = "display";
			c1->layoutClass = "layout2";
		}

		{
			UIElement * c1 = new UIElement;
			root->child.push_back(c1);
			c1->capturable = true;
			c1->focusable = true;
			c1->parent = root;
			c1->displayClass = "round yellow";
			c1->layoutClass = "layout3";
		}

		{
			UIElement * c1 = new UIElement;
			root->child.push_back(c1);
			c1->capturable = true;
			c1->focusable = true;
			c1->parent = root;
			c1->displayClass = "round sky";
			c1->layoutClass = "layout3";
		}

		{
			UIElement * c1 = new UIElement;
			root->child.push_back(c1);
			c1->capturable = true;
			c1->focusable = true;
			c1->parent = root;
			c1->displayClass = "dark round";
			c1->layoutClass = "layout3";
		}

		{
			UIElement * c1 = new UIElement;
			root->child.push_back(c1);
			c1->capturable = true;
			c1->focusable = true;
			c1->parent = root;
			c1->displayClass = "round purple";
			c1->layoutClass = "layout3";
		}

	}

	pages[page->name] = page;

}


void LUIMain() {

	(new Window(L"hhh", "helloPage"))->Show();
}
