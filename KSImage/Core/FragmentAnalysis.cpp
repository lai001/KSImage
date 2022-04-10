#include "FragmentAnalysis.hpp"

#include <iostream>
#include <iterator>
#include <unordered_map>
#include <regex>
#include <spdlog/spdlog.h>

namespace ks
{

	const std::string AdditionalCode = R"(
float ks_remap(float v, float fromLower, float fromUpper, float toLower, float toUpper)
{
    return (v - fromLower) / (fromUpper - fromLower) * (toUpper - toLower) + toLower;
}
float ks_isInRange(float x, float left, float right)
{
	return step(step(x, left) + step(right, x), 0.0);
}

float ks_isBorder(vec2 uv)
{
	return ks_isInRange(uv.x, 0.0, 1.0) * ks_isInRange(uv.y, 0.0, 1.0);
}

vec4 ks_texture2DBorder(BgfxSampler2D t, vec2 uv)
{
	return ks_isBorder(uv) * texture2D(t, uv);
})";

	const std::string TestFragmentShaderCode = R"(
kernel vec4 mosaicKernel(BgfxSampler2D image)
{
	return vec4(1.0, 1.0, 1.0, 1.0);
})";

	FragmentAnalysis::AnalysisResult FragmentAnalysis::analysis(const std::string & code) const noexcept
	{
		AnalysisResult result;

		std::string IdentifierStr;

		bool isFindKernel = false;
		bool isFindReturnType = false;
		bool isFindEntryFunc = false;

		std::unique_ptr<ks::KernelUniform::Info> uniformInfo;

		for (const char& c : code)
		{
			if (std::isspace(c) || c == ';' || c == '(' || c == ')' || c == '{' || c == '}' || c == ',')
			{
				if (IdentifierStr == "")
				{

				}
				else
				{
					//spdlog::debug(IdentifierStr);
					if (IdentifierStr == "kernel")
					{
						isFindKernel = true;
					}
					else if (IdentifierStr == "vec4" && isFindKernel)
					{
						isFindReturnType = true;
					}
					else if (isFindKernel && isFindReturnType && isFindEntryFunc == false)
					{
						isFindEntryFunc = true;
						result.entryFuncName = IdentifierStr;
					}

					if (isFindEntryFunc && isFindKernel && isFindReturnType)
					{
						if (IdentifierStr == "BgfxSampler2D")
						{
							uniformInfo = std::make_unique<ks::KernelUniform::Info>("", ks::KernelUniform::ValueType::texture2d);
							result.texture2DCount += 1;
						}
						else if (IdentifierStr == "float")
						{
							uniformInfo = std::make_unique<ks::KernelUniform::Info>("", ks::KernelUniform::ValueType::f32);
						}
						else if (IdentifierStr == "vec2")
						{
							uniformInfo = std::make_unique<ks::KernelUniform::Info>("", ks::KernelUniform::ValueType::vec2);
						}
						else if (IdentifierStr == "vec3")
						{
							uniformInfo = std::make_unique<ks::KernelUniform::Info>("", ks::KernelUniform::ValueType::vec3);
						}
						else if (IdentifierStr == "vec4")
						{
							uniformInfo = std::make_unique<ks::KernelUniform::Info>("", ks::KernelUniform::ValueType::vec4);
						}
						else if (IdentifierStr == "mat3")
						{
							uniformInfo = std::make_unique<ks::KernelUniform::Info>("", ks::KernelUniform::ValueType::mat3);
						}
						else if (IdentifierStr == "mat4")
						{
							uniformInfo = std::make_unique<ks::KernelUniform::Info>("", ks::KernelUniform::ValueType::mat4);
						}
						else if (uniformInfo)
						{
							uniformInfo->name = "__u_" + IdentifierStr;
							result.uniformInfos.push_back(*uniformInfo);
							uniformInfo = nullptr;
						}
					}
				}
				IdentifierStr = "";
				if (c == ')' && isFindKernel && isFindReturnType && isFindEntryFunc)
				{
					break;
				}
			}
			else
			{
				IdentifierStr = IdentifierStr + c;
			}
		}

		assert(isFindKernel && isFindReturnType && isFindEntryFunc);

		result.fragmentShaderCode = generateFragmentShaderCode(result, code);

		{ // internal uniform
			result.uniformInfos.push_back(ks::KernelUniform::Info("__u_workingSpacePixelSize", ks::KernelUniform::ValueType::vec2));
			for (int number = 0; number < result.texture2DCount; number++)
			{
				result.uniformInfos.push_back(ks::KernelUniform::Info(fmt::format("__u_sampler{}Space", number), ks::KernelUniform::ValueType::vec4));
			}
		}

		return result;
	}

	FragmentAnalysis::AnalysisResult FragmentAnalysis::analysisTest() const noexcept
	{
		return analysis(TestFragmentShaderCode);
	}

	std::string FragmentAnalysis::generateFragmentShaderCode(const AnalysisResult & result, const std::string& originSourceCode) const noexcept
	{
		std::string templateCode = R"(
$input v_texcoord0
#include "bgfx_shader.sh"
$SAMPLER2DCode
$uniformCode
$internalUniformCode
vec2 __v_texcoord0;
$helperCode
$kernelCode
void main()
{
	__v_texcoord0 = v_texcoord0;
	$mainCode
})";
		
		std::string kernelCode = "";
		std::string mainCode = "";
		std::string SAMPLER2DCode = "";
		std::string uniformCode = "";
		std::string helperCode = "";
		std::string internalUniformCode = "";

		{
			internalUniformCode = "uniform vec4 __u_workingSpacePixelSize;\n";
			for (int number = 0; number < result.texture2DCount; number++)
			{
				internalUniformCode = internalUniformCode + fmt::format("uniform vec4 __u_sampler{}Space;\n", number);
			}
		}

		{
			kernelCode = originSourceCode;
			bool replaceStatus = replace(kernelCode, "kernel ", "");
			assert(replaceStatus);
		}

		{
			helperCode = AdditionalCode + helperCode;
			helperCode = helperCode + R"(
vec2 ks_destCoord()
{
	return __v_texcoord0;
}

vec2 ks_workingSpacePixelSize()
{
	return __u_workingSpacePixelSize.xy;
})";

			for (int number = 0; number < result.texture2DCount; number++)
			{
				helperCode = helperCode + fmt::format(R"(
vec4 ks_sampler{}Space()
{}
	return __u_sampler{}Space;
{})", number, "{", number, "}");

				helperCode = helperCode + fmt::format(R"(
vec2 ks_sampler{}Transform()
{}
	vec2 t;
	t.x = ks_remap(__v_texcoord0.x, __u_sampler{}Space.x, __u_sampler{}Space.z, 0.0, 1.0);
	t.y = ks_remap(__v_texcoord0.y, __u_sampler{}Space.y, __u_sampler{}Space.w, 0.0, 1.0);
	return t;
{})", number, "{", number, number, number, number, "}");
			}

		}

		std::vector<std::string> parameters;
		int stage = 0;
		for (int i = 0; i < result.uniformInfos.size(); i++)
		{
			const ks::KernelUniform::Info uniform = result.uniformInfos[i];
			if (uniform.type == ks::KernelUniform::ValueType::texture2d)
			{
				std::string name = fmt::format("{}", uniform.name);
				std::string line = fmt::format("SAMPLER2D({}, {});\n", name, stage);
				SAMPLER2DCode += line;
				stage += 1;

				parameters.push_back(name);
			}
			else
			{
				std::string name = fmt::format("{}", uniform.name);
				std::unordered_map<ks::KernelUniform::ValueType, std::string> dic;
				dic[ks::KernelUniform::ValueType::f32] = "vec4";
				dic[ks::KernelUniform::ValueType::vec2] = "vec4";
				dic[ks::KernelUniform::ValueType::vec3] = "vec4";
				dic[ks::KernelUniform::ValueType::vec4] = "vec4";
				dic[ks::KernelUniform::ValueType::mat3] = "mat3";
				dic[ks::KernelUniform::ValueType::mat4] = "mat4";
				std::string typeStr = dic[uniform.type];

				std::string line0 = fmt::format("uniform {} {};\n", typeStr, name);
				std::string line1 = "";
				std::string line = line0 + line1;
				uniformCode += line;

				dic[ks::KernelUniform::ValueType::f32] = ".x";
				dic[ks::KernelUniform::ValueType::vec2] = ".xy";
				dic[ks::KernelUniform::ValueType::vec3] = ".xyz";
				dic[ks::KernelUniform::ValueType::vec4] = "";
				dic[ks::KernelUniform::ValueType::mat3] = "";
				dic[ks::KernelUniform::ValueType::mat4] = "";
				std::string suffix = dic[uniform.type];

				parameters.push_back(fmt::format("{}{}", name, suffix));
			}
		}

		std::string parametersLine = "";
		for (int i = 0; i < parameters.size(); i++)
		{
			bool isLast = (parameters.size() - 1) == i;
			parametersLine = parametersLine + parameters[i] + std::string(isLast ? "" : ", ");
		}
		mainCode = fmt::format("gl_FragColor = {}({});", result.entryFuncName, parametersLine);

		replace(templateCode, "$SAMPLER2DCode", SAMPLER2DCode);
		replace(templateCode, "$uniformCode", uniformCode);
		replace(templateCode, "$helperCode", helperCode);
		replace(templateCode, "$kernelCode", kernelCode);
		replace(templateCode, "$mainCode", mainCode);
		replace(templateCode, "$internalUniformCode", internalUniformCode);

		return templateCode;
	}

	bool FragmentAnalysis::replace(std::string & str, const std::string & from, const std::string & to) const noexcept
	{
		size_t start_pos = str.find(from);
		if (start_pos == std::string::npos)
			return false;
		str.replace(start_pos, from.length(), to);
		return true;
	}

}