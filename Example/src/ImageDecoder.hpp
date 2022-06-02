#ifndef KSImage_Example_src_ImageDecoder_hpp
#define KSImage_Example_src_ImageDecoder_hpp

#include <KSImage/KSImage.hpp>

class ImageDecoder : public ks::IImageDecoder
{
public:
	virtual ks::PixelBuffer * create(const std::string & filePath) override;
};

#endif // !KSImage_Example_src_ImageDecoder_hpp