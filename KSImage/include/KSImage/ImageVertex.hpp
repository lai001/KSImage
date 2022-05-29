#ifndef KSImage_ImageVertex_hpp
#define KSImage_ImageVertex_hpp

#include <glm/glm.hpp>
#include "defs.hpp"

namespace ks
{
	struct KSImage_API ImageVertex
	{
		glm::vec3 position;
		glm::vec2 texcoord;

		explicit ImageVertex(const glm::vec3& position, const glm::vec2& texcoord)
			:position(position), texcoord(texcoord)
		{
		}

		explicit ImageVertex()
		{

		}
	};
}

#endif // !KSImage_ImageVertex_hpp