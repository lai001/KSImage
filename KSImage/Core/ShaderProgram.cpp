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

	std::shared_ptr<ShaderProgram> ShaderProgram::create(const bgfx::Memory * vertextCode, const bgfx::Memory * fragmentCode) noexcept
	{
		assert(vertextCode);
		assert(fragmentCode);
		std::shared_ptr<ShaderProgram> program = std::make_shared<ShaderProgram>();

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