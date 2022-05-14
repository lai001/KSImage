#include "Kernel.hpp"
#include <fstream>
#include <string>
#include <sstream>
#include "Util.hpp"
#include "FragmentAnalysis.hpp"
#include "ShaderCompiler.hpp"

namespace ks
{
	const std::string DefaultVertexShaderCode = R"(

$input a_position, a_texcoord0
$output v_texcoord0

void main()
{
	gl_Position = vec4(a_position, 1.0);
	v_texcoord0 = a_texcoord0;
}

)";

	Kernel::~Kernel()
	{
		if (bgfx::isValid(m_vbh))
		{
			bgfx::destroy(m_vbh);
		}
		if (bgfx::isValid(m_ibh))
		{
			bgfx::destroy(m_ibh);
		}
		assert(program);
		delete program;
		for (ks::KernelUniform * item : uniforms)
		{
			assert(item);
			delete item;
		}
		for (const auto& args : kernel_texture2ds)
		{
			assert(args.second);
			delete args.second;
		}
	}

	std::shared_ptr<Kernel> Kernel::create(
		const std::string& vertexShaderCodePath,
		const std::string& fragmentShaderCodePath,
		const std::vector<KernelUniform::Info>& kernel_uniform_infos) noexcept
	{
		const bgfx::Memory * codeMemory = ks::ShaderCompiler::readShaderCodeFromFile(fragmentShaderCodePath);
		const bgfx::Memory * vertextCodeMemory = ks::ShaderCompiler::readShaderCodeFromFile(vertexShaderCodePath);
		assert(codeMemory);
		assert(vertextCodeMemory);
		std::shared_ptr<Kernel> kernel = std::make_shared<Kernel>();
		kernel->program = ShaderProgram::create(vertextCodeMemory, codeMemory);
		for (auto Info : kernel_uniform_infos)
		{
			kernel->uniforms.push_back(new ks::KernelUniform(Info));
			if (Info.type == ks::KernelUniform::ValueType::texture2d)
			{
				kernel->kernel_texture2ds.emplace(Info.name, ks::KernelTexture2D::createPlaceHolder());
				kernel->maxStage += 1;
			}
		}
		return kernel;
	}

	std::shared_ptr<Kernel> Kernel::create(
		const std::string& shaderName,
		const std::vector<KernelUniform::Info>& kernel_uniform_infos) noexcept
	{
		ks::ShaderPaire paire = ks::getShaderPaire(shaderName);
		return Kernel::create(paire.vertexPath, paire.fragmentPath, kernel_uniform_infos);
	}

	std::shared_ptr<Kernel> Kernel::create(const std::string & fragmentShaderCode) noexcept
	{
		ks::FragmentAnalysis analysis;
		ks::FragmentAnalysis::AnalysisResult result = analysis.analysis(fragmentShaderCode);
		ks::ShaderCompiler compiler;
		ks::ShaderPaire paire = compiler.compileFile("tmp", DefaultVertexShaderCode, result.fragmentShaderCode);
		return Kernel::create(paire.vertexPath, paire.fragmentPath, result.uniformInfos);
	}

	bgfx::VertexBufferHandle Kernel::setVertex(const void * vertextBuffer, const unsigned int vertextBufferSize, const bgfx::VertexLayout & layout) noexcept
	{
		if (bgfx::isValid(m_vbh))
		{
			bgfx::destroy(m_vbh);
		}
		vertexBuffer = bgfx::copy(vertextBuffer, vertextBufferSize);
		vertexBufferLayout = layout;
		m_vbh = bgfx::createVertexBuffer(vertexBuffer, layout, BGFX_BUFFER_NONE);
		assert(bgfx::isValid(m_vbh));
		return m_vbh;
	}

	bgfx::IndexBufferHandle Kernel::setIndices(const std::vector<unsigned int>& indices) noexcept
	{
		if (bgfx::isValid(m_ibh))
		{
			bgfx::destroy(m_ibh);
		}
		_indices = indices;
		m_ibh = bgfx::createIndexBuffer(bgfx::copy(_indices.data(), sizeof(unsigned int) * indices.size()), BGFX_BUFFER_INDEX32);
		assert(bgfx::isValid(m_ibh));
		return m_ibh;
	}

	void Kernel::bindVertex(const void * vertextBuffer, const unsigned int vertextBufferSize, const bgfx::VertexLayout & layout, const unsigned char stream) noexcept
	{
		bgfx::VertexBufferHandle handle = setVertex(vertextBuffer, vertextBufferSize, layout);
		bgfx::setVertexBuffer(stream, handle);
	}

	void Kernel::bindIndices(const std::vector<unsigned int>& indices) noexcept
	{
		bgfx::IndexBufferHandle handle = setIndices(indices);
		bgfx::setIndexBuffer(handle);
	}

	void Kernel::bindTexture2D(const std::string & name, const std::shared_ptr<ks::Image> image) noexcept
	{
		bool isFindTexture2D = false;

		for (auto& uniform : uniforms)
		{
			if (uniform->getName() == name)
			{
				kernel_texture2ds[name]->copyImage(image);
				kernel_texture2ds[name]->bind(_stage, uniform->getHandle());
				_stage += 1;
				_stage %= maxStage;
				isFindTexture2D = true;
			}
		}
		assert(isFindTexture2D && "Don't find Texture2D");
	}

	void Kernel::bindTexture2D(const std::string & name, const ks::PixelBuffer& pixelBuffer) noexcept
	{
		bool isFindTexture2D = false;

		for (auto& uniform : uniforms)
		{
			if (uniform->getName() == name)
			{
				kernel_texture2ds[name]->copyPixelBuffer(pixelBuffer);
				kernel_texture2ds[name]->bind(_stage, uniform->getHandle());
				_stage += 1;
				_stage %= maxStage;
				isFindTexture2D = true;
			}
		}
		assert(isFindTexture2D && "Don't find Texture2D");
	}

	void Kernel::bindTexture2D(const std::string & name, const bgfx::TextureHandle textureHandle) noexcept
	{
		bool isFindTexture2D = false;

		for (auto& uniform : uniforms)
		{
			if (uniform->getName() == name)
			{
				kernel_texture2ds[name]->copyTextureHandle(textureHandle);
				kernel_texture2ds[name]->bind(_stage, uniform->getHandle());
				_stage += 1;
				_stage %= maxStage;
				isFindTexture2D = true;
			}
		}
		assert(isFindTexture2D && "Don't find Texture2D");
	}

	void Kernel::bindUniform(const std::string & name, const ks::KernelUniform::Value & value) noexcept
	{
		bool isFindUniform = false;
		for (auto& uniform : uniforms)
		{
			if (uniform->getName() == name)
			{
				assert(uniform->getType() != ks::KernelUniform::ValueType::texture2d);
				uniform->setValue(value);
				isFindUniform = true;
			}
		}
		assert(isFindUniform && "Don't find unifom");
	}

	const ShaderProgram* Kernel::getProgram() const noexcept
	{
		return program;
	}

	bgfx::ProgramHandle Kernel::getBgfxProgram() const noexcept
	{
		return program->getProgramHandle();
	}

	bgfx::VertexBufferHandle Kernel::getVertexBufferHandle() const noexcept
	{
		return m_vbh;
	}

	bgfx::IndexBufferHandle Kernel::getIndexBufferHandle() const noexcept
	{
		return m_ibh;
	}

	std::vector<const KernelUniform*> Kernel::getUniforms() noexcept
	{
		std::vector<const KernelUniform*> _uniforms;
		for (ks::KernelUniform* uniform : uniforms)
		{
			assert(uniform);
			_uniforms.push_back(uniform);
		}
		return _uniforms;
	}

	std::unordered_map<std::string, const KernelTexture2D*> Kernel::getKernel_texture2Ds() noexcept
	{
		std::unordered_map<std::string, const KernelTexture2D*> dic;
		for (const auto& args : kernel_texture2ds)
		{
			dic[args.first] = args.second;
		}
		return dic;
	}
}