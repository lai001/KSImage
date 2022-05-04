#include "ShaderProgram.hpp"
#include <assert.h>

namespace ks
{

	ShaderProgram::~ShaderProgram()
	{
		if (bgfx::isValid(m_program))
		{
			bgfx::destroy(m_program);
		}
	}

	ShaderProgram* ShaderProgram::create(const bgfx::Memory * vertextCode, const bgfx::Memory * fragmentCode) noexcept
	{
		assert(vertextCode);
		assert(fragmentCode);
		ShaderProgram* program = new ShaderProgram();

		program->_vertextCode = vertextCode;
		program->_fragmentCode = fragmentCode;
		program->vertexShader = bgfx::createShader(program->_vertextCode);
		program->fragmentShader = bgfx::createShader(program->_fragmentCode);
		program->m_program = bgfx::createProgram(program->vertexShader, program->fragmentShader, true);

		assert(bgfx::isValid(program->vertexShader));
		assert(bgfx::isValid(program->fragmentShader));
		return program;
	}

	bgfx::ProgramHandle ShaderProgram::getProgramHandle() const noexcept
	{
		return m_program;
	}
}