#include "stdafx.h"
#include "ResourceLoader.h"
#include"UILinear.h"
#include"UIText.h"
#include"App.h"
#include<d2d1.h>
#include"Page.h"
#include"UIInput.h"
#include"UIButton.h"
#include"UIDragable.h"
using namespace LUI;
using namespace D2D1;
using namespace std;

IWICImagingFactory *ImageLoader::factory = nullptr;

#define SetFlag(f,v) {f|=(1<<(int)v);}

namespace {

	App * app = GetApp();

	char * LoadFile(std::string& fileName) {

		FILE *file = nullptr;
		if (!fopen_s(&file, fileName.c_str(), "rb")) {

			fseek(file, 0, SEEK_END);
			int n = ftell(file);

			char *c = new char[n + 1];

			fseek(file, 0, SEEK_SET);

			fread(c, 1, n, file);

			fclose(file);

			c[n] = 0;

			return c;

		}
		return nullptr;

	}
}

#define ERRRR MessageBox(0,0,0,0)

namespace LayoutLoader {

	enum class Token {
		Error,
		End,
		Str,
		LeftBrace,
		RightBrace,
		Comma,
		Dot,
		Colon,
		Semi,
		Number,
		String,
		LeftParentHeses,
		RightParentHeses,
	};


	Layout::SzGenerator size;
	std::string str;
	Token token;
	float number;
	char *s;

	Token NextToken() {
		static int chartype[] = {
			-1,2,2,2,2,2,2,2,2,31,30,2,2,32,2,2,2,2,2,2,
			2,2,2,2,2,2,2,2,2,2,2,2,33,14,23,12,11,10,8,24,
			6,7,3,4,28,2,29,5,1,1,1,1,1,1,1,1,1,1,21,22,
			25,2,26,27,13,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,16,2,17,9,0,2,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,18,20,19,15,2,2,2,2,2,2,2,2,2,2,2,2,2,
			2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
			2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
			2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
			2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
			2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
			2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
		};

		while (chartype[*s] >= 30) {
			s++;
		}
		char c = *s++;
		switch (chartype[c])
		{
		case -1:
			return token = Token::End;
		case 0:
		{
			char* start = s - 1;
			int len = 1;
			while (chartype[*s] < 2) {
				s++;
				len++;
			}
			str.assign(start, len);
			return token = Token::Str;
		}
		case 1:
		{
			int num = c - '0';
			while (chartype[*s] == 1) {
				num = *s - '0' + num * 10;
				s++;
			}
			number = (float)num;
			if (*s == '.') {
				float power = 0.1f;
				s++;
				while (chartype[*s] == 1) {

					number += power * (*s - '0');
					s++;
					power *= 0.1f;

				}
			}

			return token = Token::Number;
		}
		case 2:
		{
			int num = 0;
			while (chartype[*s] == 1) {
				num = *s - '0' + num * 10;
				s++;
			}
			number = (float)num;
			if (*s == '.') {
				float power = 0.1f;
				s++;
				while (chartype[*s] == 1) {

					number += power * (*s - '0');
					s++;
					power *= 0.1f;

				}
			}

			number = -number;

			return token = Token::Number;
		}
		case 6:
			return token = Token::LeftParentHeses;
		case 7:
			return token = Token::RightParentHeses;
		case 18:
			return token = Token::LeftBrace;
		case 19:
			return token = Token::RightBrace;
		case 21:
			return token = Token::Colon;
		case 22:
			return token = Token::Semi;
		case 23:
		{
			char* start = s;
			int len = 0;
			while (chartype[*s] != 23 && *s) {
				s++;
				len++;
			}
			s++;
			str.assign(start, len);
		}
		return Token::String;
		case 24: {
			char* start = s;
			int len = 0;
			while (chartype[*s] != 24 && *s) {
				s++;
				len++;
			}
			s++;
			str.assign(start, len);
		}
				 return Token::String;
		case 28:
			return token = Token::Comma;
		case 29:
			return token = Token::Dot;
		default:
			return token = Token::Error;
		}

	}
	bool NextSz() {

		size = { 0 };

		switch (NextToken())
		{
		case Token::Semi:
			return true;
		case Token::Number:
			size.solid = number;
			return true;
		case Token::LeftParentHeses:
			break;
		default:
			return false;
		}

		switch (NextToken())
		{
		case Token::Number:
			size.solid = number;
			break;
		case Token::RightParentHeses:
			return true;
		default:
			return false;
		}

		switch (NextToken())
		{
		case Token::Comma:
			break;
		case Token::RightParentHeses:
			return true;
		default:
			return false;
		}

		switch (NextToken())
		{
		case Token::Number:
			size.percent = number;
			break;
		case Token::RightParentHeses:
			return true;
		default:
			return false;
		}

		switch (NextToken())
		{
		case Token::Comma:
			break;
		case Token::RightParentHeses:
			return true;
		default:
			return false;
		}

		switch (NextToken())
		{
		case Token::Number:
			size.childPercent = number;
			break;
		case Token::RightParentHeses:
			return true;
		default:
			return false;
		}

		return NextToken() == Token::RightParentHeses;

	}

}

namespace DisplayLoader {

	enum class Token {
		Error,
		End,
		Str,
		LeftBrace,
		RightBrace,
		Comma,
		Dot,
		Colon,
		Semi,
		Number,
		String,
		Color,
		LeftParentHeses,
		RightParentHeses,
	};

	D2D1_COLOR_F color;
	D2D1_POINT_2F point;
	std::string str;
	Token token;
	float number;
	char *s;

	Token NextToken() {
		static int chartype[] = {
			-1,2,2,2,2,2,2,2,2,31,30,2,2,32,2,2,2,2,2,2,
			2,2,2,2,2,2,2,2,2,2,2,2,33,14,23,12,11,10,8,24,
			6,7,3,4,28,2,29,5,1,1,1,1,1,1,1,1,1,1,21,22,
			25,2,26,27,13,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,16,2,17,9,0,2,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,18,20,19,15,2,2,2,2,2,2,2,2,2,2,2,2,2,
			2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
			2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
			2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
			2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
			2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
			2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
		};

		static int hexpower[] = {
			16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
			16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
			16,16,16,16,16,16,16,16,0,1,2,3,4,5,6,7,8,9,16,16,
			16,16,16,16,16,10,11,12,13,14,15,16,16,16,16,16,16,16,16,16,
			16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,10,11,12,
			13,14,15,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
			16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
			16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
			16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
			16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
			16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
			16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
			16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
		};

		while (chartype[*s] >= 30) {
			s++;
		}
		char c = *s++;
		switch (chartype[c])
		{
		case -1:
			return token = Token::End;
		case 0:
		{
			char* start = s - 1;
			int len = 1;
			while (chartype[*s] < 2) {
				s++;
				len++;
			}
			str.assign(start, len);
			return token = Token::Str;
		}
		case 1:
		{
			int num = c - '0';
			while (chartype[*s] == 1) {
				num = *s - '0' + num * 10;
				s++;
			}
			number = (float)num;
			if (*s == '.') {
				float power = 0.1f;
				s++;
				while (chartype[*s] == 1) {

					number += power * (*s - '0');
					s++;
					power *= 0.1f;

				}
			}

			return token = Token::Number;
		}
		case 2:
		{
			int num = 0;
			while (chartype[*s] == 1) {
				num = *s - '0' + num * 10;
				s++;
			}
			number = (float)num;
			if (*s == '.') {
				float power = 0.1f;
				s++;
				while (chartype[*s] == 1) {

					number += power * (*s - '0');
					s++;
					power *= 0.1f;

				}
			}

			number = -number;

			return token = Token::Number;
		}
		case 6:
			return token = Token::LeftParentHeses;
		case 7:
			return token = Token::RightParentHeses;
		case 12:
		{
			unsigned int num = 0;
			while (hexpower[*s] < 16) {
				num = hexpower[*s] + (num << 4);
				s++;
			}

			color = ColorF(num, (num >> 24) / 255.0f);
		}
		return token = Token::Color;
		case 18:
			return token = Token::LeftBrace;
		case 19:
			return token = Token::RightBrace;
		case 21:
			return token = Token::Colon;
		case 22:
			return token = Token::Semi;
		case 23:
		{
			char* start = s;
			int len = 0;
			while (chartype[*s] != 23 && *s) {
				s++;
				len++;
			}
			s++;
			str.assign(start, len);
		}
		return Token::String;
		case 24: {
			char* start = s;
			int len = 0;
			while (chartype[*s] != 24 && *s) {
				s++;
				len++;
			}
			s++;
			str.assign(start, len);
		}
				 return Token::String;
		case 28:
			return token = Token::Comma;
		case 29:
			return token = Token::Dot;
		default:
			return token = Token::Error;
		}

	}

	bool NextPoint() {

		if (NextToken() != Token::LeftParentHeses)
			return false;

		if (NextToken() != Token::Number)
			return false;

		point.x = number;

		if (NextToken() != Token::Comma)
			return false;

		if (NextToken() != Token::Number)
			return false;

		point.y = number;

		return NextToken() == Token::RightParentHeses;
	}

}

namespace PageLoader {

	enum class Token {
		Error,
		End,
		Str,
		Equal,
		Div,
		Great,
		Small,
		Colon,
		String
	};

	
	std::string str;
	Token token;
	char *s;
	string em = "";
	Token NextToken() {
		static int chartype[] = {
			-1,2,2,2,2,2,2,2,2,31,30,2,2,32,2,2,2,2,2,2,
			2,2,2,2,2,2,2,2,2,2,2,2,33,14,23,12,11,10,8,24,
			6,7,3,4,28,2,29,5,1,1,1,1,1,1,1,1,1,1,21,22,
			25,21,26,27,13,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,16,2,17,9,0,2,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,18,20,19,15,2,2,2,2,2,2,2,2,2,2,2,2,2,
			2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
			2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
			2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
			2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
			2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
			2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
		};


		while (chartype[*s] >= 30) {
			s++;
		}
		char c = *s++;
		switch (chartype[c])
		{
		case -1:
			return token = Token::End;
		case 0:
		{
			char* start = s - 1;
			int len = 1;
			while (chartype[*s] < 2) {
				s++;
				len++;
			}
			str.assign(start, len);
			return token = Token::Str;
		}
		case 5:
			return token = Token::Div;
		case 21:
			return token = Token::Equal;
		case 23:
		{
			char* start = s;
			int len = 0;
			while (chartype[*s] != 23 && *s) {
				s++;
				len++;
			}
			s++;
			str.assign(start, len);
		}
		return Token::String;
		case 24: {
			char* start = s;
			int len = 0;
			while (chartype[*s] != 24 && *s) {
				s++;
				len++;
			}
			s++;
			str.assign(start, len);
		}
				 return Token::String;
		case 25:
			return token = Token::Small;
		case 26:
			return token = Token::Great;
		default:
			return token = Token::Error;
		}

	}
	UIElement* CreateElement() {

		enum class Key {
			element,
			linear,
			table,
			text,
			input,
			button,
			close,
			draggable,
			maxmize,
			minimize,
			radio,
			radioGroup,
			checkbox,
			swit
		};

		unordered_map<string, Key>keyMap = {
			{"switch",Key::swit},
			{ "checkbox",Key::checkbox },
			{ "linear",Key::linear },
		{ "text",Key::text },
		{"input",Key::input},
		{"button",Key::button},
		{"close",Key::close},
		{"draggable",Key::draggable},
		{"maxmize",Key::maxmize},
		{"minimize",Key::minimize},
		{"radio",Key::radio},
		{"radiogroup",Key::radioGroup}
		};

		switch (keyMap[str])
		{
		case Key::swit: {
			auto e = new Switch;
			e->name = str;
			return e;
		}
		case Key::checkbox: {
			auto e = new CheckBox;
			e->name = str;
			return e;
		}
		case Key::radioGroup: {
			auto e = new RadioGroup;
			e->name = str;
			return e;
		}
		case Key::radio: {
			auto e = new Radio;
			e->name = str;
			return e;
		}
		case Key::maxmize: {
			auto e = new MaxButton;
			e->name = str;
			return e;
		}
		case Key::minimize: {
			auto e = new MiniButton;
			e->name = str;
			return e;
		}
		case Key::draggable: {
			auto e = new UIDragable;
			e->name = str;
			return e;
		}


		case Key::close: {
			auto e = new CloseButton;
			e->name = str;
			return e;
		}

		case Key::input: {

			auto e = new UIInput;
			e->name = str;
			return e;

		}
		case Key::button: {
			auto e = new UIButton;
			e->name = str;
			return e;
		}
		case Key::linear: {
			auto e = new UILinear;
			e->name = str;
			return e;
		}
	//	case Key::table:
	//		break;
		case Key::text: {
			auto e = new UIText;
			e->name = str;
			return e;
		}
		default: {
			auto e = new UIElement;
			e->name = str;
			return e;
		}
		}


	}

	bool LoadElement(UIElement*e) {

	decodeAttr:
		if (NextToken() == Token::Str) {

		decodeNext:
			string attr = str;

			if (NextToken() == Token::Equal) {
				if (NextToken() == Token::String) {
					e->SetAttr(attr, str);
					goto decodeAttr;
				}
				else {
					return false;
				}
			}
			else {
				e->SetAttr(attr, em);
				if (token == Token::Str) {
					goto decodeNext;
				}
				else if (token == Token::Div) {
					if (NextToken() != Token::Great)
						return  false;
					else return true;
				}
				else if (token != Token::Great) {
					return false;
				}
			}
		}
		else if (token == Token::Div) {
			if (NextToken() != Token::Great)
				return  false;
			else return true;
		}
		else if (token != Token::Great) {
			return false;
		}

	decodeChild:
		if (NextToken() != Token::Small) {
			return false;
		}

		if (NextToken() == Token::Div) {
			return NextToken() == Token::Str&&str == e->name&&NextToken() == Token::Great;
		}
		else if (token == Token::Str) {

			auto i = CreateElement();
			i->parent = e;
			e->child.push_back(i);

			if (!LoadElement(i))
				return false;

			goto decodeChild;
		}



	}


}

Page* LUI::LoadPage(std::string&&name) {

	using namespace PageLoader;

	Page* page = new Page;

	auto fs = s = LoadFile(name);

	if (!fs)
		return false;
	
	char bom[] = { (char)239,(char)187,(char)191,(char)254,(char)255 };

	if (fs[0] == bom[0] && fs[1] == bom[1] && fs[2] == bom[2]) {
		s += 3;
	}
	else if (fs[0] == bom[3] && fs[1] == bom[4]) {
		s += 2;
	}

	FDASSERT(NextToken() == Token::Small || token == Token::End);

	if (token == Token::End) {
		delete[]fs;
		return page;
	}

	FDASSERT(NextToken() == Token::Str);

	page->root = CreateElement();

	FDASSERT(LoadElement(page->root));

	FDASSERT(NextToken() == Token::End);

	delete[]fs;
	return page;

}



bool LUI::LoadLayout(std::string&&name) {

	using namespace LayoutLoader;

	enum class Key {
		Error,
		Left,
		Right,
		Top,
		Bottom,
		Width,
		Height,
		Margin,
		Padding,
		Wrap,
		Text
	};

	static unordered_map<string, Key>keyMap = {
		{ "left",Key::Left },{ "right",Key::Right },
	{ "top",Key::Top },{ "bottom",Key::Bottom },
	{ "margin",Key::Margin },{ "padding",Key::Padding },
	{ "width",Key::Width },{ "height",Key::Height },
	{ "wrap",Key::Wrap },{ "text",Key::Text } };

	auto fs = s = LoadFile(name);

	if (!fs)
		return false;

	std::vector<Layout*>layouts;

	char bom[] = { (char)239,(char)187,(char)191,(char)254,(char)255 };

	if (fs[0] == bom[0] && fs[1] == bom[1] && fs[2] == bom[2]) {
		s += 3;
	}
	else if (fs[0] == bom[3] && fs[1] == bom[4]) {
		s += 2;
	}

decodeNames:

	if (NextToken() != Token::Str) {
		delete[]fs;
		return LayoutLoader::token == Token::End;
	}

	auto &e = app->layouts[LayoutLoader::str];

	if (!e)
		e = new Layout;

	layouts.push_back(e);

	if (NextToken() == Token::Comma)
		goto decodeNames;

	if (LayoutLoader::token != Token::LeftBrace) {
		delete[]fs;
		return false;
	}

decodeAttrs:

	switch (NextToken())
	{
	case Token::Str:
		if (NextToken() != Token::Colon) {
			delete[]fs;
			return false;
		}
		break;
	case Token::Semi:
		goto decodeAttrs;
	case Token::RightBrace:
		layouts.clear();
		goto decodeNames;
	default:
		delete[]fs;
		return false;
	}

	switch (keyMap[LayoutLoader::str]) {
	case Key::Top:
		for (auto i : layouts) {

			SetFlag(i->flag, LayoutFlag::top);
			SetFlag(i->flag, LayoutFlag::isTop);
			i->isTop = true;

		}

		if (NextSz()) {
			for (auto i : layouts)
				i->top = LayoutLoader::size;
		}
		else {
			delete[]fs;
			return false;
		}
		if (LayoutLoader::token != Token::Semi)
			if (NextToken() != Token::Semi) {
				delete fs;
				return false;
			}

		goto decodeAttrs;
	case Key::Bottom:
		for (auto i : layouts) {

			SetFlag(i->flag, LayoutFlag::bottom);
			SetFlag(i->flag, LayoutFlag::isTop);
			i->isTop = false;

		}

		if (NextSz()) {
			for (auto i : layouts)
				i->bottom = LayoutLoader::size;
		}
		else {
			delete[]fs;
			return false;
		}
		if (LayoutLoader::token != Token::Semi)
			if (NextToken() != Token::Semi) {
				delete fs;
				return false;
			}

		goto decodeAttrs;
	case Key::Left:
		for (auto i : layouts) {

			SetFlag(i->flag, LayoutFlag::left);
			SetFlag(i->flag, LayoutFlag::isLeft);
			i->isLeft = true;

		}

		if (NextSz()) {
			for (auto i : layouts)
				i->left = LayoutLoader::size;
		}
		else {
			delete[]fs;
			return false;
		}
		if (LayoutLoader::token != Token::Semi)
			if (NextToken() != Token::Semi) {
				delete fs;
				return false;
			}

		goto decodeAttrs;
	case Key::Right:
		for (auto i : layouts) {

			SetFlag(i->flag, LayoutFlag::right);
			SetFlag(i->flag, LayoutFlag::isLeft);
			i->isLeft = false;

		}

		if (NextSz()) {
			for (auto i : layouts)
				i->right = LayoutLoader::size;
		}
		else {
			delete[]fs;
			return false;
		}
		if (LayoutLoader::token != Token::Semi)
			if (NextToken() != Token::Semi) {
				delete fs;
				return false;
			}

		goto decodeAttrs;
	case Key::Margin:
		for (auto i : layouts)
			SetFlag(i->flag, LayoutFlag::margin);

		if (NextSz()) {
			for (auto i : layouts)
				i->margin.top = LayoutLoader::size;
		}
		else {
			delete[]fs;
			return false;
		}
		if (LayoutLoader::token == Token::Semi)
			goto decodeAttrs;

		if (NextSz()) {
			for (auto i : layouts)
				i->margin.right = LayoutLoader::size;
		}
		else {
			delete[]fs;
			return false;
		}
		if (LayoutLoader::token == Token::Semi)
			goto decodeAttrs;

		if (NextSz()) {
			for (auto i : layouts)
				i->margin.bottom = LayoutLoader::size;
		}
		else {
			delete[]fs;
			return false;
		}
		if (LayoutLoader::token == Token::Semi)
			goto decodeAttrs;

		if (NextSz()) {
			for (auto i : layouts)
				i->margin.left = LayoutLoader::size;
		}
		else {
			delete[]fs;
			return false;
		}
		if (LayoutLoader::token != Token::Semi)
			if (NextToken() != Token::Semi) {
				delete[]fs;
				return false;
			}
		goto decodeAttrs;
	case Key::Padding:
		for (auto i : layouts)
			SetFlag(i->flag, LayoutFlag::padding);

		if (NextSz()) {
			for (auto i : layouts)
				i->padding.top = LayoutLoader::size;
		}
		else {
			delete[]fs;
			return false;
		}
		if (LayoutLoader::token == Token::Semi)
			goto decodeAttrs;

		if (NextSz()) {
			for (auto i : layouts)
				i->padding.right = LayoutLoader::size;
		}
		else {
			delete[]fs;
			return false;
		}
		if (LayoutLoader::token == Token::Semi)
			goto decodeAttrs;

		if (NextSz()) {
			for (auto i : layouts)
				i->padding.bottom = LayoutLoader::size;
		}
		else {
			delete[]fs;
			return false;
		}
		if (LayoutLoader::token == Token::Semi)
			goto decodeAttrs;

		if (NextSz()) {
			for (auto i : layouts)
				i->padding.left = LayoutLoader::size;
		}
		else {
			delete[]fs;
			return false;
		}
		if (LayoutLoader::token != Token::Semi)
			if (NextToken() != Token::Semi) {
				delete[]fs;
				return false;
			}
		goto decodeAttrs;
	case Key::Text:
		for (auto i : layouts) {
			SetFlag(i->flag, LayoutFlag::text);
		}
		if (NextToken() == Token::Number) {
			for (auto i : layouts)
				i->text.size = LayoutLoader::number;
		}
		else if (token == Token::Semi) {
			goto decodeAttrs;
		}
		else {
			delete[]fs;
			return false;
		}


		if (NextToken() == Token::String) {
			wstring s;
			UTF8ToUnicode(LayoutLoader::str.c_str(), s);

			for (auto i : layouts)
				i->text.fontFamily = s;
		}
		else if (token == Token::Semi) {
			goto decodeAttrs;
		}
		else {
			delete[]fs;
			return false;
		}

		if (NextToken() == Token::Number) {
			for (auto i : layouts)
				i->text.wrapping = (DWRITE_WORD_WRAPPING)(int)LayoutLoader::number;
		}
		else if (token == Token::Semi) {
			goto decodeAttrs;
		}
		else {
			delete[]fs;
			return false;
		}

		if (NextToken() == Token::Number) {
			for (auto i : layouts)
				i->text.weight = (DWRITE_FONT_WEIGHT)(int)LayoutLoader::number;
		}
		else if (token == Token::Semi) {
			goto decodeAttrs;
		}
		else {
			delete[]fs;
			return false;
		}

		if (NextToken() == Token::Number) {
			for (auto i : layouts)
				i->text.style = (DWRITE_FONT_STYLE)(int)LayoutLoader::number;
		}
		else if (token == Token::Semi) {
			goto decodeAttrs;
		}
		else {
			delete[]fs;
			return false;
		}

		if (NextToken() == Token::Number) {
			for (auto i : layouts)
				i->text.stretch = (DWRITE_FONT_STRETCH)(int)LayoutLoader::number;
		}
		else if (token == Token::Semi) {
			goto decodeAttrs;
		}
		else {
			delete[]fs;
			return false;
		}

		if (NextToken() == Token::Number) {
			for (auto i : layouts)
				i->text.vertical = (DWRITE_TEXT_ALIGNMENT)(int)LayoutLoader::number;
		}
		else if (token == Token::Semi) {
			goto decodeAttrs;
		}
		else {
			delete[]fs;
			return false;
		}

		if (NextToken() == Token::Number) {
			for (auto i : layouts)
				i->text.horizontal = (DWRITE_PARAGRAPH_ALIGNMENT)(int)LayoutLoader::number;
		}
		else if (token == Token::Semi) {
			goto decodeAttrs;
		}
		else {
			delete[]fs;
			return false;
		}

		if (NextToken() != Token::Semi) {
			delete[]fs;
			return false;
		}

		goto decodeAttrs;
	case Key::Width:
		for (auto i : layouts)
			SetFlag(i->flag, LayoutFlag::width);

		if (NextSz()) {
			for (auto i : layouts) {
				i->width = LayoutLoader::size;
				i->widthWrapContent = false;
				SetFlag(i->flag, LayoutFlag::widthWrapContent);
			}
		}
		else {
			delete[]fs;
			return false;
		}
		if (LayoutLoader::token != Token::Semi) {
			if (NextToken() == Token::Str&&keyMap[LayoutLoader::str] == Key::Wrap) {
				for (auto i : layouts) {
					i->widthWrapContent = true;
				}
			}
			else if (token != Token::Semi) {
				delete fs;
				return false;
			}


		}
		goto decodeAttrs;
	case Key::Height:
		for (auto i : layouts)
			SetFlag(i->flag, LayoutFlag::height);

		if (NextSz()) {
			for (auto i : layouts) {
				i->height = LayoutLoader::size;
				i->heightWrapContent = false;
				SetFlag(i->flag, LayoutFlag::heightWrapContent);
			}
		}
		else {
			delete[]fs;
			return false;
		}
		if (LayoutLoader::token != Token::Semi) {
			if (NextToken() == Token::Str&&keyMap[LayoutLoader::str] == Key::Wrap) {
				for (auto i : layouts) {
					i->heightWrapContent = true;
				}
			}
			else if (token != Token::Semi) {
				delete fs;
				return false;
			}
		}

		goto decodeAttrs;
	default:
		delete[]fs;
		return false;
	}
}

bool LUI::LoadDisplay(std::string&&name) {

	using namespace DisplayLoader;

	enum class Key {
		color,
		shadow,
		duration,
		radius,
		transform,
		opacity,
		cursor,
		border,
		background,
		linear,
		radial,
		all,
		left,
		right,
		top,
		bottom,
		image,
		center,
		scale,
		skew,
		rotate,
		offset,
		hover,
		capture,
		focus
	};

	static unordered_map<string, Key>keyMap = {
		{ "left",Key::left },{ "right",Key::right },
	{ "top",Key::top },{ "bottom",Key::bottom },
	{ "color",Key::color },{ "duration",Key::duration },
	{ "radius",Key::radius },{"transform",Key::transform},
	{ "opacity",Key::opacity },{ "border",Key::border },
	{"shadow",Key::shadow},{"cursor",Key::cursor},
	{ "background",Key::background },{ "linear",Key::linear },
	{ "radial",Key::radial },{"image",Key::image},
	{ "scale",Key::scale },{ "center",Key::center },
	{ "rotate",Key::rotate },{ "offset",Key::transform },
	{ "skew",Key::skew },{"capture",Key::capture},
	{"hover",Key::hover},{"focus",Key::focus}
	};




	auto fs = s = LoadFile(name);

	if (!fs)
		return false;

	char bom[] = { (char)239,(char)187,(char)191,(char)254,(char)255 };

	if (fs[0] == bom[0] && fs[1] == bom[1] && fs[2] == bom[2]) {
		s += 3;
	}
	else if (fs[0] == bom[3] && fs[1] == bom[4]) {
		s += 2;
	}

	std::vector<Display*>displays;

decodeNames:

	if (NextToken() != Token::Str) {
		delete[]fs;
		return DisplayLoader::token == Token::End;
	}

	if (NextToken() == Token::Colon) {
		auto s = DisplayLoader::str;
		if (NextToken() != Token::Str) {
			delete[]fs;
			return false;
		}

		switch (keyMap[DisplayLoader::str]) {
		case Key::hover: {
			auto &e = app->hoverDisplays[s];

			if (!e)
				e = new Display;

			displays.push_back(e);
			break;
		}
		case Key::focus: {
			auto &e = app->focusDisplays[s];

			if (!e)
				e = new Display;

			displays.push_back(e);
			break;
		}
		case Key::capture: {
			auto &e = app->captureDisplays[s];

			if (!e)
				e = new Display;

			displays.push_back(e);
			break;
		}
		default:
			delete[]fs;
			return false;

		}

		if (NextToken() == Token::Comma) {
			goto decodeNames;
		}
		else if (token != Token::LeftBrace) {
			delete[]fs;
			return false;
		}
	}
	else {
		auto &e = app->displays[DisplayLoader::str];

		if (!e)
			e = new Display;

		displays.push_back(e);

		if (token == Token::Comma) {
			goto decodeNames;
		}
		else if (token != Token::LeftBrace) {
			delete[]fs;
			return false;
		}

	}

decodeAttrs:

	switch (NextToken())
	{
	case Token::Str:
		if (NextToken() != Token::Colon) {
			delete[]fs;
			return false;
		}
		break;
	case Token::Semi:
		goto decodeAttrs;
	case Token::RightBrace:
		displays.clear();
		goto decodeNames;
	default:
		delete[]fs;
		return false;
	}

	switch (keyMap[DisplayLoader::str]) {
	case Key::color:
		if (NextToken() == Token::Color) {
			for (auto i : displays) {
				SetFlag(i->flag, DisplayFlag::color);
				i->color = DisplayLoader::color;
			}
			if (NextToken() != Token::Semi) {
				delete[]fs;
				return false;
			}
		}
		else {
			delete[]fs;
			return false;
		}
		goto decodeAttrs;
	case Key::cursor:
		if (NextToken() != Token::Str) {
			delete[]fs;
			return false;
		}
		for (auto i : displays) {
			SetFlag(i->flag, DisplayFlag::cursor);
		}
		if (str == "hand") {
			for (auto i : displays) {
				i->cursor = CursorType::hand;
			}
		}
		else if (str == "text") {
			for (auto i : displays) {
				i->cursor = CursorType::text;
			}
		}
		else if (str == "wait") {
			for (auto i : displays) {
				i->cursor = CursorType::wait;
			}
		}
		else {
			for (auto i : displays) {
				i->cursor = CursorType::arrow;
			}
		}

		if (NextToken() != Token::Semi) {
			delete[]fs;
			return false;
		}

		goto decodeAttrs;
	case Key::shadow:
		if (NextToken() == Token::Number) {
			for (auto i : displays) {
				SetFlag(i->flag, DisplayFlag::boxShadow);
				i->boxShadow.size = (int)DisplayLoader::number;
			}
		}
		else if (DisplayLoader::token != Token::Semi) {
			delete[]fs;
			return false;
		}

		if (NextToken() == Token::Color) {
			for (auto i : displays) {
				i->boxShadow.color = DisplayLoader::color;
			}
		}
		else if (DisplayLoader::token != Token::Semi) {
			delete[]fs;
			return false;
		}

		if (NextToken() == Token::Str) {

			switch (keyMap[DisplayLoader::str])
			{
			case Key::left:
				for (auto i : displays) {
					i->boxShadow.type = BorderType::Left;
				}
				break;
			case Key::right:
				for (auto i : displays) {
					i->boxShadow.type = BorderType::Right;
				}
				break;
			case Key::top:
				for (auto i : displays) {
					i->boxShadow.type = BorderType::Top;
				}
				break;
			case Key::bottom:
				for (auto i : displays) {
					i->boxShadow.type = BorderType::Bottom;
				}
				break;
			default:
				delete[]fs;
				return false;
			}
		}
		else if (token == Token::Semi) {
			for (auto i : displays) {
				i->boxShadow.type = BorderType::All;
			}
			goto decodeAttrs;
		}
		else {
			delete[]fs;
			return false;
		}

		if (NextToken() != Token::Semi) {
			delete[]fs;
			return false;
		}
		goto decodeAttrs;
	case Key::background:
		for (auto i : displays) {
			SetFlag(i->flag, DisplayFlag::background);
		}
		switch (NextToken())
		{
		case DisplayLoader::Token::Color:
			for (auto i : displays) {
				i->background.colors[0] = color;
				i->background.count = 1;
				i->background.type = Display::Background::Type::Solid;
			}
			break;
		case DisplayLoader::Token::Str:
			switch (keyMap[DisplayLoader::str])
			{
			case Key::image:
				if (NextToken() != Token::LeftParentHeses) {
					delete[]fs;
					return false;
				}
				if (NextToken() != Token::String) {
					delete[]fs;
					return false;
				}
				for (auto i : displays) {
					i->background.type = Display::Background::Type::Image;
					i->background.imageName = DisplayLoader::str.c_str();
				}
				if (NextToken() != Token::RightParentHeses) {
					delete[]fs;
					return false;
				}
				break;
			case Key::linear:
				if (NextToken() != Token::LeftParentHeses) {
					delete[]fs;
					return false;
				}

				for (int i = 0; i < 4; i++) {
					if (NextToken() != Token::Color) {
						delete[]fs;
						return false;
					}
					for (auto k : displays) {
						k->background.colors[i] = DisplayLoader::color;
					}
					if (NextToken() != Token::Number) {
						delete[]fs;
						return false;
					}
					for (auto k : displays) {
						k->background.stops[i] = DisplayLoader::number;
					}
					if (NextToken() == Token::RightParentHeses) {
						for (auto k : displays)
							k->background.count = i + 1;
						break;
					}
					else if (token != Token::Comma) {
						delete[]fs;
						return false;
					}
				}

				if (token != Token::RightParentHeses) {
					delete[]fs;
					return false;
				}

				for (auto i : displays) {
					i->background.type = Display::Background::Type::Linear;
				}

				break;
			case Key::radial:
				if (NextToken() != Token::LeftParentHeses) {
					delete[]fs;
					return false;
				}
				for (int i = 0; i < 4; i++) {
					if (NextToken() != Token::Color) {
						delete[]fs;
						return false;
					}
					for (auto k : displays) {
						k->background.colors[i] = DisplayLoader::color;
					}
					if (NextToken() != Token::Number) {
						delete[]fs;
						return false;
					}
					for (auto k : displays) {
						k->background.stops[i] = DisplayLoader::number;
					}
					if (NextToken() == Token::RightParentHeses) {
						for (auto k : displays)
							k->background.count = i + 1;
						break;
					}
					else if (token != Token::Comma) {
						delete[]fs;
						return false;
					}
				}

				if (token != Token::RightParentHeses) {
					delete[]fs;
					return false;
				}

				for (auto i : displays) {
					i->background.type = Display::Background::Type::Radial;
				}
				break;
			default:
				delete[]fs;
				return false;
			}
			break;
		default:
			delete[]fs;
			return false;
		}
		goto decodeAttrs;
	case Key::duration:
		if (NextToken() == Token::Number) {
			for (auto i : displays) {
				SetFlag(i->flag, DisplayFlag::duration);
				i->duration = DisplayLoader::number;
			}
			if (NextToken() != Token::Semi) {
				delete[]fs;
				return false;
			}
		}
		else {
			delete[]fs;
			return false;
		}
		goto decodeAttrs;
	case Key::radius:
		if (NextToken() == Token::Number) {
			for (auto i : displays) {
				SetFlag(i->flag, DisplayFlag::radius);
				i->radius = DisplayLoader::number;
			}
			if (NextToken() != Token::Semi) {
				delete[]fs;
				return false;
			}
		}
		else {
			delete[]fs;
			return false;
		}
		goto decodeAttrs;
	case Key::border:
		if (NextToken() == Token::Number) {
			for (auto i : displays) {
				SetFlag(i->flag, DisplayFlag::border);
				i->border.size = DisplayLoader::number;
			}
		}
		else if (DisplayLoader::token != Token::Semi) {
			delete[]fs;
			return false;
		}

		if (NextToken() == Token::Color) {
			for (auto i : displays) {
				i->border.color = DisplayLoader::color;
			}
		}
		else if (DisplayLoader::token != Token::Semi) {
			delete[]fs;
			return false;
		}

		if (NextToken() == Token::Str) {

			switch (keyMap[DisplayLoader::str])
			{
			case Key::left:
				for (auto i : displays) {
					i->border.type = BorderType::Left;
				}
				break;
			case Key::right:
				for (auto i : displays) {
					i->border.type = BorderType::Right;
				}
				break;
			case Key::top:
				for (auto i : displays) {
					i->border.type = BorderType::Top;
				}
				break;
			case Key::bottom:
				for (auto i : displays) {
					i->border.type = BorderType::Bottom;
				}
				break;
			default:
				delete[]fs;
				return false;
			}
		}
		else if (token == Token::Semi) {
			for (auto i : displays) {
				i->border.type = BorderType::All;
			}
			goto decodeAttrs;
		}
		else {
			delete[]fs;
			return false;
		}

		if (NextToken() != Token::Semi) {
			delete[]fs;
			return false;
		}
		goto decodeAttrs;
	case Key::transform:

		for (auto i : displays) {
			i->transform.use = true;
			SetFlag(i->flag, DisplayFlag::transform);
		}

	decodeTransform:

		if (NextToken() == Token::Str) {

			switch (keyMap[DisplayLoader::str])
			{
			case Key::center:

				if (NextPoint()) {
					for (auto i : displays)
						i->transform.center = point;

				}
				else {
					delete[]fs;
					return false;
				}
				goto decodeTransform;
			case Key::transform:
				if (NextPoint()) {
					for (auto i : displays) {
						i->transform.transform.width = point.x;
						i->transform.transform.height = point.y;
					}
				}
				else {
					delete[]fs;
					return false;
				}
				goto decodeTransform;
			case Key::skew:
				if (NextPoint()) {
					for (auto i : displays) {
						i->transform.skew.width = point.x;
						i->transform.skew.height = point.y;
					}
				}
				else {
					delete[]fs;
					return false;
				}
				goto decodeTransform;
			case Key::scale:
				if (NextPoint()) {
					for (auto i : displays) {
						i->transform.scale.width = point.x;
						i->transform.scale.height = point.y;
					}
				}
				else {
					delete[]fs;
					return false;
				}
				goto decodeTransform;
			case Key::rotate:
				if (NextToken() != Token::LeftParentHeses) {
					delete[]fs;
					return false;
				}
				if (NextToken() == Token::Number) {
					for (auto i : displays)
						i->transform.rotate = number;

				}
				else {
					delete[]fs;
					return false;
				}
				if (NextToken() != Token::RightParentHeses) {
					delete[]fs;
					return false;
				}

				goto decodeTransform;
			default:
				delete[]fs;
				return false;

			}

		}
		else if (token != Token::Semi) {
			delete[]fs;
			return false;
		}

		goto decodeAttrs;
	case Key::opacity:
		if (NextToken() == Token::Number) {
			for (auto i : displays) {
				SetFlag(i->flag, DisplayFlag::opacity);
				i->opacity = DisplayLoader::number;
			}
			if (NextToken() != Token::Semi) {
				delete[]fs;
				return false;
			}
		}
		else {
			delete[]fs;
			return false;
		}
		goto decodeAttrs;
	default:
		delete[]fs;
		return fs;
	}
}


IWICFormatConverter* ImageLoader::Load(std::string&&name) {

	HRESULT hr = S_OK;
	IWICBitmapDecoder *decoder = nullptr;
	IWICBitmapFrameDecode *pFrame = nullptr;
	IWICFormatConverter *converter = nullptr;
	wstring str;

	UTF8ToUnicode(name.c_str(), str);

	hr = factory->CreateDecoderFromFilename(str.c_str(), 0, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &decoder);
	if (!SUCCEEDED(hr)) {
		goto end;
	}
	hr = decoder->GetFrame(0, &pFrame);
	if (!SUCCEEDED(hr)) {
		goto end;
	}
	hr = factory->CreateFormatConverter(&converter);
	if (!SUCCEEDED(hr)) {
		goto end;
	}
	hr = converter->Initialize(
		pFrame,
		GUID_WICPixelFormat32bppPBGRA,
		WICBitmapDitherTypeNone,
		0,
		0.f,
		WICBitmapPaletteTypeCustom
	);
	if (!SUCCEEDED(hr)) {
		SafeRelease<IWICFormatConverter>(&converter);
		goto end;
	}
end:
	SafeRelease<IWICBitmapDecoder>(&decoder);
	SafeRelease<IWICBitmapFrameDecode>(&pFrame);

	return converter;
}