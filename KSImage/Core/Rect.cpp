#include "Rect.hpp"

namespace ks
{
	Rect::Rect()
		:x(0), y(0), width(0), height(0)
	{

	}

	Rect::Rect(const float x, const float y, const float width, const float height)
		: x(x), y(y), width(width), height(height)
	{

	}

	float Rect::getMaxX() const noexcept
	{
		return x + width;
	}

	float Rect::getMaxY() const noexcept
	{
		return y + height;
	}

	Rect Rect::unionWithOther(const Rect & rect) const noexcept
	{
		Rect _rect;
		_rect.x = std::min(rect.x, x);
		_rect.y = std::min(rect.y, y);
		_rect.width = std::abs(std::min({ rect.x, rect.getMaxX(), x, getMaxX() }) - std::max({ rect.x, rect.getMaxX(), x, getMaxX() }));
		_rect.height = std::abs(std::min({ rect.y, rect.getMaxY(), y, getMaxY() }) - std::max({ rect.y, rect.getMaxY(), y, getMaxY() }));
		return _rect;
	}
}