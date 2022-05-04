#include "FilterContext.hpp"
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

	ks::PixelBuffer* FilterContext::render(const ks::Image* image, const ks::Rect& bound) noexcept
	{
		std::unique_ptr<ks::FrameBuffer> frameBuffer;
		const bgfx::ViewId framebufferViewId = 1;
		const bgfx::ViewId mainViewId = 0;
		bgfx::touch(framebufferViewId);

		ks::Filter * filter = image->getSourceFilter();
		assert(filter);

		//const ks::Rect rect = ks::getUnionRect(filter->getInputImages());
		
		frameBuffer = std::make_unique<ks::FrameBuffer>(bound.width, bound.height);
		bgfx::setViewRect(framebufferViewId, 0, 0, frameBuffer->width, frameBuffer->height);
		bgfx::setViewFrameBuffer(framebufferViewId, frameBuffer->m_FrameBufferHandle);
		//bgfx::setViewFrameBuffer(mainViewId, BGFX_INVALID_HANDLE);
		//bgfx::setViewClear(mainViewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x000000ff, 1.0f, 0);
		bgfx::setViewClear(framebufferViewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x00000000, 1.0f, 0);

		{
			const KernelRenderInstruction renderInstruction = filter->onPrepare(bound);
			bgfx::setVertexBuffer(0, filter->getKernel()->getVertexBufferHandle());
			bgfx::setIndexBuffer(filter->getKernel()->getIndexBufferHandle());

			//assert(filter->getKernel()->getUniforms().size() == filter->getUniformValues().size());

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
					std::shared_ptr<ks::Image> image = value.texture2d;
					filter->getKernel()->bindTexture2D(kernelUniform->getName(), image);
				}
				else
				{
					filter->getKernel()->bindUniform(kernelUniform->getName(), value);
				}
			}
			const ks::FragmentAnalysis::ShareInfo shareInfo;
			
			filter->getKernel()->bindUniform(shareInfo.workingSpacePixelSizeUniformName(), KernelUniform::Value(renderInstruction.getWorkingSpacePixelSize()));
			const std::vector<glm::vec4> sampleSapceRects = renderInstruction.getSampleSapceRects();
			for (int number = 0; number < sampleSapceRects.size(); number++)
			{
				filter->getKernel()->bindUniform(shareInfo.uniformSamplerSpaceName(number),
					KernelUniform::Value(sampleSapceRects[number]));
			}

			bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A);
			bgfx::submit(framebufferViewId, filter->getKernel()->getBgfxProgram());
		}

		ks::PixelBuffer* bufferPtr = new ks::PixelBuffer(frameBuffer->width, frameBuffer->height, ks::PixelBuffer::FormatType::rgba8);
		assert(bgfx::isValid(frameBuffer->m_rb));
		bgfx::blit(mainViewId, frameBuffer->m_rb, 0, 0, bgfx::getTexture(frameBuffer->m_FrameBufferHandle));
		const uint32_t associatedFrameNumber = bgfx::readTexture(frameBuffer->m_rb, bufferPtr->getMutableData()[0], 0);
		frameNumber = bgfx::frame();
		frameNumber = bgfx::frame();
		bgfx::setViewFrameBuffer(framebufferViewId, BGFX_INVALID_HANDLE);
		return bufferPtr;
	}
}