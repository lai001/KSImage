#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <Foundation/Foundation.hpp>
#include "Rect.hpp"

namespace ks
{
	enum ImageFormat
	{
		rgb8,
		rgba8,
		unknow
	};

	class Image: public boost::noncopyable
	{
		friend class Filter;
	private:
		int _sourceWidth;
		int _sourceHeight;
		int _sourceChannels;
		Rect _rect;
		unsigned char* _data = nullptr;
		std::string _filePath;
		ImageFormat format = ImageFormat::unknow;
		Filter* sourceFilter = nullptr;

	public:
		~Image();

	public:
		static std::shared_ptr<Image> create();
		static std::shared_ptr<Image> create(const std::string& filePath);

		const unsigned char* getData() const noexcept;
		const int getSourceWidth() const noexcept;
		const int getSourceHeight() const noexcept;
		const int getSourceChannels() const noexcept;
		const ImageFormat getImageFormat() const noexcept;
		const Rect getRect() const noexcept;

		void forceUpdateRect(const Rect& rect) noexcept;

		Filter* getSourceFilter() const noexcept;
	};
}

#endif // !IMAGE_HPP