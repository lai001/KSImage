#include "MixTwoImageFilter.hpp"

namespace ks
{
	std::shared_ptr<ks::Image> ks::MixTwoImageFilter::outputImage()
	{
		setValues({ inputImage, u_intensity, inputTargetImage });
		std::shared_ptr<ks::Image> superImage = Filter::outputImage();
		return superImage;
	}

	std::shared_ptr<MixTwoImageFilter> MixTwoImageFilter::create() noexcept
	{
		static std::shared_ptr<Kernel> kernel = Kernel::create(R"(

kernel vec4 fragmentShader(BgfxSampler2D inputImage, float intensity, BgfxSampler2D inputTargetImage)
{
	vec2 uv = ks_destCoord();
	vec4 color0 = ks_texture2DBorder(inputImage, ks_sampler0Transform());
	vec4 color1 = ks_texture2DBorder(inputTargetImage, ks_sampler1Transform());
	return mix(color0, color1, intensity);
}

)");

		std::shared_ptr<MixTwoImageFilter> ptr = std::make_shared<MixTwoImageFilter>();
		ptr->kernel = kernel;
		return ptr;
	}
}