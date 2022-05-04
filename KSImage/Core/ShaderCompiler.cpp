#include "ShaderCompiler.hpp"
#include <stdio.h>
#include <array>
#include <iostream>
#include <fstream>
#include <spdlog/spdlog.h>

namespace ks
{

	std::string getVariableShaderPath() noexcept
	{
		return ks::Application::getAppDir() + "/VariableShader";
	}

	std::string getShadercPath() noexcept
	{
		return getVariableShaderPath() + "/shaderc.exe";
	}

	const bgfx::Memory * ShaderCompiler::compileVertexMemory(const std::string& name, const std::string& vertex) const noexcept
	{
		const std::string inputFilePath = fmt::format("{}/{}.vert.tmp", getVariableShaderPath(), name);
		const std::string outputFilePath = fmt::format("{}/{}.vert", getVariableShaderPath(), name);
		bool status = ks::File::write(vertex, inputFilePath);
		assert(status);

		std::string cmd = fmt::format("{} -f {} -o {} --type vertex --platform windows -p vs_5_0 -O 3 --varyingdef VariableShader/varying.def.sc -i VariableShader", getShadercPath(), inputFilePath, outputFilePath);
		std::string result = ks::os::runCommand(cmd, nullptr);

		return ks::ShaderCompiler::readShaderCodeFromFile(outputFilePath);
	}

	const bgfx::Memory * ShaderCompiler::compileFragmentMemory(const std::string& name, const std::string& fragment) const noexcept
	{
		const std::string inputFilePath = fmt::format("{}/{}.frag.tmp", getVariableShaderPath(), name);
		const std::string outputFilePath = fmt::format("{}/{}.frag", getVariableShaderPath(), name);

		bool status = ks::File::write(fragment, inputFilePath);
		assert(status);

		std::string cmd = fmt::format("{} -f {} -o {} --type fragment --platform windows -p ps_5_0 -O 3 --varyingdef VariableShader/varying.def.sc -i VariableShader", getShadercPath(), inputFilePath, outputFilePath);
		std::string result = ks::os::runCommand(cmd, nullptr);

		return ks::ShaderCompiler::readShaderCodeFromFile(outputFilePath);
	}

	const bgfx::Memory * ShaderCompiler::readShaderCodeFromFile(const std::string & path) noexcept
	{
		bool status;
		std::vector<char> buffer = ks::File::readBinData(path, &status);
		if (status)
		{
			const bgfx::Memory* memory = bgfx::copy(buffer.data(), buffer.size());
			return memory;
		}
		else
		{
			return nullptr;
		}
	}

	ks::ShaderCompiler::CompileResult ks::ShaderCompiler::compileMemory(const std::string & name, const std::string & vertex, const std::string & fragment) const noexcept
	{
		CompileResult result;
		result.vertexCode = compileVertexMemory(name, vertex);
		result.fragmentCode = compileFragmentMemory(name, fragment);
		return result;
	}

	std::string ShaderCompiler::compileVertexFile(const std::string & name, const std::string & vertex) const noexcept
	{
		const std::string inputFilePath = fmt::format("{}/{}.vert.tmp", getVariableShaderPath(), name);
		const std::string outputFilePath = fmt::format("{}/{}.vert", getVariableShaderPath(), name);

		bool status = ks::File::write(vertex, inputFilePath);
		assert(status);

		std::string cmd = fmt::format("{} -f {} -o {} --type vertex --platform windows -p vs_5_0 -O 3 --varyingdef VariableShader/varying.def.sc -i VariableShader", getShadercPath(), inputFilePath, outputFilePath);
		std::string result = ks::os::runCommand(cmd, nullptr);

		return outputFilePath;
	}

	std::string ShaderCompiler::compileFragmentFile(const std::string & name, const std::string & fragment) const noexcept
	{
		const std::string inputFilePath = fmt::format("{}/{}.frag.tmp", getVariableShaderPath(), name);
		const std::string outputFilePath = fmt::format("{}/{}.frag", getVariableShaderPath(), name);

		bool status = ks::File::write(fragment, inputFilePath);
		assert(status);

		std::string cmd = fmt::format("{} -f {} -o {} --type fragment --platform windows -p ps_5_0 -O 3 --varyingdef VariableShader/varying.def.sc -i VariableShader", getShadercPath(), inputFilePath, outputFilePath);
		std::string result = ks::os::runCommand(cmd, nullptr);

		return outputFilePath;
	}

	ks::ShaderPaire ShaderCompiler::compileFile(const std::string & name, const std::string & vertex, const std::string & fragment) const noexcept
	{
		ks::ShaderPaire paire;
		paire.vertexPath = compileVertexFile(name, vertex);
		paire.fragmentPath = compileFragmentFile(name, fragment);
		return paire;
	}
}