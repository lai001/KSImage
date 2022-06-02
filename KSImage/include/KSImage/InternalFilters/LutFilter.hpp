#ifndef KSImage_InternalFilters_LutFilter_hpp
#define KSImage_InternalFilters_LutFilter_hpp

#include "Filter.hpp"
#include "defs.hpp"

namespace ks
{
	class KSImage_API LutFilter : public Filter
	{
	public:
		float intensity = 1.0f;
		ks::Image* inputImage = nullptr;
		ks::Image* lutImage = nullptr;

		virtual ks::Image* outputImage(const ks::Rect* rect = nullptr) override;
		static LutFilter* create() noexcept;
	};
}

#endif // ! KSImage_InternalFilters_LutFilter_hpp