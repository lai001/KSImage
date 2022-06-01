#ifndef KSImage_MixTwoImageFilter_hpp
#define KSImage_MixTwoImageFilter_hpp

#include "Filter.hpp"
#include "defs.hpp"

namespace ks
{
	class KSImage_API MixTwoImageFilter: public Filter
	{
	public:
		float u_intensity = 0.0;
		ks::Image* inputImage = nullptr;
		ks::Image* inputTargetImage = nullptr;

		virtual ks::Image* outputImage(const ks::Rect* rect = nullptr) override;
		static MixTwoImageFilter* create() noexcept;

	};
}

#endif // !KSImage_MixTwoImageFilter_hpp