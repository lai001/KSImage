#ifndef KSImage_InternalFilters_SourceOverFilter_hpp
#define KSImage_InternalFilters_SourceOverFilter_hpp

#include "Filter.hpp"
#include "defs.hpp"

namespace ks
{
	class KSImage_API SourceOverFilter : public Filter
	{
	public:
		ks::Image* inputImage = nullptr;
		ks::Image* inputTargetImage = nullptr;
		virtual ks::Image* outputImage(const ks::Rect* rect = nullptr) override;
		static SourceOverFilter* create() noexcept;
	};
}

#endif // !KSImage_InternalFilters_SourceOverFilter_hpp