#include "Filter.hpp"
#include "ImageVertex.hpp"

namespace ks
{
	std::shared_ptr<ks::Image> Filter::outputImage(const ks::Rect* rect)
	{
		if (rect)
		{
			currentOutputImage = ks::Image::create(*rect);
		}
		else
		{
			std::vector<std::shared_ptr<ks::Image>> images;
			for (size_t i = 0; i < uniformValues.size(); i++)
			{
				ks::KernelUniform::Value uniformValue = uniformValues[i];
				if (uniformValue.type == ks::KernelUniform::ValueType::texture2d)
				{
					images.push_back(uniformValue.texture2d);
				}
			}
			currentOutputImage = ks::Image::create(ks::getUnionRect(images));
		}
		currentOutputImage->sourceFilter = this;
		return currentOutputImage;
	}

	KernelRenderInstruction Filter::onPrepare(const ks::Rect& renderRect)
	{
		kernel->setIndices(std::vector<unsigned int >{
			0, 1, 2, 1, 2, 3
		});
		
		std::vector<ks::ImageVertex> vertexs = {
			ks::ImageVertex(glm::vec3(-1.0,  1.0, 1.0), glm::vec2(0.0, 0.0)),
			ks::ImageVertex(glm::vec3( 1.0,  1.0, 1.0), glm::vec2(1.0, 0.0)),
			ks::ImageVertex(glm::vec3(-1.0, -1.0, 1.0), glm::vec2(0.0, 1.0)),
			ks::ImageVertex(glm::vec3( 1.0, -1.0, 1.0), glm::vec2(1.0, 1.0)),
		};
		kernel->setVertex(vertexs.data(), vertexs.size() * sizeof(ks::ImageVertex), ImageVertex::vertexLayout);

		KernelRenderInstruction instruction;
		const std::vector<std::shared_ptr<ks::Image>>& inputImages = getInputImages();
		for (const std::shared_ptr<ks::Image>& inputImage : inputImages)
		{
			Rect sampleSapceRectNorm;
			const Rect rect = inputImage->getRect();
			sampleSapceRectNorm.width = (rect.width / renderRect.width);
			sampleSapceRectNorm.height = (rect.height / renderRect.height);
			sampleSapceRectNorm.x = (rect.x - renderRect.x) / renderRect.width;
			sampleSapceRectNorm.y = (rect.y - renderRect.y) / renderRect.height;

			glm::vec4 sampleSapceRectNormTopLeft;
			sampleSapceRectNormTopLeft.x = sampleSapceRectNorm.x;
			sampleSapceRectNormTopLeft.z = sampleSapceRectNorm.getMaxX();

			sampleSapceRectNormTopLeft.y = 1.0 - (sampleSapceRectNorm.y + sampleSapceRectNorm.height);
			sampleSapceRectNormTopLeft.w = sampleSapceRectNormTopLeft.y + sampleSapceRectNorm.height;

			instruction.sampleSapceRectsNorm.push_back(sampleSapceRectNormTopLeft);
		}
		return instruction;
	}

	void Filter::setValues(const std::vector<ks::KernelUniform::Value>& uniformValues) noexcept
	{
		this->uniformValues.clear();
		this->uniformValues = uniformValues;
	}

	std::shared_ptr<ks::Kernel> Filter::getKernel() const noexcept
	{
		return kernel;
	}

	std::vector<ks::KernelUniform::Value> Filter::getUniformValues() const noexcept
	{
		return uniformValues;
	}

	std::vector<std::shared_ptr<ks::Image>> Filter::getInputImages() const noexcept
	{
		std::vector<std::shared_ptr<ks::Image>> images;
		for (auto& uniformValue : uniformValues)
		{
			if (uniformValue.type == ks::KernelUniform::ValueType::texture2d)
			{
				images.push_back(uniformValue.texture2d);
			}
		}
		return images;
	}

	const std::shared_ptr<ks::Image> Filter::getCurrentOutputImage() const noexcept
	{
		return currentOutputImage;
	}
}