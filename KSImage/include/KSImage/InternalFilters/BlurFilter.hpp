#ifndef KSImage_InternalFilters_BlurFilter_hpp
#define KSImage_InternalFilters_BlurFilter_hpp

#include "Filter.hpp"
#include "defs.hpp"

namespace ks
{
	class KSImage_API BlurFilter : public Filter
	{
	public:
		Image* inputImage = nullptr;

		virtual Image* outputImage(const Rect* rect = nullptr) override;
		static BlurFilter* create() noexcept;
	};
}

#endif // !KSImage_InternalFilters_BlurFilter_hpp
