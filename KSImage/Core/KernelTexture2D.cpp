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

	KernelTexture2D* KernelTexture2D::createPlaceHolder(/*const Info& info*/) noexcept
	{
		 return new KernelTexture2D();
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
			textureHandle = bgfx::createTexture2D(width, height, false, 1, chooseFormat(image->getImageFormat()), BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE, memoryRef);
			assert(bgfx::isValid(textureHandle));
		}
		else
		{
			assert(false);
		}
	}

	void KernelTexture2D::copyPixelBuffer(const ks::PixelBuffer& pixelBuffer) noexcept
	{
		if (bgfx::isValid(textureHandle))
		{
			bgfx::destroy(textureHandle);
			textureHandle = BGFX_INVALID_HANDLE;
		}

		const void* data = pixelBuffer.getImmutableData()[0];

		if (data)
		{
			const int width = pixelBuffer.getWidth();
			const int height = pixelBuffer.getHeight();
			const int channels = pixelBuffer.getChannels();
			//const bgfx::Memory* memoryRef = bgfx::makeRef(data, width * height * channels, nullptr, nullptr);
			const bgfx::Memory* memoryRef = bgfx::copy(data, width * height * channels);
			textureHandle = bgfx::createTexture2D(width, height, false, 1, chooseFormat(pixelBuffer.getType()), BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE, memoryRef);
			assert(bgfx::isValid(textureHandle));
		}
		else
		{
			assert(false);
		}
	}

	void KernelTexture2D::copyTextureHandle(bgfx::TextureHandle handle) noexcept
	{
		if (bgfx::isValid(textureHandle))
		{
			bgfx::destroy(textureHandle);
			textureHandle = BGFX_INVALID_HANDLE;
		}
		textureHandle = handle;
	}

	void KernelTexture2D::bind(const uint8_t stage, const bgfx::UniformHandle& uniformHandle) const noexcept
	{
		assert(bgfx::isValid(textureHandle));
		assert(bgfx::isValid(uniformHandle));
		bgfx::setTexture(stage, uniformHandle, textureHandle, BGFX_SAMPLER_UVW_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
	}

	bgfx::TextureFormat::Enum KernelTexture2D::chooseFormat(const int channelCount) noexcept
	{
		assert(channelCount == 3 || channelCount == 4);
		std::unordered_map<int, bgfx::TextureFormat::Enum> dic;
		dic[3] = bgfx::TextureFormat::RGB8;
		dic[4] = bgfx::TextureFormat::RGBA8;
		return dic[channelCount];
	}

	bgfx::TextureFormat::Enum KernelTexture2D::chooseFormat(const ks::PixelBuffer::FormatType formatType) noexcept
	{
		std::unordered_map<ks::PixelBuffer::FormatType, bgfx::TextureFormat::Enum> dic;
		dic[ks::PixelBuffer::FormatType::rgb8] = bgfx::TextureFormat::RGB8;
		dic[ks::PixelBuffer::FormatType::rgba8] = bgfx::TextureFormat::RGBA8;
		dic[ks::PixelBuffer::FormatType::gray8] = bgfx::TextureFormat::R8;
		dic[ks::PixelBuffer::FormatType::bgra8] = bgfx::TextureFormat::BGRA8;
		assert(dic.end() != dic.find(formatType));
		return dic[formatType];
	}

	bgfx::TextureFormat::Enum KernelTexture2D::chooseFormat(const ks::Image::FormatType formatType) noexcept
	{
		std::unordered_map<ks::Image::FormatType, bgfx::TextureFormat::Enum> dic;
		dic[ks::Image::FormatType::rgb8] = bgfx::TextureFormat::RGB8;
		dic[ks::Image::FormatType::rgba8] = bgfx::TextureFormat::RGBA8;
		assert(dic.end() != dic.find(formatType));
		return dic[formatType];
	}

	bgfx::TextureHandle KernelTexture2D::getTextureHandle() const noexcept
	{
		return textureHandle;
	}
}