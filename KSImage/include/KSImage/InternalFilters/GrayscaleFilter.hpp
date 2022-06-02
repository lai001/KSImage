#ifndef KSImage_InternalFilters_GrayscaleFilter_hpp
#define KSImage_InternalFilters_GrayscaleFilter_hpp

#include "Filter.hpp"
#include "defs.hpp"

namespace ks
{
	class KSImage_API GrayscaleFilter : public Filter
	{
	public:
		Image* inputImage = nullptr;

		virtual Image* outputImage(const Rect* rect = nullptr) override;
		static GrayscaleFilter* create() noexcept;
	};
}

#endif // !KSImage_InternalFilters_GrayscaleFilter_hpp
