#include "TransformFilter.hpp"
#include <functional>
#include "ImageVertex.hpp"
#include "RectTransDescription.hpp"
#include "Util.hpp"

namespace ks
{
	std::shared_ptr<Image> TransformFilter::outputImage(const ks::Rect* rect)
	{
		setValues({ inputImage });
		ks::RectTransDescription des = ks::RectTransDescription(inputImage->getRect()).applyTransform(transform);
		return Filter::outputImage(&des.getBound());
	}

	std::shared_ptr<TransformFilter> TransformFilter::create() noexcept
	{
		static std::shared_ptr<Kernel> kernel = Kernel::create(R"(
kernel vec4 fragmentShader(BgfxSampler2D inputImage)
{
	return texture2D(inputImage, ks_destCoord());
}
)");
		std::shared_ptr<TransformFilter> ptr = std::make_shared<TransformFilter>();
		ptr->kernel = kernel;
		return ptr;
	}

	KernelRenderInstruction TransformFilter::onPrepare(const ks::Rect& renderRect)
	{
		KernelRenderInstruction instruction;
		//instruction.accept(getInputImages(), renderRect);
		instruction.accept(std::vector<std::shared_ptr<ks::Image>>({ currentOutputImage }), renderRect);
		
		kernel->setIndices(std::vector<unsigned int>{
			0, 1, 2, 1, 2, 3
		});
		ks::RectTransDescription des = ks::RectTransDescription(inputImage->getRect()).applyTransform(transform);
		des = ks::convertToNDC(des, renderRect);

		const std::vector<ks::ImageVertex> vertexs = {
			ks::ImageVertex(glm::vec3(des.getQuad().topLeft, 1.0), glm::vec2(0.0, 0.0)), // Top-Left
			ks::ImageVertex(glm::vec3(des.getQuad().topRight, 1.0), glm::vec2(1.0, 0.0)), // Top-Right
			ks::ImageVertex(glm::vec3(des.getQuad().bottomLeft, 1.0), glm::vec2(0.0, 1.0)), // Bottom-Left
			ks::ImageVertex(glm::vec3(des.getQuad().bottomRight, 1.0), glm::vec2(1.0, 1.0)), // Bottom-Right
		};
		kernel->setVertex(vertexs.data(), vertexs.size() * sizeof(ks::ImageVertex), ImageVertex::vertexLayout);
		return instruction;
	}
}