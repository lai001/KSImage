#include "ImageDecoder.hpp"
#include "ImageIO.hpp"
#include <Foundation/Foundation.hpp>

ks::PixelBuffer * ImageDecoder::create(const std::string & filePath)
{
	return ImageIO::readImageFromFilePath2(filePath);
}