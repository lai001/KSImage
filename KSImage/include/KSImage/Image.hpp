#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <Foundation/Foundation.hpp>
#include "Rect.hpp"
#include "defs.hpp"

namespace ks
{
	class KSImage_API Image: public boost::noncopyable
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
		ks::PixelBuffer* pixelBuffer = nullptr;

	public:
		~Image();

	public:
		static std::shared_ptr<Image> create();
		static std::shared_ptr<Image> create(const ks::Rect& rect);
		static std::shared_ptr<Image> create(const std::string& filePath);
		static std::shared_ptr<Image> createRetain(ks::PixelBuffer* pixelBuffer);

		const unsigned char* getData() const noexcept;
		const int getSourceWidth() const noexcept;
		const int getSourceHeight() const noexcept;
		const int getSourceChannels() const noexcept;
		const Image::FormatType getImageFormat() const noexcept;
		const Rect getRect() const noexcept;

		Filter* getSourceFilter() const noexcept;

		static bool isCompatible(const ks::PixelBuffer::FormatType type) noexcept;
		static ks::Image::FormatType pixelFormatToImageFormat(const ks::PixelBuffer::FormatType type) noexcept;
	};
}

#endif // !IMAGE_HPP