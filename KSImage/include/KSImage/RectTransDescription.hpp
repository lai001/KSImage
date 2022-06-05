#ifndef KSImage_RectTransDescription_hpp
#define KSImage_RectTransDescription_hpp

#include <glm/glm.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "defs.hpp"
#include "Rect.hpp"

namespace ks
{
	struct KSImage_API Quadrilateral
	{
		glm::vec2 topLeft;
		glm::vec2 topRight;
		glm::vec2 bottomLeft;
		glm::vec2 bottomRight;

		explicit Quadrilateral()
			:topLeft(glm::vec2()), topRight(glm::vec2()), bottomLeft(glm::vec2()), bottomRight(glm::vec2())
		{

		}
		explicit Quadrilateral(glm::vec2 topLeft, glm::vec2 topRight, glm::vec2 bottomLeft, glm::vec2 bottomRight)
			:topLeft(topLeft), topRight(topRight), bottomLeft(bottomLeft), bottomRight(bottomRight)
		{

		}
	};

	struct KSImage_API RectTransDescription
	{
	private:
		ks::Rect rect;
		ks::Quadrilateral quad;
		glm::mat3 transform = glm::identity<glm::mat3>();

	public:
		explicit RectTransDescription();

		explicit RectTransDescription(ks::Rect rect);

		explicit RectTransDescription(ks::Quadrilateral quad);

		RectTransDescription applyTransform(const glm::mat3& t) const noexcept;

		RectTransDescription translate(const glm::vec2& xy) const noexcept;

		RectTransDescription scale(const glm::vec2& xy) const noexcept;

		RectTransDescription scaleAround(const glm::vec2& xy, const glm::vec2& point) const noexcept;

		RectTransDescription scaleAroundCenter(const glm::vec2& xy) noexcept;

		RectTransDescription rotate(const float radian) const noexcept;

		RectTransDescription rotateAroundCenter(const float radian) const noexcept;

		RectTransDescription rotateAround(const float radian, const glm::vec2& point) const noexcept;

		ks::Rect getBound() const noexcept;

		ks::Quadrilateral getQuad() const noexcept;

		glm::mat3 getTransform() const noexcept;

		RectTransDescription newRect(const ks::Rect rect) const noexcept;
	};
}

#endif // !KSImage_RectTransDescription_hpp