#include "Filter.hpp"
#include "ImageVertex.hpp"

namespace ks
{
	std::shared_ptr<ks::Image> Filter::outputImage()
	{
		currentOutputImage = ks::Image::create();
		currentOutputImage->sourceFilter = this;
		return currentOutputImage;
	}

	KernelRenderInstruction Filter::onPrepare()
	{
		KernelRenderInstruction instruction;
		instruction.accept(getInputImages());

		kernel->setIndices(std::vector<unsigned int >{
			0, 1, 2, 1, 2, 3
		});
		std::vector<ks::ImageVertex> vertexs = {
			ks::ImageVertex(glm::vec3(-1.0,  1.0, 0.0), glm::vec2(0.0, 0.0)), // Top-Left
			ks::ImageVertex(glm::vec3(1.0,  1.0, 0.0), glm::vec2(1.0, 0.0)), // Top-Right
			ks::ImageVertex(glm::vec3(-1.0, -1.0, 0.0), glm::vec2(0.0, 1.0)), // Bottom-Left
			ks::ImageVertex(glm::vec3(1.0, -1.0, 0.0), glm::vec2(1.0, 1.0)), // Bottom-Right
		};
		kernel->setVertex(vertexs.data(), vertexs.size() * sizeof(ks::ImageVertex), ImageVertex::vertexLayout);
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
}