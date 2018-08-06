#include "stdafx.h"
#include "Page.h"
#include"UIElement.h"

using namespace LUI;
using namespace std;

Page::Page()
{
	root = nullptr;
}

Page::Page(Page& other) {
	root = new UIElement(*other.root);

}


Page::~Page()
{
}
