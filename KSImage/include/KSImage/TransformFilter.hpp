#ifndef KSImage_TransformFilter_hpp
#define KSImage_TransformFilter_hpp

#include <glm/glm.hpp>
#include "Filter.hpp"
#include "defs.hpp"

namespace ks
{
	class KSImage_API TransformFilter: public ks::Filter
	{
	public:
		std::shared_ptr<ks::Image> inputImage = std::shared_ptr<ks::Image>(nullptr);
		glm::mat3 transform = glm::identity<glm::mat3>();

		virtual std::shared_ptr<ks::Image> outputImage(const ks::Rect* rect = nullptr) override;
		static std::shared_ptr<ks::TransformFilter> create() noexcept;
		virtual KernelRenderInstruction onPrepare(const ks::Rect& renderRect) override;
	};
}

#endif // !KSImage_TransformFilter_hpp
