#include "PixelBuffer.hpp"

namespace ks
{
	PixelBuffer::PixelBuffer(const int width, const int height, const PixelBufferFormatType type, void * data)
		: _width(width), _height(height), _type(type), _data(data)
	{
	}

	PixelBuffer::PixelBuffer(const int width, const int height, const PixelBufferFormatType type)
		: _width(width), _height(height), _type(type), _data(new unsigned char[PixelBuffer::getBytesCount(width, height, type)])
	{
	}

	PixelBuffer::~PixelBuffer()
	{
		delete _data;
	}

	std::shared_ptr<PixelBuffer> PixelBuffer::create(const int width, const int height, const PixelBufferFormatType type, void * data) noexcept
	{
		return std::make_shared<PixelBuffer>(width, height, type, data);
	}

	std::shared_ptr<PixelBuffer> PixelBuffer::create(const int width, const int height, const PixelBufferFormatType type) noexcept
	{
		return std::make_shared<PixelBuffer>(width, height, type);
	}

	unsigned int PixelBuffer::getBytesCount(const int width, const int height, const PixelBufferFormatType type) noexcept
	{
		std::unordered_map<PixelBufferFormatType, unsigned int> channels;
		channels[PixelBufferFormatType::rgb8] = 3;
		channels[PixelBufferFormatType::rgba8] = 4;
		return width * height * channels[type];
	}

	int PixelBuffer::getWidth() const noexcept
	{
		return _width;
	}

	int PixelBuffer::getHeight() const noexcept
	{
		return _height;
	}

	const void * PixelBuffer::getImmutableData() const noexcept
	{
		return _data;
	}

	void * PixelBuffer::getMutableData() noexcept
	{
		return _data;
	}

	PixelBuffer::PixelBufferFormatType PixelBuffer::getType() const noexcept
	{
		return _type;
	}
	unsigned int PixelBuffer::getChannels() const noexcept
	{
		std::unordered_map<PixelBufferFormatType, unsigned int> channels;
		channels[PixelBufferFormatType::rgb8] = 3;
		channels[PixelBufferFormatType::rgba8] = 4;
		return channels[_type];
	}
}