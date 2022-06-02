#include "InternalFilters/GrayscaleFilter.hpp"

namespace ks
{
	Image* GrayscaleFilter::outputImage(const Rect* rect)
	{
		setValues({ inputImage });
		return Filter::outputImage(rect);
	}

	GrayscaleFilter* GrayscaleFilter::create() noexcept
	{
		static std::shared_ptr<Kernel> kernel = Kernel::create(R"(
kernel float4 fragmentShader(Texture2D inputImage, PS_INPUT ps_input)
{
	float4 color = inputImage.Sample(ks_sampler0(), ps_input.texcoord);
    float average = 0.2126 * color.x + 0.7152 * color.y + 0.0722 * color.z;
    color = float4(average, average, average, color.z);
	return color;
}
)");
		GrayscaleFilter* ptr = new GrayscaleFilter();
		ptr->kernel = kernel;
		return ptr;
	}
}