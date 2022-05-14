#include "FilterContext.hpp"
#include <spdlog/spdlog.h>
#include "FragmentAnalysis.hpp"
#include "FrameBuffer.hpp"
#include "Util.hpp"
#include "Filter.hpp"

namespace ks
{
	unsigned int FilterContext::frameNumber;

	FilterContext* FilterContext::create() noexcept
	{
		return new FilterContext();
	}

	void FilterContext::Init(const bgfx::Init& init, void* nwh) noexcept
	{
		bgfx::PlatformData pd;
		memset(&pd, 0, sizeof(pd));
		pd.nwh = nwh;
		bgfx::setPlatformData(pd);

		bool isbgfxInitSuccess = bgfx::init(init);
		assert(isbgfxInitSuccess);
		bgfx::setDebug(BGFX_DEBUG_NONE);
	}

	void FilterContext::Init(void* nwh, const unsigned int width, const unsigned int height) noexcept
	{
		bgfx::Init init;
		init.type = bgfx::RendererType::Enum::Count;
		init.vendorId = BGFX_PCI_ID_NONE;
		init.resolution.width = width;
		init.resolution.height = height;
		init.resolution.reset = BGFX_RESET_VSYNC;
		FilterContext::Init(init, nwh);
	}

	void FilterContext::shutdown() noexcept
	{
		// FIXME:
		// bgfx::shutdown();
	}
}

namespace ks
{
	struct FilterChainNode
	{
		std::vector<FilterChainNode> childNodes;
		ks::Filter* filter = nullptr;
		bool isRoot = false;
	};

	FilterChainNode makeChain(const ks::Image & image)
	{
		ks::Filter* filter = image.getSourceFilter();
		if (filter)
		{
			FilterChainNode node;
			node.filter = filter;
			for (const auto inputImage : filter->getInputImages())
			{
				FilterChainNode childNode;
				childNode = makeChain(*inputImage.get());
				if (childNode.filter)
				{
					node.childNodes.push_back(childNode);
				}
			}
			return node;
		}
		else
		{
			FilterChainNode node;
			return node;
		}
	}

	std::shared_ptr<ks::FrameBuffer> render(ks::Filter* filter, std::vector<bgfx::TextureHandle> textureHandles, const ks::Rect& renderRect, bgfx::ViewId& ioFramebufferViewId)
	{
		assert(filter);
		const bgfx::ViewId mainViewId = 0;
		bgfx::touch(ioFramebufferViewId);
		std::shared_ptr<ks::FrameBuffer> frameBuffer = std::make_shared<ks::FrameBuffer>(renderRect.width, renderRect.height);
		bgfx::setViewRect(ioFramebufferViewId, 0, 0, frameBuffer->width, frameBuffer->height);
		bgfx::setViewFrameBuffer(ioFramebufferViewId, frameBuffer->m_FrameBufferHandle);
		bgfx::setViewClear(ioFramebufferViewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x00000000, 1.0f, 0);

		{
			const KernelRenderInstruction renderInstruction = filter->onPrepare(renderRect);
			bgfx::setVertexBuffer(0, filter->getKernel()->getVertexBufferHandle());
			bgfx::setIndexBuffer(filter->getKernel()->getIndexBufferHandle());

			int textureHandlesIndex = 0;

			for (int i = 0; i < filter->getKernel()->getUniforms().size(); i++)
			{
				const int inputValuesCount = filter->getUniformValues().size();
				if (i >= inputValuesCount)
				{
					break;
				}
				const ks::KernelUniform* kernelUniform = filter->getKernel()->getUniforms()[i];
				const ks::KernelUniform::Value value = filter->getUniformValues()[i];
				assert(value.type == kernelUniform->getType() && "Input value type must equal to unifom type");
				if (kernelUniform->getType() == ks::KernelUniform::ValueType::texture2d)
				{
					bgfx::TextureHandle handle = textureHandles[textureHandlesIndex];
					textureHandlesIndex += 1;
					if (bgfx::isValid(handle))
					{
						filter->getKernel()->bindTexture2D(kernelUniform->getName(), handle);
					}
				}
				else
				{
					filter->getKernel()->bindUniform(kernelUniform->getName(), value);
				}
			}
			const ks::FragmentAnalysis::ShareInfo shareInfo;
			
			filter->getKernel()->bindUniform(shareInfo.workingSpacePixelSizeUniformName(), KernelUniform::Value(glm::vec2(renderRect.width, renderRect.height)));
			const std::vector<glm::vec4> sampleSapceRectsNorm = renderInstruction.sampleSapceRectsNorm;
			for (int number = 0; number < sampleSapceRectsNorm.size(); number++)
			{
				filter->getKernel()->bindUniform(shareInfo.uniformSamplerSpaceName(number),
					KernelUniform::Value(sampleSapceRectsNorm[number]));
			}

			bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A);
			bgfx::submit(ioFramebufferViewId, filter->getKernel()->getBgfxProgram());
			ioFramebufferViewId += 1;
		}
		assert(bgfx::isValid(frameBuffer->m_rb));
		//bgfx::setViewFrameBuffer(ioFramebufferViewId, BGFX_INVALID_HANDLE);
		return frameBuffer;
	}

	std::shared_ptr<ks::FrameBuffer> _walk(FilterChainNode rootNode, const ks::Rect& renderRect, bgfx::ViewId& ioFramebufferViewId)
	{
		std::vector<bgfx::TextureHandle> textureHandles;

		for (FilterChainNode childNode : rootNode.childNodes)
		{
			std::shared_ptr<ks::FrameBuffer> childBuffer = _walk(childNode, renderRect, ioFramebufferViewId);
			bgfx::TextureHandle childHandle = bgfx::getTexture(childBuffer->m_FrameBufferHandle);
			textureHandles.push_back(childHandle);
		}

		if (textureHandles.empty())
		{
			for (std::shared_ptr<ks::Image> image : rootNode.filter->getInputImages())
			{
				const void* data = image->getData();
				assert(data);
				const int width = image->getSourceWidth();
				const int height = image->getSourceHeight();
				const int channels = image->getSourceChannels();
				const bgfx::Memory* memoryRef = bgfx::copy(data, width * height * channels);
				bgfx::TextureHandle textureHandle = bgfx::createTexture2D(width, height, false, 1, ks::KernelTexture2D::chooseFormat(image->getImageFormat()), BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE, memoryRef);
				assert(bgfx::isValid(textureHandle));
				textureHandles.push_back(textureHandle);
			}
		}

		if (rootNode.isRoot)
		{
			return render(rootNode.filter, textureHandles, renderRect, ioFramebufferViewId);
		}
		else
		{
			ks::Rect renderRect = rootNode.filter->getCurrentOutputImage()->getRect();
			return render(rootNode.filter, textureHandles, renderRect, ioFramebufferViewId);
		}
	}

	ks::PixelBuffer* FilterContext::render(const ks::Image & image, const ks::Rect& bound) const noexcept
	{
		FilterChainNode rootNode = makeChain(image);
		rootNode.isRoot = true;
		bgfx::ViewId ioFramebufferViewId = 1;
		std::shared_ptr<ks::FrameBuffer> frameBuffer = _walk(rootNode, bound, ioFramebufferViewId);
		ks::PixelBuffer* bufferPtr = new ks::PixelBuffer(frameBuffer->width, frameBuffer->height, ks::PixelBuffer::FormatType::rgba8);
		const bgfx::ViewId mainViewId = 0;
		bgfx::blit(mainViewId, frameBuffer->m_rb, 0, 0, bgfx::getTexture(frameBuffer->m_FrameBufferHandle));
		const uint32_t associatedFrameNumber = bgfx::readTexture(frameBuffer->m_rb, bufferPtr->getMutableData()[0], 0);
		frameNumber = bgfx::frame();
		frameNumber = bgfx::frame();
		return bufferPtr;
	}

}