#include "KernelTexture2D.hpp"

namespace ks
{
	KernelTexture2D::~KernelTexture2D()
	{
		if (bgfx::isValid(textureHandle))
		{
			bgfx::destroy(textureHandle);
			textureHandle = BGFX_INVALID_HANDLE;
		}
	}

	std::shared_ptr<KernelTexture2D> KernelTexture2D::createPlaceHolder(/*const Info& info*/) noexcept
	{
		 std::shared_ptr<KernelTexture2D> ptr = std::make_shared<KernelTexture2D>();
		 return ptr;
	}

	void KernelTexture2D::copyImage(const std::shared_ptr<ks::Image> image) noexcept
	{
		if (bgfx::isValid(textureHandle))
		{
			bgfx::destroy(textureHandle);
			textureHandle = BGFX_INVALID_HANDLE;
		}

		const void* data = image->getData();

		if (data)
		{
			const int width = image->getSourceWidth();
			const int height = image->getSourceHeight();
			const int channels = image->getSourceChannels();
			//const bgfx::Memory* memoryRef = bgfx::makeRef(data, width * height * channels, nullptr, nullptr);
			const bgfx::Memory* memoryRef = bgfx::copy(data, width * height * channels);
			textureHandle = bgfx::createTexture2D(width, height, false, 1, chooseFormat(channels), BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE, memoryRef);
			assert(bgfx::isValid(textureHandle));
		}
		else
		{
			assert(false);
		}
	}

	void KernelTexture2D::copyPixelBuffer(const std::shared_ptr<ks::PixelBuffer> pixelBuffer) noexcept
	{
		if (bgfx::isValid(textureHandle))
		{
			bgfx::destroy(textureHandle);
			textureHandle = BGFX_INVALID_HANDLE;
		}

		const void* data = pixelBuffer->getImmutableData();

		if (data)
		{
			const int width = pixelBuffer->getWidth();
			const int height = pixelBuffer->getHeight();
			const int channels = pixelBuffer->getChannels();
			//const bgfx::Memory* memoryRef = bgfx::makeRef(data, width * height * channels, nullptr, nullptr);
			const bgfx::Memory* memoryRef = bgfx::copy(data, width * height * channels);
			textureHandle = bgfx::createTexture2D(width, height, false, 1, chooseFormat(channels), BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE, memoryRef);
			assert(bgfx::isValid(textureHandle));
		}
		else
		{
			assert(false);
		}
	}

	void KernelTexture2D::bind(const uint8_t stage, const bgfx::UniformHandle& uniformHandle) const noexcept
	{
		assert(bgfx::isValid(textureHandle));
		assert(bgfx::isValid(uniformHandle));
		bgfx::setTexture(stage, uniformHandle, textureHandle, BGFX_SAMPLER_UVW_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
	}

	bgfx::TextureFormat::Enum KernelTexture2D::chooseFormat(const int channelCount) const noexcept
	{
		assert(channelCount == 3 || channelCount == 4);
		std::unordered_map<int, bgfx::TextureFormat::Enum> dic;
		dic[3] = bgfx::TextureFormat::RGB8;
		dic[4] = bgfx::TextureFormat::RGBA8;
		return dic[channelCount];
	}
}