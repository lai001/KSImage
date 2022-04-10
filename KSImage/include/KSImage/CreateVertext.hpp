#ifndef VERTEXCREATE_Hpp
#define VERTEXCREATE_Hpp

#include <functional>
#include <bgfx/bgfx.h>

namespace ks
{
	static bgfx::VertexLayout createVertextLayout(std::function<void(bgfx::VertexLayout&)> closure)
	{
		bgfx::VertexLayout layout;
		closure(layout);
		return layout;
	}
}

#endif // !VERTEXCREATE_Hpp