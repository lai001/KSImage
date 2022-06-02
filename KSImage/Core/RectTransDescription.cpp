#include "RectTransDescription.hpp"
#include <algorithm>

namespace ks
{
	RectTransDescription::RectTransDescription()
		:rect(Rect()), quad(Quadrilateral())
	{
	}

	RectTransDescription::RectTransDescription(Rect rect)
		:rect(rect)
	{
		quad.topLeft = glm::vec2(rect.x, rect.getMaxY());
		quad.topRight = glm::vec2(rect.getMaxX(), rect.getMaxY());
		quad.bottomLeft = glm::vec2(rect.x, rect.y);
		quad.bottomRight = glm::vec2(rect.getMaxX(), rect.y);
	}

	RectTransDescription::RectTransDescription(Quadrilateral quad)
		:quad(quad)
	{
		const glm::vec2 topLeft = quad.topLeft;
		const glm::vec2 topRight = quad.topRight;
		const glm::vec2 bottomLeft = quad.bottomLeft;
		const glm::vec2 bottomRight = quad.bottomRight;
		const float x = std::min(topLeft.x, bottomLeft.x);
		const float y = std::min(bottomLeft.y, bottomRight.y);
		const float width = std::max(topRight.x, bottomRight.x) - x;
		const float height = std::max(topLeft.y, topRight.y) - y;
		rect = Rect(x, y, width, height);
	}

	RectTransDescription RectTransDescription::applyTransform(const glm::mat3& t) noexcept
	{
		RectTransDescription rectTransDescription;
		glm::vec3 topLeft = t * glm::vec3(quad.topLeft, 1.0);
		glm::vec3 topRight = t * glm::vec3(quad.topRight, 1.0);
		glm::vec3 bottomLeft = t * glm::vec3(quad.bottomLeft, 1.0);
		glm::vec3 bottomRight = t * glm::vec3(quad.bottomRight, 1.0);
		rectTransDescription.quad.topLeft = topLeft;
		rectTransDescription.quad.topRight = topRight;
		rectTransDescription.quad.bottomLeft = bottomLeft;
		rectTransDescription.quad.bottomRight = bottomRight;
		const float minX = std::min({ topLeft.x, topRight.x, bottomLeft.x, bottomRight.x });
		const float minY = std::min({ topLeft.y, topRight.y, bottomLeft.y, bottomRight.y });
		const float maxX = std::max({ topLeft.x, topRight.x, bottomLeft.x, bottomRight.x });
		const float maxY = std::max({ topLeft.y, topRight.y, bottomLeft.y, bottomRight.y });
		const float width = maxX - minX;
		const float height = maxY - minY;
		rectTransDescription.rect = Rect(minX, minY, width, height);
		rectTransDescription.transform = t * transform;
		return rectTransDescription;
	}

	RectTransDescription RectTransDescription::translate(const glm::vec2& xy) noexcept
	{
		glm::mat3 identity = glm::identity<glm::mat3>();
		glm::mat3 t = glm::translate(identity, xy);
		return applyTransform(t);
	}

	RectTransDescription RectTransDescription::scale(const glm::vec2& xy) noexcept
	{
		glm::mat3 identity = glm::identity<glm::mat3>();
		glm::mat3 t = glm::scale(identity, xy);
		return applyTransform(t);
	}

	RectTransDescription RectTransDescription::scaleAround(const glm::vec2 & xy, const glm::vec2 & point) noexcept
	{
		glm::mat3 t0 = glm::translate(glm::identity<glm::mat3>(), glm::vec2(-point.x, -point.y));
		glm::mat3 s = glm::scale(glm::identity<glm::mat3>(), xy);
		glm::mat3 t1 = glm::inverse(t0);
		return applyTransform(t1 * s * t0);
	}

	RectTransDescription RectTransDescription::scaleAroundCenter(const glm::vec2 & xy) noexcept
	{
		const glm::vec2 point = glm::vec2(rect.width / 2.0 + rect.x, rect.height / 2.0 + rect.y);
		return scaleAround(xy, point);
	}

	RectTransDescription RectTransDescription::rotate(const float radian) noexcept
	{
		glm::mat3 identity = glm::identity<glm::mat3>();
		glm::mat3 t = glm::rotate(identity, radian);
		return applyTransform(t);
	}

	RectTransDescription RectTransDescription::rotateAroundCenter(const float radian) noexcept
	{
		const glm::vec2 point = glm::vec2(rect.width / 2.0 + rect.x, rect.height / 2.0 + rect.y);
		return rotateAround(radian, point);
	}

	RectTransDescription RectTransDescription::rotateAround(const float radian, const glm::vec2 & point) noexcept
	{
		glm::mat3 t0 = glm::translate(glm::identity<glm::mat3>(), glm::vec2(-point.x, -point.y));
		glm::mat3 r = glm::rotate<float>(glm::identity<glm::mat3>(), radian);
		glm::mat3 t1 = glm::inverse(t0);
		return applyTransform(t1 * r * t0);
	}

	Rect RectTransDescription::getBound() const noexcept
	{
		return rect;
	}

	Quadrilateral RectTransDescription::getQuad() const noexcept
	{
		return quad;
	}

	glm::mat3 RectTransDescription::getTransform() const noexcept
	{
		return transform;
	}
}