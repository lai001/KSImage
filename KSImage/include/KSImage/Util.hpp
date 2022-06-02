#ifndef KSImage_Util_hpp
#define KSImage_Util_hpp

#include <Foundation/Foundation.hpp>
#include "Image.hpp"
#include "Rect.hpp"
#include "defs.hpp"
#include "RectTransDescription.hpp"

namespace ks
{
	Rect KSImage_API getUnionRect(const std::vector<ks::Image*>& images) noexcept;

	Rect KSImage_API makeRectAspectFit(const float aspectWidthRatio, const float aspectHeightRatio, const Rect& boundingRect) noexcept;

	ks::RectTransDescription KSImage_API convertToNDC(const ks::RectTransDescription& des) noexcept;

	ks::RectTransDescription KSImage_API convertToNDC(const ks::RectTransDescription& des, const ks::Rect& boundingBox) noexcept;

	ks::PixelBuffer* KSImage_API createOriginLUT(const unsigned int cubeLength = 64) noexcept;
}

#endif // !KSImage_Util_hpp