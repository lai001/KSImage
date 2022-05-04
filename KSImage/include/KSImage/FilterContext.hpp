#ifndef FILTER_CONTEXT_HPP
#define FILTER_CONTEXT_HPP

#include <Foundation/Foundation.hpp>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include "Image.hpp"
#include "defs.hpp"

namespace ks
{
	class KSImage_API FilterContext : public boost::noncopyable
	{
	public:
		ks::PixelBuffer* render(const ks::Image* image, const ks::Rect& bound) noexcept;

		static FilterContext* create() noexcept;

		static void Init(const bgfx::Init& init, void* nwh) noexcept;
		static void Init(void* nwh, const unsigned int width = 1280, const unsigned int height = 720) noexcept;
		static void shutdown() noexcept;

	private:
		static unsigned int frameNumber;
	};
}

#endif // !FILTER_CONTEXT_HPP