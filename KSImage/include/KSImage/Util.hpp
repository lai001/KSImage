#ifndef KSUTIL_HPP
#define KSUTIL_HPP

#include <bgfx/bgfx.h>
#include <Foundation/Foundation.hpp>
#include "Image.hpp"
#include "Rect.hpp"
#include "defs.hpp"
#include "RectTransDescription.hpp"

namespace ks
{
	struct KSImage_API ShaderPaire
	{
		std::string vertexPath;
		std::string fragmentPath;
	};

	std::string KSImage_API shaderDir() noexcept;

	ShaderPaire KSImage_API getShaderPaire(const std::string& name) noexcept;

	Rect KSImage_API getUnionRect(const std::vector<ks::Image>& images) noexcept;
	Rect KSImage_API getUnionRect(const std::vector<std::shared_ptr<ks::Image>>& images) noexcept;

	Rect KSImage_API makeRectAspectFit(const float aspectWidthRatio, const float aspectHeightRatio, const Rect& boundingRect) noexcept;

	ks::RectTransDescription KSImage_API convertToNDC(const ks::RectTransDescription& des) noexcept;

	ks::RectTransDescription KSImage_API convertToNDC(const ks::RectTransDescription& des, const ks::Rect& boundingBox) noexcept;
}

#endif // !KSUTIL_HPP