#pragma once
#include<vector>
#include<d2d1.h>
namespace LUI {
	class UIElement;

	template<typename T>
	class AnimationObject {

	public:
		AnimationObject(T& dest_, T start_, T end_) :dest(dest_), start(start_), end(end_) {};

		inline void Update(float percent) {
			if (percent >= 1.0f)
				dest = end;
			else
				dest = (end - start)*percent + start;
		}
		T & dest;
		T start;
		T  end;
	};
	enum class AnimationType {

	};

	class Animation
	{
	public:
		Animation();
		~Animation();
		UIElement*dest;
		std::vector<AnimationObject<int>*>animInt;
		std::vector<AnimationObject<float>*>animFloat;
		float duration;
		float startTime;

		void Release();

		AnimationType type;

		void Update(float delta);

		template <typename T>
		inline void Add(T& dest, T start, T end) {}

		template<>
		inline void Add(int& dest, int start, int end) {
			if(start!=end)
				animInt.push_back(new AnimationObject<int>(dest, start, end));
		}
		template<>
		inline void Add(float& dest, float start, float end) {
			if (start != end)
				animFloat.push_back(new AnimationObject<float>(dest, start, end));
		}

		template<>
		inline void Add(D2D1_COLOR_F& dest, D2D1_COLOR_F start, D2D1_COLOR_F end) {

			Add(dest.a, start.a, end.a);
			Add(dest.r, start.r, end.r);
			Add(dest.g, start.g, end.g);
			Add(dest.b, start.b, end.b);

		}

		template<>
		inline void Add(D2D1_SIZE_F& dest, D2D1_SIZE_F start, D2D1_SIZE_F end) {
			Add(dest.width, start.width, end.width);
			Add(dest.height, start.height, end.height);
		}

		template<>
		inline void Add(D2D1_POINT_2F& dest, D2D1_POINT_2F start, D2D1_POINT_2F end) {
			Add(dest.x, start.x, end.x);
			Add(dest.y, start.y, end.y);
		}

	};

}