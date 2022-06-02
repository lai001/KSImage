#include "Util.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace ks
{
	Rect getUnionRect(const std::vector<Image*>& images) noexcept
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

	RectTransDescription KSImage_API convertToNDC(const RectTransDescription& des) noexcept
	{
		return convertToNDC(des, des.getBound());
	}

	RectTransDescription KSImage_API convertToNDC(const RectTransDescription & des, const Rect & boundingBox) noexcept
	{
		const glm::mat4 projection = glm::ortho<float>(boundingBox.x, boundingBox.getMaxX(), boundingBox.y, boundingBox.getMaxY(), 0.1, 1000.0);

		const std::function<glm::vec2(const glm::vec2&, const glm::mat4&)> applyProjection = [](const glm::vec2& point, const glm::mat4& projection)
		{
			glm::vec4 p = projection * glm::vec4(point, 1.0, 1.0);
			return glm::vec2(p.x, p.y);
		};

		const Quadrilateral quad = des.getQuad();
		const glm::vec2 topLeft = applyProjection(quad.topLeft, projection);
		const glm::vec2 topRight = applyProjection(quad.topRight, projection);
		const glm::vec2 bottomLeft = applyProjection(quad.bottomLeft, projection);
		const glm::vec2 bottomRight = applyProjection(quad.bottomRight, projection);

		return RectTransDescription(Quadrilateral(topLeft, topRight, bottomLeft, bottomRight));
	}

	PixelBuffer * createOriginLUT(const unsigned int cubeLength) noexcept
	{
		assert(ks::isPowerOfTwo(cubeLength));
		const unsigned int smallCubeLength = sqrt(cubeLength);
		const unsigned int imageLength = smallCubeLength * cubeLength;

		PixelBuffer* pixelBuffer = new PixelBuffer(imageLength, imageLength, PixelBuffer::FormatType::rgba8);

		for (unsigned z = 0; z < cubeLength; z++)
		{
			for (unsigned x = 0; x < cubeLength; x++)
			{
				for (unsigned y = 0; y < cubeLength; y++)
				{
					glm::vec3 pixelf(
						(float)x * 255.0f / (float)(cubeLength - 1) + 0.5,
						(float)y * 255.0f / (float)(cubeLength - 1) + 0.5,
						(float)z * 255.0f / (float)(cubeLength - 1) + 0.5
					);
					glm::u8vec4 pixelu8(pixelf.x, pixelf.y, pixelf.z, 255);
					const unsigned int imgY = z / smallCubeLength * cubeLength + y;
					const unsigned int imgX = z % smallCubeLength * cubeLength + x;
					const unsigned int index = imgY * imageLength + imgX;
					memcpy(pixelBuffer->getMutableData()[0] + index * sizeof(glm::u8vec4), glm::value_ptr(pixelu8), sizeof(glm::u8vec4));
				}
			}
		}

		return pixelBuffer;
	}
}