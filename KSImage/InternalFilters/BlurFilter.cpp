#include "InternalFilters/BlurFilter.hpp"

namespace ks
{
	Image* BlurFilter::outputImage(const Rect* rect)
	{
		setValues({ inputImage });
		return Filter::outputImage(rect);
	}

	BlurFilter* BlurFilter::create() noexcept
	{
		static std::shared_ptr<Kernel> kernel = Kernel::create(R"(
kernel float4 fragmentShader(Texture2D inputImage, PS_INPUT ps_input)
{
	const float offset = 1.0 / 300.0;  
	float2 offsets[9] = {
		float2(-offset,  offset),
		float2( 0.0f,    offset),
		float2( offset,  offset),
		float2(-offset,  0.0f),
		float2( 0.0f,    0.0f),
		float2( offset,  0.0f),
		float2(-offset, -offset),
		float2( 0.0f,   -offset),
		float2( offset, -offset)
	};

	float kernel[9] = {
        1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0,
		2.0 / 16.0, 4.0 / 16.0, 2.0 / 16.0,
		1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0  
    };

	float3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
		float4 _color = inputImage.Sample(ks_sampler0(), ps_input.texcoord + offsets[i]);
        sampleTex[i] = float3(_color.x, _color.y, _color.z);
    }

    float3 col = float3(0.0, 0.0, 0.0);
    for(int i = 0; i < 9; i++)
	{
        col += sampleTex[i] * kernel[i];
	}

	float4 color = inputImage.Sample(ks_sampler0(), ps_input.texcoord);
    color = float4(col.x, col.y, col.z, color.w);
	return color;
}
)");
		BlurFilter* ptr = new BlurFilter();
		ptr->kernel = kernel;
		return ptr;
	}
}