#ifndef SHADERCOMPILERHPP
#define SHADERCOMPILERHPP

#include <string>
#include <Foundation/Foundation.hpp>
#include <bgfx/bgfx.h>
#include "Util.hpp"
#include "defs.hpp"

namespace ks
{
	class KSImage_API ShaderCompiler
	{
	public:
		struct CompileResult
		{
			const bgfx::Memory* vertexCode = nullptr;
			const bgfx::Memory* fragmentCode = nullptr;
		};
		static const bgfx::Memory * readShaderCodeFromFile(const std::string & path) noexcept;

		const bgfx::Memory* compileVertexMemory(const std::string& name, const std::string& vertex) const noexcept;
		const bgfx::Memory* compileFragmentMemory(const std::string& name, const std::string& fragment) const noexcept;
		CompileResult compileMemory(const std::string& name, const std::string& vertex, const std::string& fragment) const noexcept;

		std::string compileVertexFile(const std::string& name, const std::string& vertex) const noexcept;
		std::string compileFragmentFile(const std::string& name, const std::string& fragment) const noexcept;
		ks::ShaderPaire compileFile(const std::string& name, const std::string& vertex, const std::string& fragment) const noexcept;

	};
}

#endif // !SHADERCOMPILERHPP