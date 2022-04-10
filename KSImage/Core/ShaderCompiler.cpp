#include "ShaderCompiler.hpp"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <spdlog/spdlog.h>

namespace ks
{
	const int CMD_RESULT_BUF_SIZE = 1024 * 20;

	std::string getVariableShaderPath() noexcept
	{
		return fd::Application::getAppDir() + "/VariableShader";
	}

	std::string getShadercPath() noexcept
	{
		return getVariableShaderPath() + "/shaderc.exe";
	}

	const bgfx::Memory * ShaderCompiler::compileVertexMemory(const std::string& name, const std::string& vertex) const noexcept
	{
		std::ofstream file;
		const std::string inputFilePath = fmt::format("{}/{}.vert.tmp", getVariableShaderPath(), name);
		const std::string outputFilePath = fmt::format("{}/{}.vert", getVariableShaderPath(), name);
		file.open(inputFilePath);
		file << vertex;
		file.close();

		std::string cmd = fmt::format("{} -f {} -o {} --type vertex --platform windows -p vs_5_0 -O 3 --varyingdef VariableShader/varying.def.sc -i VariableShader", getShadercPath(), inputFilePath, outputFilePath);
		char result[1024] = { 0 };
		int status = executeCMD(cmd.c_str(), result);
		assert(status != 0);

		return ks::ShaderCompiler::readShaderCodeFromFile(outputFilePath);
	}

	const bgfx::Memory * ShaderCompiler::compileFragmentMemory(const std::string& name, const std::string& fragment) const noexcept
	{
		std::ofstream file;
		const std::string inputFilePath = fmt::format("{}/{}.frag.tmp", getVariableShaderPath(), name);
		const std::string outputFilePath = fmt::format("{}/{}.frag", getVariableShaderPath(), name);
		file.open(inputFilePath);
		file << fragment;
		file.close();

		std::string cmd = fmt::format("{} -f {} -o {} --type fragment --platform windows -p ps_5_0 -O 3 --varyingdef VariableShader/varying.def.sc -i VariableShader", getShadercPath(), inputFilePath, outputFilePath);
		char result[CMD_RESULT_BUF_SIZE] = { 0 };
		int status = executeCMD(cmd.c_str(), result);
		assert(status != 0);
		spdlog::debug(result);

		return ks::ShaderCompiler::readShaderCodeFromFile(outputFilePath);
	}

	int ShaderCompiler::executeCMD(const char *cmd, char *result) const noexcept
	{
		int iRet = -1;
		char buf_ps[CMD_RESULT_BUF_SIZE];
		char ps[CMD_RESULT_BUF_SIZE] = { 0 };
		FILE *ptr;

		strcpy(ps, cmd);
		
		if ((ptr = _popen(ps, "r")) != NULL)
		{
			while (fgets(buf_ps, sizeof(buf_ps), ptr) != NULL)
			{
				strcat(result, buf_ps);
				if (strlen(result) > CMD_RESULT_BUF_SIZE)
				{
					break;
				}
			}
			_pclose(ptr);
			ptr = NULL;
			iRet = 1;  // success
		}
		else
		{
			spdlog::error("popen {} error\n", ps);
			iRet = 0; // fail
		}

		return iRet;
	}

	const bgfx::Memory * ShaderCompiler::readShaderCodeFromFile(const std::string & path) noexcept
	{
		std::ifstream file(path, std::ios::binary | std::ios::ate);
		std::streamsize size = file.tellg();
		file.seekg(0, std::ios::beg);
		assert(size > 0);
		std::vector<char> buffer(size);
		if (file.read(buffer.data(), size))
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
		std::ofstream file;
		const std::string inputFilePath = fmt::format("{}/{}.vert.tmp", getVariableShaderPath(), name);
		const std::string outputFilePath = fmt::format("{}/{}.vert", getVariableShaderPath(), name);
		file.open(inputFilePath);
		file << vertex;
		file.close();

		std::string cmd = fmt::format("{} -f {} -o {} --type vertex --platform windows -p vs_5_0 -O 3 --varyingdef VariableShader/varying.def.sc -i VariableShader", getShadercPath(), inputFilePath, outputFilePath);
		char result[1024] = { 0 };
		int status = executeCMD(cmd.c_str(), result);
		assert(status != 0);

		return outputFilePath;
	}

	std::string ShaderCompiler::compileFragmentFile(const std::string & name, const std::string & fragment) const noexcept
	{
		std::ofstream file;
		const std::string inputFilePath = fmt::format("{}/{}.frag.tmp", getVariableShaderPath(), name);
		const std::string outputFilePath = fmt::format("{}/{}.frag", getVariableShaderPath(), name);
		file.open(inputFilePath);
		file << fragment;
		file.close();

		std::string cmd = fmt::format("{} -f {} -o {} --type fragment --platform windows -p ps_5_0 -O 3 --varyingdef VariableShader/varying.def.sc -i VariableShader", getShadercPath(), inputFilePath, outputFilePath);
		char result[CMD_RESULT_BUF_SIZE] = { 0 };
		int status = executeCMD(cmd.c_str(), result);
		assert(status != 0);
		spdlog::debug(result);

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