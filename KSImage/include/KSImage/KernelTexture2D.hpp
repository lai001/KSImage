#ifndef KERNEL_TEXTURE2D_HPP
#define KERNEL_TEXTURE2D_HPP

#include <Foundation/Foundation.hpp>
#include <bgfx/bgfx.h>
#include <glm/glm.hpp>
#include "Image.hpp"
#include "defs.hpp"

namespace ks
{
	class KSImage_API KernelTexture2D : public boost::noncopyable
	{
	private:
		bgfx::TextureHandle textureHandle = BGFX_INVALID_HANDLE;

	public:
		~KernelTexture2D();
		static KernelTexture2D* createPlaceHolder(/*const Info& info*/) noexcept;

		void copyImage(const std::shared_ptr<ks::Image> image) noexcept;
		void copyPixelBuffer(const ks::PixelBuffer& pixelBuffer) noexcept;
		void copyTextureHandle(bgfx::TextureHandle handle) noexcept;

		void bind(const uint8_t stage, const bgfx::UniformHandle& uniformHandle) const noexcept;

		static bgfx::TextureFormat::Enum chooseFormat(const int channelCount) noexcept;
		static bgfx::TextureFormat::Enum chooseFormat(const ks::PixelBuffer::FormatType formatType) noexcept;
		static bgfx::TextureFormat::Enum chooseFormat(const ks::Image::FormatType formatType) noexcept;

		bgfx::TextureHandle getTextureHandle() const noexcept;
	};
}

#endif // !KERNEL_TEXTURE2D_HPP
