#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP

#include <bgfx/bgfx.h>
#include <Foundation/Foundation.hpp>

namespace ks
{
	class FrameBuffer : public boost::noncopyable
	{
	public:
		bgfx::FrameBufferHandle m_FrameBufferHandle = BGFX_INVALID_HANDLE;
		bgfx::TextureHandle m_rb = BGFX_INVALID_HANDLE;
	public:
		const int width;
		const int height;

		explicit FrameBuffer(const int width, const int height);
		~FrameBuffer();

		static std::shared_ptr<FrameBuffer> create(const int width, const int height) noexcept;
	};
}

#endif // !FRAMEBUFFER_HPP