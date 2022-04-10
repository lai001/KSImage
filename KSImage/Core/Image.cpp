#include "Image.hpp"
#include <stb_image.h>
#include <spdlog/spdlog.h>
#include "Util.hpp"

namespace ks
{
	Image::~Image()
	{
		if (_data)
		{
			stbi_image_free(_data);
		}
	}

	std::shared_ptr<Image> Image::create()
	{
		std::shared_ptr<Image> image = std::make_shared<Image>();
		return image;
	}

	std::shared_ptr<Image> ks::Image::create(const std::string & filePath)
	{
		std::shared_ptr<Image> ptr = std::make_shared<Image>();
		unsigned char* data = stbi_load(filePath.c_str(), &ptr->_sourceWidth, &ptr->_sourceHeight, &ptr->_sourceChannels, 0);
		if (data == nullptr)
		{
			spdlog::error(fmt::format("Can not open {}", filePath));
			assert(false);
		}
		ptr->_data = data;
		ptr->_rect = Rect(0, 0, ptr->_sourceWidth, ptr->_sourceHeight);
		if (ptr->_sourceChannels == 3)
		{
			ptr->format = ImageFormat::rgb8;
		}
		else if (ptr->_sourceChannels == 4)
		{
			ptr->format = ImageFormat::rgba8;
		}
		else
		{
			spdlog::error("Incorrect channels");
			assert(false);
		}
		return ptr;
	}

	const unsigned char * Image::getData() const noexcept
	{
		return _data;
	}

	const int Image::getSourceWidth() const noexcept
	{
		return _sourceWidth;
	}

	const int Image::getSourceHeight() const noexcept
	{
		return _sourceHeight;
	}

	const int Image::getSourceChannels() const noexcept
	{
		return _sourceChannels;
	}

	const ImageFormat Image::getImageFormat() const noexcept
	{
		return format;
	}

	const Rect Image::getRect() const noexcept
	{
		return _rect;
	}

	void Image::forceUpdateRect(const Rect & rect) noexcept
	{
		_rect = rect;
	}
	Filter * Image::getSourceFilter() const noexcept
	{
		return sourceFilter;
	}
}