#ifndef VERTEXCREATE_Hpp
#define VERTEXCREATE_Hpp

#include <functional>
#include <bgfx/bgfx.h>
#include "defs.hpp"

namespace ks
{
	bgfx::VertexLayout KSImage_API createVertextLayout(std::function<void(bgfx::VertexLayout&)> closure) noexcept;
}

#endif // !VERTEXCREATE_Hpp