#ifndef IMAGEVERTEX_H
#define IMAGEVERTEX_H

#include <glm/glm.hpp>
#include <bgfx/bgfx.h>
#include "defs.hpp"

namespace ks
{
	struct KSImage_API ImageVertex
	{
		glm::vec3 position;
		glm::vec2 texcoord;
		static bgfx::VertexLayout vertexLayout;

		explicit ImageVertex(const glm::vec3 position, const glm::vec2 texcoord)
			:position(position), texcoord(texcoord)
		{
		}

		explicit ImageVertex()
		{

		}
	};
}

#endif // !IMAGEVERTEX_H