#include "ImageDecoder.hpp"

ks::PixelBuffer * ImageDecoder::create(const std::string & filePath)
{
	int sourceWidth;
	int sourceHeight;
	int sourceChannels;

	unsigned char* data = stbi_load(filePath.c_str(), &sourceWidth, &sourceHeight, &sourceChannels, 0);
	if (data == nullptr)
	{
		spdlog::error(fmt::format("Can not open {}", filePath));
		assert(false);
	}
	assert(sourceWidth > 0);
	assert(sourceHeight > 0);
	assert(sourceChannels > 0);

	ks::PixelBuffer::FormatType pixelFormat;
	if (sourceChannels == 3)
	{
		pixelFormat = ks::PixelBuffer::FormatType::rgb8;
	}
	else if (sourceChannels == 4)
	{
		pixelFormat = ks::PixelBuffer::FormatType::rgba8;
	}
	else
	{
		spdlog::error("Incorrect channels");
		assert(false);
	}
	unsigned char* pixelBufferData[ks::PixelBuffer::maxPlanes];
	pixelBufferData[0] = data;
	ks::PixelBuffer* pixelBuffer = new ks::PixelBuffer(sourceWidth, sourceHeight, pixelFormat, pixelBufferData, 1);

	return pixelBuffer;
}