#include "FilterContext.hpp"
#include <spdlog/spdlog.h>
#include "FragmentAnalysis.hpp"
#include "Util.hpp"
#include "Filter.hpp"

namespace ks
{
	struct IFrameBufferDeletor {
		void operator()(IFrameBuffer* p) const
		{
			Kernel::renderEngine->erase(p);
		}
	};

	struct ITexture2DDeletor
	{
		void operator()(ITexture2D* p) const
		{
			Kernel::renderEngine->erase(p);
		}
	};

	FilterContext* FilterContext::create() noexcept
	{
		return new FilterContext();
	}

#ifdef _WIN32
	void FilterContext::Init(const ks::D3D11RenderEngineCreateInfo & info) noexcept
	{
		assert(ks::Kernel::renderEngine == nullptr);
		ks::Kernel::renderEngine = ks::RenderEngine::create(info);
	}
#endif

	void FilterContext::Init(const ks::GLRenderEngineCreateInfo & info) noexcept
	{
		assert(ks::Kernel::renderEngine == nullptr);
		ks::Kernel::renderEngine = ks::RenderEngine::create(info);
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

	std::shared_ptr<ks::IFrameBuffer> render(ks::Filter* filter, 
		std::vector<ks::ITexture2D*> textureHandles,
		const ks::Rect& renderRect)
	{
		assert(ks::Kernel::renderEngine);
		assert(filter);
		std::shared_ptr<ks::Kernel> kernel = filter->getKernel();
		const KernelRenderInstruction renderInstruction = filter->onPrepare(renderRect);
		assert(kernel);

		ks::IRenderEngine& renderEngine = *ks::Kernel::renderEngine;

		std::shared_ptr<ks::IFrameBuffer> frameBuffer = std::shared_ptr<ks::IFrameBuffer>(renderEngine.createFrameBuffer(renderRect.width, renderRect.height), IFrameBufferDeletor());

		kernel->setUniform(filter->getUniformValues(), [textureHandles, renderInstruction](unsigned int index, glm::vec4* sampleSapceRectsNorm)
		{
			*sampleSapceRectsNorm = renderInstruction.sampleSapceRectsNorm[index];
			return textureHandles[index];
		});

		kernel->commit(frameBuffer);

		return frameBuffer;
	}

	std::shared_ptr<ks::IFrameBuffer> _walk(FilterChainNode rootNode,
		const ks::Rect& renderRect)
	{
		assert(ks::Kernel::renderEngine);
		ks::IRenderEngine& renderEngine = *ks::Kernel::renderEngine;

		std::vector<ks::ITexture2D*> textureHandles;
		std::vector<std::shared_ptr<ks::IFrameBuffer>> childBuffers;

		std::function<void()> cleanClosure = std::function<void()>(); 

		for (FilterChainNode childNode : rootNode.childNodes)
		{
			std::shared_ptr<ks::IFrameBuffer> childBuffer = _walk(childNode, renderRect);
			ks::ITexture2D* childHandle = childBuffer->getColorTexture();
			textureHandles.push_back(childHandle);
			childBuffers.push_back(childBuffer);
		}

		if (textureHandles.empty())
		{
			for (std::shared_ptr<ks::Image> image : rootNode.filter->getInputImages())
			{
				const unsigned char *data = image->getData();
				assert(data);
				const int width = image->getSourceWidth();
				const int height = image->getSourceHeight();
				
				ks::ITexture2D* textureHandle = renderEngine.createTexture2D(width,
					height,
					ks::TextureFormat::R8G8B8A8_UNORM, 
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

		if (rootNode.isRoot)
		{
			std::shared_ptr<ks::IFrameBuffer> frameBuffer = render(rootNode.filter, textureHandles, renderRect);
			if (cleanClosure)
			{
				cleanClosure();
			}
			return frameBuffer;
		}
		else
		{
			ks::Rect renderRect = rootNode.filter->getCurrentOutputImage()->getRect();
			std::shared_ptr<ks::IFrameBuffer> frameBuffer = render(rootNode.filter, textureHandles, renderRect);
			if (cleanClosure)
			{
				cleanClosure();
			}
			return frameBuffer;
		}
	}

	ks::PixelBuffer* FilterContext::render(const ks::Image & image, const ks::Rect& bound) const noexcept
	{
		assert(ks::Kernel::renderEngine);
		ks::IRenderEngine& renderEngine = *ks::Kernel::renderEngine;

		FilterChainNode rootNode = makeChain(image);
		rootNode.isRoot = true;
		std::shared_ptr<ks::IFrameBuffer> frameBuffer = _walk(rootNode, bound);
		ks::PixelBuffer* bufferPtr = new ks::PixelBuffer(bound.width,
			bound.height,
			ks::PixelBuffer::FormatType::rgba8);
		renderEngine.readTexture(frameBuffer.get(), *bufferPtr);
		return bufferPtr;
	}
}