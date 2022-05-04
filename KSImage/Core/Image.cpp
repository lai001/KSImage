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

	std::shared_ptr<Image> Image::create()
	{
		std::shared_ptr<Image> image = std::make_shared<Image>();
		return image;
	}

	std::shared_ptr<Image> Image::create(const ks::Rect & rect)
	{
		std::shared_ptr<Image> image = std::make_shared<Image>();
		image->_rect = rect;
		return image;
	}

	std::shared_ptr<Image> ks::Image::create(const std::string & filePath)
	{
		assert(ks::IImageDecoder::shared);
		ks::PixelBuffer* pixelBuffer = ks::IImageDecoder::shared->create(filePath);
		return createRetain(pixelBuffer);
	}

	std::shared_ptr<Image> Image::createRetain(ks::PixelBuffer* pixelBuffer)
	{
		assert(pixelBuffer);
		std::shared_ptr<Image> image = std::make_shared<ks::Image>();
		image->format = pixelFormatToImageFormat(pixelBuffer->getType());
		image->pixelBuffer = pixelBuffer;
		image->_rect = ks::Rect(0, 0, pixelBuffer->getWidth(), pixelBuffer->getHeight());
		return image;
	}

	const unsigned char * Image::getData() const noexcept
	{
		if (ks::PixelBuffer* buffer = pixelBuffer)
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
		if (ks::PixelBuffer* buffer = pixelBuffer)
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
		if (ks::PixelBuffer* buffer = pixelBuffer)
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
		if (ks::PixelBuffer* buffer = pixelBuffer)
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

	bool Image::isCompatible(const ks::PixelBuffer::FormatType type) noexcept
	{
		std::unordered_map<ks::PixelBuffer::FormatType, ks::Image::FormatType> dic;
		dic[ks::PixelBuffer::FormatType::rgb8] = ks::Image::FormatType::rgb8;
		dic[ks::PixelBuffer::FormatType::rgba8] = ks::Image::FormatType::rgba8;
		return dic.end() != dic.find(type);
	}

	ks::Image::FormatType Image::pixelFormatToImageFormat(const ks::PixelBuffer::FormatType type) noexcept
	{
		std::unordered_map<ks::PixelBuffer::FormatType, ks::Image::FormatType> dic;
		dic[ks::PixelBuffer::FormatType::rgb8] = ks::Image::FormatType::rgb8;
		dic[ks::PixelBuffer::FormatType::rgba8] = ks::Image::FormatType::rgba8;
		assert(isCompatible(type));
		return dic[type];
	}
}