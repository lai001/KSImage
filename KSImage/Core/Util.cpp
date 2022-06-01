#include "Util.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace ks
{
	Rect getUnionRect(const std::vector<ks::Image*>& images) noexcept
	{
		Rect rect;
		for (const auto& image : images)
		{
			rect = rect.unionWithOther(image->getRect());
		}
		return rect;
	}

	Rect KSImage_API makeRectAspectFit(const float aspectWidthRatio, const float aspectHeightRatio, const Rect & boundingRect) noexcept
	{
		float widthScale = boundingRect.width / aspectWidthRatio;
		float heightScale = boundingRect.height / aspectHeightRatio;
		float scale = std::min(widthScale, heightScale);
		const float width = aspectWidthRatio * scale;
		const float height = aspectHeightRatio * scale;
		const float x = (boundingRect.width - width) / 2.0 + boundingRect.x;
		const float y = (boundingRect.height - height) / 2.0 + boundingRect.y;
		return Rect(x, y, width, height);
	}

	ks::RectTransDescription KSImage_API convertToNDC(const ks::RectTransDescription& des) noexcept
	{
		return convertToNDC(des, des.getBound());
	}

	ks::RectTransDescription KSImage_API convertToNDC(const ks::RectTransDescription & des, const ks::Rect & boundingBox) noexcept
	{
		const glm::mat4 projection = glm::ortho<float>(boundingBox.x, boundingBox.getMaxX(), boundingBox.y, boundingBox.getMaxY(), 0.1, 1000.0);

		const std::function<glm::vec2(const glm::vec2&, const glm::mat4&)> applyProjection = [](const glm::vec2& point, const glm::mat4& projection)
		{
			glm::vec4 p = projection * glm::vec4(point, 1.0, 1.0);
			return glm::vec2(p.x, p.y);
		};

		const ks::Quadrilateral quad = des.getQuad();
		const glm::vec2 topLeft = applyProjection(quad.topLeft, projection);
		const glm::vec2 topRight = applyProjection(quad.topRight, projection);
		const glm::vec2 bottomLeft = applyProjection(quad.bottomLeft, projection);
		const glm::vec2 bottomRight = applyProjection(quad.bottomRight, projection);

		return ks::RectTransDescription(ks::Quadrilateral(topLeft, topRight, bottomLeft, bottomRight));
	}
}