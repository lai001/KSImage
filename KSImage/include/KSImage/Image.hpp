#ifndef KSImage_Image_hpp
#define KSImage_Image_hpp

#include <Foundation/Foundation.hpp>
#include "Rect.hpp"
#include "defs.hpp"

namespace ks
{
	class KSImage_API Image: public noncopyable
	{
		friend class Filter;

	public:
		enum class FormatType
		{
			rgb8,
			rgba8
		};

	private:
		Rect _rect;
		Image::FormatType format = Image::FormatType::rgba8;
		Filter* sourceFilter = nullptr;
		const ks::PixelBuffer* pixelBuffer = nullptr;
		std::function<void()> cleanClosure;

	public:
		~Image();

	public:
		static Image* create();
		static Image* create(const ks::Rect& rect);
		static Image* create(const std::string& filePath);
		static Image* createRetain(ks::PixelBuffer* pixelBuffer);
		static Image* createBorrow(const ks::PixelBuffer* pixelBuffer);

		const unsigned char* getData() const noexcept;
		const int getSourceWidth() const noexcept;
		const int getSourceHeight() const noexcept;
		const int getSourceChannels() const noexcept;
		const Image::FormatType getImageFormat() const noexcept;
		const Rect getRect() const noexcept;

		Filter* getSourceFilter() const noexcept;

		static bool isCompatible(const ks::PixelBuffer::FormatType type) noexcept;
		static ks::Image::FormatType pixelFormatToImageFormat(const ks::PixelBuffer::FormatType type) noexcept;
		const ks::PixelBuffer* getPixelBuffer() const noexcept;
	};
}

#endif // !KSImage_Image_hpp