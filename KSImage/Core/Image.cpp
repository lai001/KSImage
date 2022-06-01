#include "Image.hpp"
#include <unordered_map>
#include "Util.hpp"
#include "ImageDecoder.hpp"

namespace ks
{
	Image::~Image()
	{
		if (pixelBuffer)
		{
			delete pixelBuffer;
		}
	}

	Image* Image::create()
	{
		Image* image = new Image();
		return image;
	}

	Image* Image::create(const Rect & rect)
	{
		Image* image = new Image();
		image->_rect = rect;
		return image;
	}

	Image* Image::create(const std::string & filePath)
	{
		assert(IImageDecoder::shared);
		PixelBuffer* pixelBuffer = IImageDecoder::shared->create(filePath);
		return createRetain(pixelBuffer);
	}

	Image* Image::createRetain(PixelBuffer* pixelBuffer)
	{
		assert(pixelBuffer);
		Image* image = new Image();
		image->format = pixelFormatToImageFormat(pixelBuffer->getType());
		image->pixelBuffer = pixelBuffer;
		image->_rect = Rect(0, 0, pixelBuffer->getWidth(), pixelBuffer->getHeight());
		return image;
	}

	const unsigned char * Image::getData() const noexcept
	{
		if (PixelBuffer* buffer = pixelBuffer)
		{
			assert(isCompatible(buffer->getType()));
			return buffer->getMutableData()[0];
		}
		else
		{
			return nullptr;
		}
	}

	const int Image::getSourceWidth() const noexcept
	{
		if (PixelBuffer* buffer = pixelBuffer)
		{
			return buffer->getWidth();
		}
		else
		{
			return 0;
		}
	}

	const int Image::getSourceHeight() const noexcept
	{
		if (PixelBuffer* buffer = pixelBuffer)
		{
			return buffer->getHeight();
		}
		else
		{
			return 0;
		}
	}

	const int Image::getSourceChannels() const noexcept
	{
		if (PixelBuffer* buffer = pixelBuffer)
		{
			return buffer->getChannels();
		}
		else
		{
			return 0;
		}
	}

	const Image::FormatType Image::getImageFormat() const noexcept
	{
		return format;
	}

	const Rect Image::getRect() const noexcept
	{
		return _rect;
	}

	Filter * Image::getSourceFilter() const noexcept
	{
		return sourceFilter;
	}

	bool Image::isCompatible(const PixelBuffer::FormatType type) noexcept
	{
		std::unordered_map<PixelBuffer::FormatType, Image::FormatType> dic;
		dic[PixelBuffer::FormatType::rgb8] = Image::FormatType::rgb8;
		dic[PixelBuffer::FormatType::rgba8] = Image::FormatType::rgba8;
		return dic.end() != dic.find(type);
	}

	Image::FormatType Image::pixelFormatToImageFormat(const PixelBuffer::FormatType type) noexcept
	{
		std::unordered_map<PixelBuffer::FormatType, Image::FormatType> dic;
		dic[PixelBuffer::FormatType::rgb8] = Image::FormatType::rgb8;
		dic[PixelBuffer::FormatType::rgba8] = Image::FormatType::rgba8;
		assert(isCompatible(type));
		return dic[type];
	}

	const PixelBuffer * Image::getPixelBuffer() const noexcept
	{
		return pixelBuffer;
	}

	PixelBuffer * Image::getMutablePixelBuffer() const noexcept
	{
		return pixelBuffer;
	}
}