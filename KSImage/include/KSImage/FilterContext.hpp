#ifndef FILTER_CONTEXT_HPP
#define FILTER_CONTEXT_HPP

#include <Foundation/Foundation.hpp>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include "Image.hpp"
#include "PixelBuffer.hpp"

namespace ks
{
	class FilterContext : public boost::noncopyable
	{
	public:
		std::shared_ptr<ks::PixelBuffer> render(const ks::Image* image) noexcept;

		static std::shared_ptr<FilterContext> create() noexcept;

		static void Init(const bgfx::Init& init, void* nwh) noexcept;
		static void Init(void* nwh, const unsigned int width = 1280, const unsigned int height = 720) noexcept;
	};
}

#endif // !FILTER_CONTEXT_HPP