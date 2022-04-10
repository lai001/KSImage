#include "FrameBuffer.hpp"
#include <assert.h>

namespace ks
{
	FrameBuffer::~FrameBuffer()
	{
		assert(bgfx::isValid(m_FrameBufferHandle));
		assert(bgfx::isValid(m_rb));
		bgfx::destroy(m_rb);
		bgfx::destroy(m_FrameBufferHandle);
	}

	FrameBuffer::FrameBuffer(const int width, const int height)
		:width(width),
		height(height)
	{
		m_rb = bgfx::createTexture2D(width, height, false, 1, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_BLIT_DST | BGFX_TEXTURE_READ_BACK);
		m_FrameBufferHandle = bgfx::createFrameBuffer(width, height, bgfx::TextureFormat::RGBA8);
		assert(bgfx::isValid(m_FrameBufferHandle));
		assert(bgfx::isValid(m_rb));
	}

	std::shared_ptr<FrameBuffer> FrameBuffer::create(const int width, const int height) noexcept
	{
		return std::make_shared<FrameBuffer>(width, height);
	}
}