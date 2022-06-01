#ifndef KSImage_TransformFilter_hpp
#define KSImage_TransformFilter_hpp

#include <glm/glm.hpp>
#include "Filter.hpp"
#include "defs.hpp"

namespace ks
{
	class KSImage_API TransformFilter: public Filter
	{
	public:
		Image* inputImage = nullptr;
		glm::mat3 transform = glm::identity<glm::mat3>();

		virtual Image* outputImage(const Rect* rect = nullptr) override;
		static TransformFilter* create() noexcept;
		virtual KernelRenderInstruction onPrepare(const Rect& renderRect) override;
	};
}

#endif // !KSImage_TransformFilter_hpp
