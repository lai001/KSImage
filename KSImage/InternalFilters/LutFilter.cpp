#include "InternalFilters/LutFilter.hpp"

namespace ks
{
	Image* LutFilter::outputImage(const Rect* rect)
	{
		assert(lutImage->getSourceWidth() == lutImage->getSourceHeight());
		assert(lutImage->getSourceWidth() == 512);
		setValues({ inputImage, lutImage, intensity });
		return Filter::outputImage(rect);
	}

	LutFilter* LutFilter::create() noexcept
	{
		static std::shared_ptr<Kernel> kernel = Kernel::create(R"(
kernel float4 fragmentShader(Texture2D inputImage, Texture2D lutImage, float intensity, PS_INPUT ps_input)
{
	float4 textureColor = ks_texture2DBorder(inputImage, ks_sampler0(), ks_sampler0Transform(ps_input));
	float zIndex = textureColor.z * 63.0;
	float2 smallCubeUV;
	smallCubeUV.y = floor(floor(zIndex) / 8.0);
    smallCubeUV.x = floor(zIndex) - (smallCubeUV.y * 8.0); 
    float2 lutUV;
    lutUV.x = (smallCubeUV.x * 0.125) + 0.5/512.0 + ((0.125 - 1.0/512.0) * textureColor.x);
    lutUV.y = (smallCubeUV.y * 0.125) + 0.5/512.0 + ((0.125 - 1.0/512.0) * textureColor.y);
    float4 mapColor = ks_texture2DBorder(lutImage, ks_sampler1(), lutUV);
	float4 mixColor = lerp(textureColor, float4(mapColor.x, mapColor.y, mapColor.z, textureColor.w), intensity);
	return mixColor;
}
)");

		LutFilter* ptr = new LutFilter();
		ptr->kernel = kernel;
		return ptr;
	}
}