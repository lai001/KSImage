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
kernel float4 fragmentShader(Texture2D inputImage, PS_INPUT ps_input)
{
	float4 color = inputImage.Sample(ks_sampler0(), ps_input.texcoord);
	return color;
}
)");
		std::shared_ptr<TransformFilter> ptr = std::make_shared<TransformFilter>();
		ptr->kernel = kernel;
		return ptr;
	}

	KernelRenderInstruction TransformFilter::onPrepare(const ks::Rect& renderRect)
	{
		const std::vector<unsigned int > indexBuffer = std::vector<unsigned int >{
			0, 1, 2, 2, 1, 3
		};

		ks::RectTransDescription des = ks::RectTransDescription(inputImage->getRect()).applyTransform(transform);
		des = ks::convertToNDC(des, renderRect);
		const ks::Quadrilateral quad = des.getQuad();
		const std::vector<ks::ImageVertex> vertexs = {
			ks::ImageVertex(glm::vec3(quad.topLeft, 1.0), glm::vec2(0.0, 0.0)), // Top-Left
			ks::ImageVertex(glm::vec3(quad.topRight, 1.0), glm::vec2(1.0, 0.0)), // Top-Right
			ks::ImageVertex(glm::vec3(quad.bottomLeft, 1.0), glm::vec2(0.0, 1.0)), // Bottom-Left
			ks::ImageVertex(glm::vec3(quad.bottomRight, 1.0), glm::vec2(1.0, 1.0)), // Bottom-Right
		};

		kernel->setVertexObject(vertexs.data(), vertexs.size(), sizeof(ks::ImageVertex),
			indexBuffer.data(), indexBuffer.size(), ks::IIndexBuffer::IndexDataType::uint32);

		assert(getInputImages().size() == 1);
		KernelRenderInstruction instruction;
		instruction.sampleSapceRectsNorm.push_back(glm::vec4(0.0, 0.0, 1.0, 1.0));
		return instruction;
	}
}