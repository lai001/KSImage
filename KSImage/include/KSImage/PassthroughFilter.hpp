#ifndef KSImage_PassthroughFilter_hpp
#define KSImage_PassthroughFilter_hpp

#include "Filter.hpp"
#include "defs.hpp"

namespace ks
{
	class KSImage_API PassthroughFilter : public Filter
	{
	public:
		Rect innerRect;
		Image* inputImage = nullptr;

		virtual Image* outputImage(const Rect* rect = nullptr) override;
		static PassthroughFilter* create() noexcept;
		virtual KernelRenderInstruction onPrepare(const Rect& renderRect) override;
	};
}

#endif // !KSImage_PassthroughFilter_hpp
