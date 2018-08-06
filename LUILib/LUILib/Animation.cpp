#include "stdafx.h"
#include "Animation.h"

using namespace LUI;

void Animation::Update(float cur) {

	float percent =(cur - startTime) / duration;

	if (percent > 1.0f) {

		for (auto i : animFloat)
			i->dest = i->end;

		for (auto i : animInt)
			i->dest = i->end;

		Release();
	}

	percent = (cos(percent*3.1415926f) - 1) / -2.0f;

	for (auto i : animFloat)
		i->dest = (i->end - i->start)*percent + i->start;



	for (auto i : animInt)
		i->dest = (i->end - i->start)*percent + i->start;


}


Animation::Animation()
{
}


Animation::~Animation()
{
	Release();
}

void Animation::Release() {

	for (auto i : animFloat)
		delete i;

	animFloat.clear();
	for (auto i : animInt)
		delete i;

	animInt.clear();
}