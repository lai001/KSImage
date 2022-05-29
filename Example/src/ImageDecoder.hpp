#ifndef IMAGEDECODERHPP
#define IMAGEDECODERHPP

//#include <stb_image_write.h>
//#include <stb_image.h>
#include <KSImage/KSImage.hpp>


class ImageDecoder : public ks::IImageDecoder
{
public:
	virtual ks::PixelBuffer * create(const std::string & filePath) override;
};

#endif // !IMAGEDECODERHPP