#include "InternalFilters/SourceOverFilter.hpp"

namespace ks
{
	Image* SourceOverFilter::outputImage(const Rect* rect)
	{
		setValues({ inputImage, inputTargetImage });
		return Filter::outputImage(rect);
	}

	SourceOverFilter* SourceOverFilter::create() noexcept
	{
		static std::shared_ptr<Kernel> kernel = Kernel::create(R"(
kernel float4 fragmentShader(Texture2D inputImage, Texture2D inputTargetImage, PS_INPUT ps_input)
{
	float4 color0 = ks_texture2DBorder(inputImage, ks_sampler0(), ks_sampler0Transform(ps_input));
	float4 color1 = ks_texture2DBorder(inputTargetImage, ks_sampler1(), ks_sampler1Transform(ps_input));
	float4 mixColor = color0 + (1.0 - color0.w) * color1;
	return mixColor;
}
)");

		SourceOverFilter* ptr = new SourceOverFilter();
		ptr->kernel = kernel;
		return ptr;
	}
}