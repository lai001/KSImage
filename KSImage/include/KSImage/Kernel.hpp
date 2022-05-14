#ifndef KERNEL_HPP
#define KERNEL_HPP

#include <Foundation/Foundation.hpp>
#include "ShaderProgram.hpp"
#include "KernelUniform.hpp"
#include "KernelTexture2D.hpp"
#include "defs.hpp"

namespace ks
{
	class KSImage_API Kernel : public boost::noncopyable
	{
	private:
		ShaderProgram* program = nullptr;
		const bgfx::Memory* vertexBuffer = nullptr;
		bgfx::VertexLayout vertexBufferLayout;
		std::vector<unsigned int> _indices;
		bgfx::VertexBufferHandle m_vbh = BGFX_INVALID_HANDLE;
		bgfx::IndexBufferHandle m_ibh = BGFX_INVALID_HANDLE;
		std::unordered_map<std::string, KernelTexture2D*> kernel_texture2ds;
		std::vector<KernelUniform*> uniforms;
		int _stage = 0;
		int maxStage = 0;

	public:
		~Kernel();

		static std::shared_ptr<Kernel> create(
			const std::string& vertexShaderCodePath,
			const std::string& fragmentShaderCodePath,
			const std::vector<KernelUniform::Info>& kernel_uniform_infos) noexcept;

		static std::shared_ptr<Kernel> create(
			const std::string& shaderName,
			const std::vector<KernelUniform::Info>& kernel_uniform_infos) noexcept;

		static std::shared_ptr<Kernel> create(const std::string& fragmentShaderCode) noexcept;

		bgfx::VertexBufferHandle setVertex(const void* vertextBuffer, const unsigned int vertextBufferSize, const bgfx::VertexLayout& layout) noexcept;
		bgfx::IndexBufferHandle setIndices(const std::vector<unsigned int>& indices) noexcept;

		void bindVertex(const void* vertextBuffer, const unsigned int vertextBufferSize, const bgfx::VertexLayout& layout, const unsigned char stream) noexcept;
		void bindIndices(const std::vector<unsigned int>& indices) noexcept;

		void bindTexture2D(const std::string& name, const std::shared_ptr<ks::Image> image) noexcept;
		void bindTexture2D(const std::string& name, const ks::PixelBuffer& pixelBuffer) noexcept;
		void bindTexture2D(const std::string& name, const bgfx::TextureHandle textureHandle) noexcept;
		void bindUniform(const std::string& name, const ks::KernelUniform::Value& value) noexcept;

		const ShaderProgram* getProgram() const noexcept;
		bgfx::ProgramHandle getBgfxProgram() const noexcept;
		bgfx::VertexBufferHandle getVertexBufferHandle() const noexcept;
		bgfx::IndexBufferHandle getIndexBufferHandle() const noexcept;

		std::vector<const KernelUniform*> getUniforms() noexcept;
		std::unordered_map<std::string, const KernelTexture2D*> getKernel_texture2Ds() noexcept;
	};
}

#endif // !KERNEL_HPP