#ifndef KSRenderEngine_Example_ImageIO_hpp
#define KSRenderEngine_Example_ImageIO_hpp

#include <string>
#include <Foundation/Foundation.hpp>

class ImageIO
{
public:
	static ks::PixelBuffer* readImageFromFilePath2(const std::string& path);
	static bool saveImage(const ks::PixelBuffer& buffer, const std::string& targetPath);
};

#endif // !KSRenderEngine_Example_ImageIO_hpp