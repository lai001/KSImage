#include "ImageIO.hpp"
#include <assert.h>
#include <mutex>
#include <OpenImageIO/imageio.h>

using namespace OIIO;

ks::PixelBuffer* ImageIO::readImageFromFilePath2(const std::string & path)
{
	OpenImageIO_v2_3::ImageInput::unique_ptr inp = ImageInput::open(path);
	assert(inp);
	const ImageSpec &spec = inp->spec();
	int xres = spec.width;
	int yres = spec.height;
	int channels = spec.nchannels;
	std::vector<unsigned char> pixels(xres * yres * channels);
	ks::PixelBuffer* pixelBuffer = new ks::PixelBuffer(xres, yres, ks::PixelBuffer::FormatType::rgba8);
	inp->read_image(TypeDesc::UINT8, &pixels[0]);
	memset(pixelBuffer->getMutableData()[0], 255, xres * yres * 4);
	for (size_t i = 0; i < yres; i++)
	{
		for (size_t j = 0; j < xres; j++)
		{
			unsigned char* src = pixels.data();
			unsigned char* dst = pixelBuffer->getMutableData()[0];
			memcpy(dst + (i * xres + j) * 4,
				src + (i * xres + j) * channels,
				channels);
		}
	}
	inp->close();
	return pixelBuffer;
}

bool ImageIO::saveImage(const ks::PixelBuffer & pixelBuffer, const std::string & targetPath)
{
	const char *filename = targetPath.c_str();
	int xres = pixelBuffer.getWidth();
	int yres = pixelBuffer.getHeight();
	int channels = pixelBuffer.getChannels();  
	std::unique_ptr<ImageOutput> out = ImageOutput::create(filename);
	assert(out);
	ImageSpec spec(xres, yres, channels, TypeDesc::UINT8);
	spec.channelnames = { "R", "G", "B", "A" };
	bool status = true;
	status = out->open(filename, spec);
	status = out->write_image(TypeDesc::UINT8, pixelBuffer.getImmutableData()[0]);
	status = out->close();
	return status;
}
