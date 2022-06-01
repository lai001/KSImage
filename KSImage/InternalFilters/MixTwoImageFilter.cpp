#include "MixTwoImageFilter.hpp"

namespace ks
{
	ks::Image* ks::MixTwoImageFilter::outputImage(const ks::Rect* rect)
	{
		setValues({ inputImage, u_intensity, inputTargetImage });
		return Filter::outputImage(rect);
	}

	MixTwoImageFilter* MixTwoImageFilter::create() noexcept
	{
		static std::shared_ptr<Kernel> kernel = Kernel::create(R"(
kernel float4 fragmentShader(Texture2D inputImage, float intensity, Texture2D inputTargetImage, PS_INPUT ps_input)
{
	float2 uv = ps_input.texcoord;
	float4 color0 = ks_texture2DBorder(inputImage, ks_sampler0(), ks_sampler0Transform(ps_input));
	float4 color1 = ks_texture2DBorder(inputTargetImage, ks_sampler1(), ks_sampler1Transform(ps_input));
	float4 mixColor = lerp(color0, color1, intensity);
	return mixColor;
}
)");

		MixTwoImageFilter* ptr = new MixTwoImageFilter();
		ptr->kernel = kernel;
		return ptr;
	}
}