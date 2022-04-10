#ifndef KERNEL_HPP
#define KERNEL_HPP

#include <Foundation/Foundation.hpp>
#include "ShaderProgram.hpp"
#include "KernelUniform.hpp"
#include "KernelTexture2D.hpp"

namespace ks
{
	class Kernel : public boost::noncopyable
	{
	private:
		std::shared_ptr<ShaderProgram> program;
		const bgfx::Memory* vertexBuffer = nullptr;
		bgfx::VertexLayout vertexBufferLayout;
		std::vector<unsigned int> _indices;
		bgfx::VertexBufferHandle m_vbh = BGFX_INVALID_HANDLE;
		bgfx::IndexBufferHandle m_ibh = BGFX_INVALID_HANDLE;
		std::unordered_map<std::string, std::shared_ptr<KernelTexture2D>> kernel_texture2ds;
		std::vector<std::shared_ptr<KernelUniform>> uniforms;
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
		bgfx::IndexBufferHandle setIndices(std::vector<unsigned int>& indices) noexcept;

		void bindVertex(const void* vertextBuffer, const unsigned int vertextBufferSize, const bgfx::VertexLayout& layout, const unsigned char stream) noexcept;
		void bindIndices(std::vector<unsigned int>& indices) noexcept;

		void bindTexture2D(const std::string& name, const std::shared_ptr<ks::Image> image);
		void bindTexture2D(const std::string& name, const std::shared_ptr<ks::PixelBuffer> pixelBuffer);
		void bindUniform(const std::string& name, const ks::KernelUniform::Value& value);

		std::shared_ptr<ShaderProgram> getProgram() const noexcept;
		bgfx::ProgramHandle getBgfxProgram() const noexcept;
		bgfx::VertexBufferHandle getVertexBufferHandle() const noexcept;
		bgfx::IndexBufferHandle getIndexBufferHandle() const noexcept;

		std::vector<std::shared_ptr<KernelUniform>> getUniforms() noexcept;
		std::unordered_map<std::string, std::shared_ptr<KernelTexture2D>> getKernel_texture2Ds() noexcept;
	};
}

#endif // !KERNEL_HPP