#include "PassthroughFilter.hpp"
#include <functional>
#include "ImageVertex.hpp"
#include "RectTransDescription.hpp"
#include "Util.hpp"

namespace ks
{
	Image* PassthroughFilter::outputImage(const ks::Rect* rect)
	{
		setValues({ inputImage });
		return Filter::outputImage(rect);
	}

	PassthroughFilter* PassthroughFilter::create() noexcept
	{
		static std::shared_ptr<Kernel> kernel = Kernel::create(R"(
kernel float4 fragmentShader(Texture2D inputImage, PS_INPUT ps_input)
{
	float4 color = inputImage.Sample(ks_sampler0(), ks_sampler0Transform(ps_input));
	return color;
}
)");
		PassthroughFilter* ptr = new PassthroughFilter();
		ptr->kernel = kernel;
		return ptr;
	}

	KernelRenderInstruction PassthroughFilter::onPrepare(const ks::Rect& renderRect)
	{
		const std::vector<unsigned int > indexBuffer = std::vector<unsigned int >{
			0, 1, 2, 2, 1, 3
		};

		ks::RectTransDescription des = ks::RectTransDescription(innerRect);
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
		instruction.workingSpacePixelSize = glm::vec2(renderRect.width, renderRect.height);
		instruction.sampleSapceRectsNorm.push_back(glm::vec4(0.0, 0.0, 1.0, 1.0));
		return instruction;
	}
}