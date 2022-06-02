#include "FilterContext.hpp"
#include <spdlog/spdlog.h>
#include "FragmentAnalysis.hpp"
#include "Util.hpp"
#include "Filter.hpp"
#include "InternalFilters/PassthroughFilter.hpp"

namespace ks
{
	FilterContext* FilterContext::create() noexcept
	{
		return new FilterContext();
	}

#ifdef _WIN32
	void FilterContext::Init(const D3D11RenderEngineCreateInfo & info) noexcept
	{
		assert(Kernel::renderEngine == nullptr);
		Kernel::renderEngine = RenderEngine::create(info);
	}
#endif

	void FilterContext::Init(const GLRenderEngineCreateInfo & info) noexcept
	{
		assert(Kernel::renderEngine == nullptr);
		Kernel::renderEngine = RenderEngine::create(info);
	}
}

namespace ks
{
	struct FilterChainNode
	{
		std::vector<FilterChainNode> childNodes;
		Filter* filter = nullptr;
	};

	FilterChainNode makeChain(const Image & image)
	{
		Filter* filter = image.getSourceFilter();
		if (filter)
		{
			FilterChainNode node;
			node.filter = filter;
			for (const Image* inputImage : filter->getInputImages())
			{
				FilterChainNode childNode;
				childNode = makeChain(*inputImage);
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

	IFrameBuffer* render(Filter* filter,
		std::vector<ITexture2D*> textureHandles,
		const Rect& renderRect)
	{
		assert(Kernel::renderEngine);
		assert(filter);
		std::shared_ptr<Kernel> kernel = filter->getKernel();
		const KernelRenderInstruction renderInstruction = filter->onPrepare(renderRect);
		assert(kernel);
		IRenderEngine& renderEngine = *Kernel::renderEngine;
		IFrameBuffer* frameBuffer = renderEngine.createFrameBuffer(renderRect.width, renderRect.height);
		kernel->setUniform(filter->getUniformValues(), textureHandles, renderInstruction);
		kernel->commit(*frameBuffer);
		return frameBuffer;
	}

	IFrameBuffer* _walk(FilterChainNode rootNode)
	{
		assert(Kernel::renderEngine);
		IRenderEngine& renderEngine = *Kernel::renderEngine;

		std::vector<ITexture2D*> textureHandles;
		std::vector<IFrameBuffer*> childBuffers;

		std::function<void()> cleanClosure = std::function<void()>(); 

		for (FilterChainNode childNode : rootNode.childNodes)
		{
			IFrameBuffer* childBuffer = _walk(childNode);
			ITexture2D* childHandle = childBuffer->getColorTexture();
			textureHandles.push_back(childHandle);
			childBuffers.push_back(childBuffer);
		}

		if (textureHandles.empty())
		{
			for (const Image* image : rootNode.filter->getInputImages())
			{
				const unsigned char *data = image->getData();
				assert(data);
				const int width = image->getSourceWidth();
				const int height = image->getSourceHeight();
				
				ITexture2D* textureHandle = renderEngine.createTexture2D(width,
					height,
					TextureFormat::R8G8B8A8_UNORM, 
					data);
				textureHandles.push_back(textureHandle);
			}

			cleanClosure = [textureHandles]()
			{
				for (size_t i = 0; i < textureHandles.size(); i++)
				{
					Kernel::renderEngine->erase(textureHandles[i]);
				}
			};
		}

		defer
		{
			if (cleanClosure)
			{
				cleanClosure();
			}
			for (size_t i = 0; i < childBuffers.size(); i++)
			{
				assert(Kernel::renderEngine);
				IRenderEngine& renderEngine = *Kernel::renderEngine;
				if (childBuffers[i])
				{
					renderEngine.erase(childBuffers[i]);
				}
			}
		};

		Rect renderRect = rootNode.filter->getCurrentOutputImage()->getRect();
		return render(rootNode.filter, textureHandles, renderRect);
	}

	PixelBuffer * createPixelBuffer(const IFrameBuffer* frameBuffer)
	{
		assert(Kernel::renderEngine);
		IRenderEngine& renderEngine = *Kernel::renderEngine;
		PixelBuffer *pixelBuffer = new PixelBuffer(frameBuffer->getWidth(),
			frameBuffer->getHeight(),
			PixelBuffer::FormatType::rgba8);
		renderEngine.readTexture(frameBuffer, *pixelBuffer);
		return pixelBuffer;
	}

	PixelBuffer* passthrough(IFrameBuffer* frameBuffer, const Rect& innerRect, const Rect& bound)
	{
		// TODO:
		assert(Kernel::renderEngine);
		IRenderEngine& renderEngine = *Kernel::renderEngine;

		std::unique_ptr<Image> inputImage = std::unique_ptr<Image>(Image::createRetain(createPixelBuffer(frameBuffer)));
		std::unique_ptr<PassthroughFilter> passthroughFilter = std::unique_ptr<PassthroughFilter>(PassthroughFilter::create());
		passthroughFilter->inputImage = inputImage.get();
		passthroughFilter->innerRect = innerRect;
		Image* outputImage = passthroughFilter->outputImage(&bound);

		FilterChainNode rootNode = makeChain(*outputImage);
		IFrameBuffer* passthroughFrameBuffer = _walk(rootNode);
		defer { renderEngine.erase(passthroughFrameBuffer); };
		return createPixelBuffer(passthroughFrameBuffer);
	}

	PixelBuffer* FilterContext::render(const Image & image, const Rect& bound) const noexcept
	{
		assert(Kernel::renderEngine);
		IRenderEngine& renderEngine = *Kernel::renderEngine;

		FilterChainNode rootNode = makeChain(image);
		IFrameBuffer* frameBuffer = _walk(rootNode);
		defer { renderEngine.erase(frameBuffer); };
		PixelBuffer * pixelBuffer = passthrough(frameBuffer, rootNode.filter->getCurrentOutputImage()->getRect(), bound);
		return pixelBuffer;
	}
}