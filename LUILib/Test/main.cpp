#include"stdafx.h"
#include<Window.h>
#include<iostream>
#include<Page.h>
#include<UIElement.h>
#include<UILinear.h>
#include<UIText.h>
using namespace LUI;
using namespace std;

void InitPages(unordered_map<string, Page*>&pages) {

	Page *page = new Page;
	page->name = "helloPage";

	{
		UILinear*root = new UILinear;
		root->linearFloat = UILinear::LinearFloat::Left;
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
		{
			UIText * c1 = new UIText;
			root->child.push_back(c1);
			c1->text = L"gaasdfggdmgfhlkg mkldgjmkl ,sdfkgh噶就是空手道妇女岁的妇女 lfm fgflkghfm, dgfhg,mfdsddfbn dmd,. md,ndfgfdfmesdfnmg bfdfksldms,fdng fgmdm,sddm, dfn ffgfb nm nkn mfgd";
			//		c1->capturable = true;
			//	c1->focusable = true;
			c1->parent = root;
			c1->displayClass = "rect";
			c1->layoutClass = "text";
		}
	}

	pages[page->name] = page;

}


void LUIMain() {

	WindowSetting st;
	st.windowText = L"hh";
	st.overlap = false;
	st.resizable = true;
	st.pageName = "tt";
	auto w = new Window(st);
	w->Show();
	w->Center();
}
