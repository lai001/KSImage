#ifndef FILTER_HPP
#define FILTER_HPP

#include <Foundation/Foundation.hpp>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>
#include "Image.hpp"
#include "Kernel.hpp"
#include "Util.hpp"

namespace ks
{

	struct KernelRenderInstruction
	{
		std::vector<ks::Rect> sampleSapceRects;
		ks::Rect renderRect;

		void accept(const std::vector<std::shared_ptr<ks::Image>>& images) noexcept
		{
			renderRect = ks::getUnionRect(images);
			sampleSapceRects.clear();
			for (const auto& image : images)
			{
				Rect sampleSapceRect;
				const Rect rect = image->getRect();
				sampleSapceRect.width = (rect.width / renderRect.width);
				sampleSapceRect.height = (rect.height / renderRect.height);
				sampleSapceRect.x = (rect.x - renderRect.x) / renderRect.width;
				sampleSapceRect.y = 1.0 - (rect.getMaxY() / renderRect.height); // To Bottom-Left
				sampleSapceRects.push_back(sampleSapceRect);
			}
		}

		glm::vec2 getWorkingSpacePixelSize() const noexcept
		{
			return glm::vec2(renderRect.width, renderRect.height);
		}

		std::vector<glm::vec4> getSampleSapceRects() const noexcept
		{
			std::vector<glm::vec4> vec;
			for (auto item : sampleSapceRects)
			{
				vec.push_back(glm::vec4(item.x, item.y, item.getMaxX(), item.getMaxY()));
			}
			return vec;
		}
	};

	class Filter: public boost::noncopyable
	{
	protected:
		std::shared_ptr<ks::Kernel> kernel = std::shared_ptr<ks::Kernel>(nullptr);
		std::shared_ptr<ks::Image> currentOutputImage = std::shared_ptr<ks::Image>(nullptr);
		std::vector<ks::KernelUniform::Value> uniformValues;

	public:
		virtual KernelRenderInstruction onPrepare();
		virtual std::shared_ptr<ks::Image> outputImage();
		void setValues(const std::vector<ks::KernelUniform::Value>& uniformValues) noexcept;
		std::shared_ptr<ks::Kernel> getKernel() const noexcept;
		std::vector<ks::KernelUniform::Value> getUniformValues() const noexcept;
		std::vector<std::shared_ptr<ks::Image>> getInputImages() const noexcept;
	};
}

#endif // !FILTER_HPP