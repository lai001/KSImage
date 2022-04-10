#ifndef PIXELBUFFER_HPP
#define PIXELBUFFER_HPP

#include <Foundation/Foundation.hpp>

namespace ks
{
	class PixelBuffer : public boost::noncopyable
	{

	public:
		enum class PixelBufferFormatType
		{
			rgba8,
			rgb8,
		};

	private:
		int _width;
		int _height;
		void* _data = nullptr;
		PixelBufferFormatType _type;

	public:
		explicit PixelBuffer(const int width, const int height, const PixelBufferFormatType type, void* data);
		explicit PixelBuffer(const int width, const int height, const PixelBufferFormatType type);
		~PixelBuffer();

		static std::shared_ptr<PixelBuffer> create(const int width, const int height, const PixelBufferFormatType type, void* data) noexcept;
		static std::shared_ptr<PixelBuffer> create(const int width, const int height, const PixelBufferFormatType type) noexcept;
		static unsigned int getBytesCount(const int width, const int height, const PixelBufferFormatType type) noexcept;

		int getWidth() const noexcept;
		int getHeight() const noexcept;
		const void* getImmutableData() const noexcept;
		void* getMutableData() noexcept;
		PixelBufferFormatType getType() const noexcept;
		unsigned int getChannels() const noexcept;
	};
}

#endif // !PIXELBUFFER_HPP