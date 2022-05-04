#ifndef IImageDecoder_hpp
#define IImageDecoder_hpp

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


#endif // !IImageDecoder_hpp