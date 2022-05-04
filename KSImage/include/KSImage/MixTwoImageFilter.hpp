#ifndef REVERSE_COLOR_FILTER_HPP
#define REVERSE_COLOR_FILTER_HPP

#include "Filter.hpp"
#include "defs.hpp"

namespace ks
{
	class KSImage_API MixTwoImageFilter: public Filter
	{
	public:
		float u_intensity = 0.0;
		std::shared_ptr<ks::Image> inputImage = std::shared_ptr<ks::Image>(nullptr);
		std::shared_ptr<ks::Image> inputTargetImage = std::shared_ptr<ks::Image>(nullptr);

		virtual std::shared_ptr<ks::Image> outputImage(const ks::Rect* rect = nullptr) override;
		static std::shared_ptr<MixTwoImageFilter> create() noexcept;

	};
}

#endif // !REVERSE_COLOR_FILTER_HPP