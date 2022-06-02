#include "Filter.hpp"
#include "ImageVertex.hpp"
#include "Util.hpp"

namespace ks
{
	Image* Filter::outputImage(const Rect* rect)
	{
		if (currentOutputImage)
		{
			delete currentOutputImage;
		}
		if (rect)
		{
			currentOutputImage = Image::create(*rect);
		}
		else
		{
			std::vector<Image*> images;
			for (size_t i = 0; i < uniformValues.size(); i++)
			{
				KernelUniform::Value uniformValue = uniformValues[i];
				if (uniformValue.type == KernelUniform::ValueType::texture2d)
				{
					images.push_back(uniformValue.texture2d);
				}
			}
			currentOutputImage = Image::create(getUnionRect(images));
		}
		currentOutputImage->sourceFilter = this;
		return currentOutputImage;
	}

	Filter::~Filter()
	{
		if (currentOutputImage)
		{
			delete currentOutputImage;
		}
	}

	KernelRenderInstruction Filter::onPrepare(const Rect& renderRect)
	{
		const std::vector<unsigned int > indexBuffer = std::vector<unsigned int >{
			0, 1, 2, 2, 1, 3
		};

		const std::vector<ImageVertex> vertexs = {
			ImageVertex(glm::vec3(-1.0,  1.0, 1.0), glm::vec2(0.0, 0.0)),
			ImageVertex(glm::vec3( 1.0,  1.0, 1.0), glm::vec2(1.0, 0.0)),
			ImageVertex(glm::vec3(-1.0, -1.0, 1.0), glm::vec2(0.0, 1.0)),
			ImageVertex(glm::vec3( 1.0, -1.0, 1.0), glm::vec2(1.0, 1.0)),
		};

		kernel->setVertexObject(vertexs.data(), vertexs.size(), sizeof(ImageVertex),
			indexBuffer.data(), indexBuffer.size(), IIndexBuffer::IndexDataType::uint32);

		KernelRenderInstruction instruction;
		instruction.workingSpacePixelSize = glm::vec2(renderRect.width, renderRect.height);
		const std::vector<Image*>& inputImages = getInputImages();
		for (const Image* inputImage : inputImages)
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

	void Filter::setValues(const std::vector<KernelUniform::Value>& uniformValues) noexcept
	{
		this->uniformValues.clear();
		this->uniformValues = uniformValues;
	}

	std::shared_ptr<Kernel> Filter::getKernel() const noexcept
	{
		return kernel;
	}

	std::vector<KernelUniform::Value> Filter::getUniformValues() const noexcept
	{
		return uniformValues;
	}

	std::vector<Image*> Filter::getInputImages() const noexcept
	{
		std::vector<Image*> images;
		for (auto& uniformValue : uniformValues)
		{
			if (uniformValue.type == KernelUniform::ValueType::texture2d)
			{
				images.push_back(uniformValue.texture2d);
			}
		}
		return images;
	}

	const Image* Filter::getCurrentOutputImage() const noexcept
	{
		return currentOutputImage;
	}
}