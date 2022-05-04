#include "CreateVertext.hpp"

namespace ks
{

	bgfx::VertexLayout createVertextLayout(std::function<void(bgfx::VertexLayout&)> closure) noexcept
	{
		bgfx::VertexLayout layout;
		closure(layout);
		return layout;
	}

}