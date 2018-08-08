#pragma once
#pragma once
#ifndef _FDTYPE_
#define _FDTYPE
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b)			(((a) < (b)) ? (a) : (b))
#endif
#define REAL_EPSILON        1.192092896e-07F        /* FLT_EPSILON */
#endif 
namespace LUI {

	class KeyEvent {
	public:
		KeyEvent(int vk_, bool heldShift_, bool heldControl_, bool heldAlt_) {
			vk = vk_;
			heldAlt = heldAlt_;
			heldControl = heldControl_;
			heldShift = heldShift_;
		}

		bool heldShift;
		bool heldAlt;
		bool heldControl;
		int vk;
	};

	class LSizeF
	{
	public:
		LSizeF()
		{
			Width = Height = 0.0f;
		}

		LSizeF(const LSizeF& size)
		{
			Width = size.Width;
			Height = size.Height;
		}

		LSizeF(float width,
			float height)
		{
			Width = width;
			Height = height;
		}

		LSizeF operator+(const LSizeF& sz) const
		{
			return LSizeF(Width + sz.Width,
				Height + sz.Height);
		}

		LSizeF operator-(const LSizeF& sz) const
		{
			return LSizeF(Width - sz.Width,
				Height - sz.Height);
		}

		bool Equals(const LSizeF& sz) const
		{
			return (Width == sz.Width) && (Height == sz.Height);
		}

		bool Empty() const
		{
			return (Width == 0.0f && Height == 0.0f);
		}

	public:

		float Width;
		float Height;
	};

	class Size
	{
	public:
		Size()
		{
			Width = Height = 0;
		}

		Size(const Size& size)
		{
			Width = size.Width;
			Height = size.Height;
		}

		Size(int width,
			int height)
		{
			Width = width;
			Height = height;
		}

		Size operator+(const Size& sz) const
		{
			return Size(Width + sz.Width,
				Height + sz.Height);
		}

		Size operator-(const Size& sz) const
		{
			return Size(Width - sz.Width,
				Height - sz.Height);
		}

		bool Equals(const Size& sz) const
		{
			return (Width == sz.Width) && (Height == sz.Height);
		}

		bool Empty() const
		{
			return (Width == 0 && Height == 0);
		}

	public:

		int Width;
		int Height;
	};

	class LPointF
	{
	public:
		LPointF()
		{
			X = Y = 0.0f;
		}

		LPointF(const LPointF &point)
		{
			X = point.X;
			Y = point.Y;
		}

		LPointF(const LSizeF &size)
		{
			X = size.Width;
			Y = size.Height;
		}

		LPointF(float x,
			float y)
		{
			X = x;
			Y = y;
		}

		LPointF operator+(const LPointF& point) const
		{
			return LPointF(X + point.X,
				Y + point.Y);
		}
		void operator +=(const LPointF&point) {
			X += point.X;
			Y += point.Y;
		}
		LPointF operator-(const LPointF& point) const
		{
			return LPointF(X - point.X,
				Y - point.Y);
		}

		bool Equals(const LPointF& point)
		{
			return (X == point.X) && (Y == point.Y);
		}

	public:

		float X;
		float Y;
	};

	class Point
	{
	public:
		Point()
		{
			X = Y = 0;
		}

		Point(const Point &point)
		{
			X = point.X;
			Y = point.Y;
		}

		Point(const Size &size)
		{
			X = size.Width;
			Y = size.Height;
		}

		Point(int x,
			int y)
		{
			X = x;
			Y = y;
		}

		Point operator+(const Point& point) const
		{
			return Point(X + point.X,
				Y + point.Y);
		}

		Point operator-(const Point& point) const
		{
			return Point(X - point.X,
				Y - point.Y);
		}

		bool Equals(const Point& point)
		{
			return (X == point.X) && (Y == point.Y);
		}

	public:

		int X;
		int Y;
	};

	class LRectF
	{
	public:

		LRectF()
		{
			X = Y = Width = Height = 0.0f;
		}

		LRectF(float x,
			float y,
			float width,
			float height)
		{
			X = x;
			Y = y;
			Width = width;
			Height = height;
		}

		LRectF(const LPointF& location,
			const LSizeF& size)
		{
			X = location.X;
			Y = location.Y;
			Width = size.Width;
			Height = size.Height;
		}

		LRectF* Clone() const
		{
			return new LRectF(X, Y, Width, Height);
		}

		void GetLocation(LPointF* point) const
		{
			point->X = X;
			point->Y = Y;
		}

		void GetSize(LSizeF* size) const
		{
			size->Width = Width;
			size->Height = Height;
		}

		void GetBounds(LRectF* rect) const
		{
			rect->X = X;
			rect->Y = Y;
			rect->Width = Width;
			rect->Height = Height;
		}

		inline float GetLeft() const
		{
			return X;
		}

		inline float GetTop() const
		{
			return Y;
		}

		inline float GetRight() const
		{
			return X + Width;
		}

		inline float GetBottom() const
		{
			return Y + Height;
		}

		bool IsEmptyArea() const
		{
			return (Width <= REAL_EPSILON) || (Height <= REAL_EPSILON);
		}

		bool Equals(const LRectF & rect) const
		{
			return X == rect.X &&
				Y == rect.Y &&
				Width == rect.Width &&
				Height == rect.Height;
		}

		inline bool Contains(float x,
			float y) const
		{
			return x >= X && x < X + Width &&
				y >= Y && y < Y + Height;
		}

		inline bool Contains(const LPointF& pt) const
		{
			return Contains(pt.X, pt.Y);
		}
		bool ContainWidthRadius(const LPointF&pt, const float radius) {
			if (Contains(pt)) {
				float relativeX = pt.X - X;
				if (relativeX < radius) {
					float relativeY = pt.Y - Y;
					if (relativeY < radius) {
						float disX = radius - relativeX;
						float disY = radius - relativeY;
						return radius * radius >= (disX*disX + disY * disY);
					}
					else if (relativeY > Height - radius) {
						float disX = radius - relativeX;
						float disY = radius + relativeY - Height;
						return radius * radius >= (disX*disX + disY * disY);
					}
					return true;
				}
				else if (relativeX > Width - radius) {
					float relativeY = pt.Y - Y;
					if (relativeY < radius) {
						float disX = radius + relativeX - Width;
						float disY = radius - relativeY;
						return radius * radius >= (disX*disX + disY * disY);
					}
					else if (relativeY > Height - radius) {
						float disX = radius + relativeX - Width;
						float disY = radius + relativeY - Height;
						return radius * radius >= (disX*disX + disY * disY);
					}
					return true;
				}
				return true;
			}
			return false;

		}
		bool Contains(const LRectF& rect) const
		{
			return (X <= rect.X) && (rect.GetRight() <= GetRight()) &&
				(Y <= rect.Y) && (rect.GetBottom() <= GetBottom());
		}

		void Inflate(float dx,
			float dy)
		{
			X -= dx;
			Y -= dy;
			Width += 2 * dx;
			Height += 2 * dy;
		}

		void Inflate(const LPointF& point)
		{
			Inflate(point.X, point.Y);
		}
		void Clear() {
			X = Y = Width = Height = 0.0f;
		}
		bool Intersect(const LRectF& rect)
		{
			return Intersect(*this, *this, rect);
		}

		static bool Intersect(LRectF& c,
			const LRectF& a,
			const LRectF& b)
		{
			float right = min(a.GetRight(), b.GetRight());
			float bottom = min(a.GetBottom(), b.GetBottom());
			float left = max(a.GetLeft(), b.GetLeft());
			float top = max(a.GetTop(), b.GetTop());

			c.X = left;
			c.Y = top;
			c.Width = right - left;
			c.Height = bottom - top;
			return !c.IsEmptyArea();
		}

		bool IntersectsWith(const LRectF& rect) const
		{
			return (GetLeft() < rect.GetRight() &&
				GetTop() < rect.GetBottom() &&
				GetRight() > rect.GetLeft() &&
				GetBottom() > rect.GetTop());
		}
		void Union(const LRectF&b) {
			float right = max(GetRight(), b.GetRight());
			float bottom = max(GetBottom(), b.GetBottom());
			float left = min(GetLeft(), b.GetLeft());
			float top = min(GetTop(), b.GetTop());
			X = left;
			Y = top;
			Width = right - left;
			Height = bottom - top;
		}
		void HorizontalUnion(float x, float width) {
			float right = max(GetRight(), x + width);
			float left = min(GetLeft(), x);
			X = left;
			Width = right - left;
		}
		static bool Union(LRectF& c,
			const LRectF& a,
			const LRectF& b)
		{
			float right = max(a.GetRight(), b.GetRight());
			float bottom = max(a.GetBottom(), b.GetBottom());
			float left = min(a.GetLeft(), b.GetLeft());
			float top = min(a.GetTop(), b.GetTop());

			c.X = left;
			c.Y = top;
			c.Width = right - left;
			c.Height = bottom - top;
			return !c.IsEmptyArea();
		}

		void Offset(const LPointF& point)
		{
			Offset(point.X, point.Y);
		}

		void Offset(float dx,
			float dy)
		{
			X += dx;
			Y += dy;
		}

	public:

		float X;
		float Y;
		float Width;
		float Height;
	};

	class Rect
	{
	public:

		Rect()
		{
			X = Y = Width = Height = 0;
		}

		Rect(int x,
			int y,
			int width,
			int height)
		{
			X = x;
			Y = y;
			Width = width;
			Height = height;
		}

		Rect(const Point& location,
			const Size& size)
		{
			X = location.X;
			Y = location.Y;
			Width = size.Width;
			Height = size.Height;
		}

		Rect* Clone() const
		{
			return new Rect(X, Y, Width, Height);
		}

		void GetLocation(Point* point) const
		{
			point->X = X;
			point->Y = Y;
		}

		void GetSize(Size* size) const
		{
			size->Width = Width;
			size->Height = Height;
		}

		void GetBounds(Rect* rect) const
		{
			rect->X = X;
			rect->Y = Y;
			rect->Width = Width;
			rect->Height = Height;
		}

		int GetLeft() const
		{
			return X;
		}

		int GetTop() const
		{
			return Y;
		}

		int GetRight() const
		{
			return X + Width;
		}

		int GetBottom() const
		{
			return Y + Height;
		}

		bool IsEmptyArea() const
		{
			return (Width <= 0) || (Height <= 0);
		}

		bool Equals(const Rect & rect) const
		{
			return X == rect.X &&
				Y == rect.Y &&
				Width == rect.Width &&
				Height == rect.Height;
		}

		bool Contains(int x,
			int y) const
		{
			return x >= X && x < X + Width &&
				y >= Y && y < Y + Height;
		}

		bool Contains(const Point& pt) const
		{
			return Contains(pt.X, pt.Y);
		}

		bool Contains(Rect& rect) const
		{
			return (X <= rect.X) && (rect.GetRight() <= GetRight()) &&
				(Y <= rect.Y) && (rect.GetBottom() <= GetBottom());
		}

		void Inflate(int dx,
			int dy)
		{
			X -= dx;
			Y -= dy;
			Width += 2 * dx;
			Height += 2 * dy;
		}

		void Inflate(const Point& point)
		{
			Inflate(point.X, point.Y);
		}

		bool Intersect(const Rect& rect)
		{
			return Intersect(*this, *this, rect);
		}

		static bool Intersect(Rect& c,
			const Rect& a,
			const Rect& b)
		{
			int right = min(a.GetRight(), b.GetRight());
			int bottom = min(a.GetBottom(), b.GetBottom());
			int left = max(a.GetLeft(), b.GetLeft());
			int top = max(a.GetTop(), b.GetTop());

			c.X = left;
			c.Y = top;
			c.Width = right - left;
			c.Height = bottom - top;
			return !c.IsEmptyArea();
		}

		bool IntersectsWith(const Rect& rect) const
		{
			return (GetLeft() < rect.GetRight() &&
				GetTop() < rect.GetBottom() &&
				GetRight() > rect.GetLeft() &&
				GetBottom() > rect.GetTop());
		}

		static bool Union(Rect& c,
			const Rect& a,
			const Rect& b)
		{
			int right = max(a.GetRight(), b.GetRight());
			int bottom = max(a.GetBottom(), b.GetBottom());
			int left = min(a.GetLeft(), b.GetLeft());
			int top = min(a.GetTop(), b.GetTop());

			c.X = left;
			c.Y = top;
			c.Width = right - left;
			c.Height = bottom - top;
			return !c.IsEmptyArea();
		}

		void Offset(const Point& point)
		{
			Offset(point.X, point.Y);
		}

		void Offset(int dx,
			int dy)
		{
			X += dx;
			Y += dy;
		}

	public:

		int X;
		int Y;
		int Width;
		int Height;
	};

	class RECTF {
	public:
		RECTF() { Left = Top = Right = Bottom = 0.0f; }
		RECTF(float left, float top, float right, float bottom) {
			Left = left;
			Top = top;
			Right = right;
			Bottom = bottom;
		}
		float Left, Top, Right, Bottom;
	};
}