#ifndef KERNEL_TEXTURE2D_HPP
#define KERNEL_TEXTURE2D_HPP

#include <Foundation/Foundation.hpp>
#include <bgfx/bgfx.h>
#include <glm/glm.hpp>
#include "Image.hpp"
#include "PixelBuffer.hpp"

namespace ks
{
	class KernelTexture2D : public boost::noncopyable
	{
	private:
		bgfx::TextureHandle textureHandle = BGFX_INVALID_HANDLE;

	public:
		~KernelTexture2D();
		static std::shared_ptr<KernelTexture2D> createPlaceHolder(/*const Info& info*/) noexcept;

		void copyImage(const std::shared_ptr<ks::Image> image) noexcept;
		void copyPixelBuffer(const std::shared_ptr<ks::PixelBuffer> PixelBuffer) noexcept;

		void bind(const uint8_t stage, const bgfx::UniformHandle& uniformHandle) const noexcept;

		bgfx::TextureFormat::Enum chooseFormat(const int channelCount) const noexcept;
	};
}

#endif // !KERNEL_TEXTURE2D_HPP
