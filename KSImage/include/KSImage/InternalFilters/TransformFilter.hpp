#ifndef KSImage_InternalFilters_TransformFilter_hpp
#define KSImage_InternalFilters_TransformFilter_hpp

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
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

#endif // !KSImage_InternalFilters_TransformFilter_hpp
