#ifndef RECT_HPP
#define RECT_HPP

#include <algorithm>
#include "defs.hpp"

namespace ks
{
	struct KSImage_API Rect
	{
		float x;
		float y;
		float width;
		float height;

		Rect();

		Rect(const float x, const float y, const float width, const float height);

		float getMaxX() const noexcept;

		float getMaxY() const noexcept;

		Rect unionWithOther(const Rect& rect) const noexcept;
	};
}

#endif // !RECT_HPP