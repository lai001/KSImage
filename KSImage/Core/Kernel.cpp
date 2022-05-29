#include "Kernel.hpp"
#include "Util.hpp"

namespace ks
{
	IRenderEngine* Kernel::renderEngine = nullptr;

	const std::string DefaultVertexShaderCode = R"(
struct VS_INPUT
{
	float3 position: POSITION;
	float2 texcoord: TEXCOORD;
};

struct PS_INPUT
{
	float4 position: SV_POSITION;
	float2 texcoord: TEXCOORD;
};

PS_INPUT main(VS_INPUT input)
{
	PS_INPUT vs_output;
	vs_output.position = float4(input.position, 1.0);
	vs_output.texcoord = input.texcoord;
	return vs_output;
}
)";

	Kernel::~Kernel()
	{
		assert(shader);
		assert(renderEngine);
		renderEngine->erase(shader);

		if (renderBuffer)
		{
			renderEngine->erase(renderBuffer);
		}
	}

	std::shared_ptr<Kernel> Kernel::create(const std::string& kernelFragmentShader) noexcept
	{
		FragmentAnalysis::AnalysisResult analysisResult = FragmentAnalysis().analysis(kernelFragmentShader);

		std::shared_ptr<ks::Kernel> kernel = std::make_shared<Kernel>();
		kernel->shader = Kernel::renderEngine->createShader(DefaultVertexShaderCode, analysisResult.fragmentShaderCode);
		kernel->analysisResult = analysisResult;
		assert(kernel->shader);
		return kernel;
	}

	void Kernel::setVertexObject(const void * vertexBuffer,
		const unsigned int vertexCount,
		const unsigned int stride,
		const void * indexBuffer, 
		const unsigned int indexCount, 
		ks::IIndexBuffer::IndexDataType indexDataType) noexcept
	{
		assert(renderEngine);
		assert(shader);
		if (renderBuffer)
		{
			renderEngine->erase(renderBuffer);
		}
		renderBuffer = renderEngine->createRenderBuffer(vertexBuffer,
			vertexCount,
			stride,
			*shader,
			indexBuffer,
			indexCount,
			indexDataType);
	}

	void Kernel::setUniform(const std::vector<ks::KernelUniform::Value>& uniformValues,
		std::function<ITexture2D*(unsigned int, glm::vec4*)> unretainTexture2D) noexcept
	{
		assert(analysisResult.uniformInfos.size() == uniformValues.size());
		assert(renderEngine);
		unsigned int textureIndex = 0;
		for (size_t i = 0; i < uniformValues.size(); i++)
		{
			const ks::KernelUniform::Value value = uniformValues[i];
			assert(analysisResult.uniformInfos[i].type == value.type);
			if (value.type == ks::KernelUniform::ValueType::texture2d)
			{
				const std::string texture2DName = FragmentAnalysis::ShareInfo::texture2DName(textureIndex);
				glm::vec4 samplerSapce;
				ks::ITexture2D *texture2d = unretainTexture2D(textureIndex, &samplerSapce);
				assert(texture2d);
				setTexture2D(texture2DName, *texture2d);
				const std::string uniformSamplerSpaceName = FragmentAnalysis::ShareInfo::uniformSamplerSpaceName(textureIndex);
				setUniform(uniformSamplerSpaceName, ks::UniformValue(samplerSapce));
				textureIndex += 1;
			}
			else
			{
				const std::string name = analysisResult.uniformInfos[i].name;
				const  ks::KernelUniform::ValueType type = analysisResult.uniformInfos[i].type;
				switch (type)
				{
				case ks::KernelUniform::ValueType::f32:
					setUniform(name, ks::UniformValue(value.f32));
					break;
				case ks::KernelUniform::ValueType::vec2:
					setUniform(name, ks::UniformValue(value.vec2));
					break;
				case ks::KernelUniform::ValueType::vec3:
					setUniform(name, ks::UniformValue(value.vec3));
					break;
				case ks::KernelUniform::ValueType::vec4:
					setUniform(name, ks::UniformValue(value.vec4));
					break;
				case ks::KernelUniform::ValueType::mat3:
					setUniform(name, ks::UniformValue(value.mat3));
					break;
				case ks::KernelUniform::ValueType::mat4:
					setUniform(name, ks::UniformValue(value.mat4));
					break;
				default:
					assert(false);
					break;
				}
			}
		}
	}

	void Kernel::setUniform(const std::string & name, const ks::UniformValue & value) noexcept
	{
		assert(shader);
		const std::string actualName = fmt::format("{}.{}", ks::FragmentAnalysis::ShareInfo::cBufferBlockName(), name);
		shader->setUniform(actualName, value);
	}

	void Kernel::setTexture2D(const std::string & name, const ks::ITexture2D & texture2D) noexcept
	{
		assert(shader);
		shader->setTexture2D(name, texture2D);
	}

	void Kernel::commit(std::shared_ptr<ks::IFrameBuffer> frameBuffer)
	{
		assert(renderEngine);

		ks::IBlendState* blendState = renderEngine->createBlendState(ks::BlendStateDescription::Addition::getDefault(), ks::BlendStateDescription::getDefault());
		ks::IDepthStencilState* depthStencilState = renderEngine->createDepthStencilState(ks::DepthStencilStateDescription::getDefault());
		ks::IRasterizerState* rasterizerState = renderEngine->createRasterizerState(ks::RasterizerStateDescription::getDefault());

		IRenderBuffer & renderBuffer = *this->renderBuffer;
		renderBuffer.setClearColor(glm::vec4(0.0, 0.0, 0.0, 0.0));
		renderBuffer.setViewport(0, 0, frameBuffer->getWidth(), frameBuffer->getHeight());
		renderBuffer.setClearBufferFlags(ks::ClearBufferFlags::color);
		renderBuffer.setBlendState(*blendState);
		renderBuffer.setDepthStencilState(*depthStencilState);
		renderBuffer.setRasterizerState(*rasterizerState);
		renderBuffer.setPrimitiveTopologyType(ks::PrimitiveTopologyType::trianglelist);
		renderBuffer.commit(*frameBuffer.get());

		renderEngine->erase(blendState);
		renderEngine->erase(depthStencilState);
		renderEngine->erase(rasterizerState);
	}
}