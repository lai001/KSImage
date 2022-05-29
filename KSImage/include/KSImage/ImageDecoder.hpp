#ifndef KSImage_ImageDecoder_hpp
#define KSImage_ImageDecoder_hpp

#include <Foundation/Foundation.hpp>
#include "defs.hpp"

namespace ks
{
	class KSImage_API IImageDecoder
	{
	public:
		virtual ks::PixelBuffer* create(const std::string& path) = 0;

		static ks::IImageDecoder* shared;
	};
}


#endif // !KSImage_ImageDecoder_hpp