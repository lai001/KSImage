#ifndef KSUTIL_HPP
#define KSUTIL_HPP

#include <bgfx/bgfx.h>
#include <Foundation/Foundation.hpp>
#include "Image.hpp"
#include "Rect.hpp"

namespace ks
{
	struct ShaderPaire
	{
		std::string vertexPath;
		std::string fragmentPath;
	};

	std::string shaderDir() noexcept;

	ShaderPaire getShaderPaire(const std::string& name) noexcept;

	Rect getUnionRect(const std::vector<ks::Image>& images) noexcept;
	Rect getUnionRect(const std::vector<std::shared_ptr<ks::Image>>& images) noexcept;
}

#endif // !KSUTIL_HPP