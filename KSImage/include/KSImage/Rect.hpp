#ifndef RECT_HPP
#define RECT_HPP

#include <algorithm>

namespace ks
{
	struct Rect
	{
		float x;
		float y;
		float width;
		float height;

		Rect()
			:x(0), y(0), width(0), height(0)
		{

		}

		Rect(const float x, const float y, const float width, const float height)
			:x(x), y(y), width(width), height(height)
		{

		}

		float getMaxX() const noexcept
		{
			return x + width;
		}

		float getMaxY() const noexcept
		{
			return y + height;
		}

		Rect unionWithOther(const Rect& rect) const noexcept
		{
			Rect _rect;
			_rect.x = std::min(rect.x, x);
			_rect.y = std::min(rect.y, y);
			_rect.width = std::abs(std::min({ rect.x, rect.getMaxX(), x, getMaxX() }) - std::max({ rect.x, rect.getMaxX(), x, getMaxX() }));
			_rect.height = std::abs(std::min({ rect.y, rect.getMaxY(), y, getMaxY() }) - std::max({ rect.y, rect.getMaxY(), y, getMaxY() }));
			return _rect;
		}
	};
}

#endif // !RECT_HPP