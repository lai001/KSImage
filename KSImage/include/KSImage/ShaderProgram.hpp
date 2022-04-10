#ifndef SHADERPROGRAM_HPP
#define SHADERPROGRAM_HPP

#include <Foundation/Foundation.hpp>
#include <bgfx/bgfx.h>

namespace ks
{
	class ShaderProgram : public boost::noncopyable
	{
	private:
		const bgfx::Memory* _vertextCode = nullptr;
		const bgfx::Memory* _fragmentCode = nullptr;

		bgfx::ProgramHandle m_program = BGFX_INVALID_HANDLE;
		bgfx::ShaderHandle vertexShader = BGFX_INVALID_HANDLE;
		bgfx::ShaderHandle fragmentShader = BGFX_INVALID_HANDLE;
		
	public:
		~ShaderProgram();
		static std::shared_ptr<ShaderProgram> create(const bgfx::Memory* vertextCode, const bgfx::Memory* fragmentCode) noexcept;

		bgfx::ProgramHandle getProgramHandle() const noexcept;
	};
}

#endif // !SHADERPROGRAM_HPP