#include "stdafx.h"
#include "Page.h"
#include"UIElement.h"

using namespace LUI;
using namespace std;

Page::Page()
{
	root = nullptr;
}



Page::~Page()
{
}


Page* Page::Copy() {

	Page *page = new Page;

	page->root = root->Copy(page);
	page->name = name;
	page->binds = binds;

	return page;
}