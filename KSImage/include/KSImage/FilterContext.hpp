#ifndef KSImage_FilterContext_hpp
#define KSImage_FilterContext_hpp

#include <Foundation/Foundation.hpp>
#include <KSRenderEngine/KSRenderEngine.hpp>
#include "Image.hpp"
#include "defs.hpp"

namespace ks
{
	class KSImage_API FilterContext : public noncopyable
	{
	public:
		static ks::IRenderEngine* renderEngine;

	public:
		static FilterContext* create() noexcept;

//#ifdef _WIN32
//		static void Init(const ks::D3D11RenderEngineCreateInfo& info) noexcept;
//#endif // _WIN32
//		static void Init(const ks::GLRenderEngineCreateInfo& info) noexcept;

		ks::PixelBuffer* render(const ks::Image& image, const ks::Rect& bound) const noexcept;
	};
}

#endif // !KSImage_FilterContext_hpp