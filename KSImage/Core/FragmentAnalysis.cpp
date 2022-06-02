#include "FragmentAnalysis.hpp"

#include <iostream>
#include <iterator>
#include <unordered_map>
#include <regex>
#include <spdlog/spdlog.h>

namespace ks
{

	std::string getAdditionalCode()
	{
		const static std::string additionalCode = R"(
float ks_remap(float v, float fromLower, float fromUpper, float toLower, float toUpper)
{
	return (v - fromLower) / (fromUpper - fromLower) * (toUpper - toLower) + toLower;
}

float ks_isInRange(float x, float left, float right)
{
	return step(step(x, left) + step(right, x), 0.0);
}

float ks_isBorder(float2 uv)
{
	return ks_isInRange(uv.x, 0.0, 1.0) * ks_isInRange(uv.y, 0.0, 1.0);
}

float4 ks_texture2DBorder(Texture2D t, SamplerState _sampler, float2 uv)
{
	return ks_isBorder(uv) * t.Sample(_sampler, uv);
}

float2 ks_getWorkingSpacePixelSize()
{
	return ks_workingSpacePixelSize;
}
)";
		return additionalCode;
	}

	FragmentAnalysis::AnalysisResult FragmentAnalysis::analysis(const std::string & code) const noexcept
	{
		AnalysisResult result;

		std::string IdentifierStr;

		bool isFindKernel = false;
		bool isFindReturnType = false;
		bool isFindEntryFunc = false;

		std::unique_ptr<KernelUniform::Info> uniformInfo;

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
					else if (IdentifierStr == "float4" && isFindKernel)
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
						if (IdentifierStr == "Texture2D")
						{
							uniformInfo = std::make_unique<KernelUniform::Info>("", KernelUniform::ValueType::texture2d);
							result.texture2DCount += 1;
						}
						else if (IdentifierStr == "float")
						{
							uniformInfo = std::make_unique<KernelUniform::Info>("", KernelUniform::ValueType::f32);
						}
						else if (IdentifierStr == "float2")
						{
							uniformInfo = std::make_unique<KernelUniform::Info>("", KernelUniform::ValueType::vec2);
						}
						else if (IdentifierStr == "float3")
						{
							uniformInfo = std::make_unique<KernelUniform::Info>("", KernelUniform::ValueType::vec3);
						}
						else if (IdentifierStr == "float4")
						{
							uniformInfo = std::make_unique<KernelUniform::Info>("", KernelUniform::ValueType::vec4);
						}
						else if (IdentifierStr == "float3x3")
						{
							uniformInfo = std::make_unique<KernelUniform::Info>("", KernelUniform::ValueType::mat3);
						}
						else if (IdentifierStr == "float4x4")
						{
							uniformInfo = std::make_unique<KernelUniform::Info>("", KernelUniform::ValueType::mat4);
						}
						else if (uniformInfo)
						{
							uniformInfo->name = FragmentAnalysis::ShareInfo::internalPrefix() + IdentifierStr;
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

		
		{
			result.internalUniformInfos.push_back(KernelUniform::Info(ShareInfo().workingSpacePixelSizeUniformName(), KernelUniform::ValueType::vec2));
			for (int number = 0; number < result.texture2DCount; number++)
			{
				result.internalUniformInfos.push_back(KernelUniform::Info(ShareInfo().uniformSamplerSpaceName(number), KernelUniform::ValueType::vec4));
			}
		}

		result.fragmentShaderCode = generateFragmentShaderCode(result, code);

		return result;
	}

	std::string FragmentAnalysis::generateFragmentShaderCode(const AnalysisResult & result, const std::string& originSourceCode) const noexcept
	{
		std::string templateCode = R"(
struct PS_INPUT
{
	float4 position: SV_POSITION;
	float2 texcoord: TEXCOORD;
};

$texture2Ds
$texture2DSamplers
$constantBuffer
$helper
$kernel
float4 main(PS_INPUT input) : SV_Target
{
	$mainCode
}
)";
			
		std::vector<KernelUniform::Info> uniformInfos;
		uniformInfos.insert(uniformInfos.end(), result.uniformInfos.begin(), result.uniformInfos.end());
		uniformInfos.insert(uniformInfos.end(), result.internalUniformInfos.begin(), result.internalUniformInfos.end());

		const std::string constantBufferCode = getConstantBufferCode(uniformInfos);
		const std::string texture2DBufferCode = getTexture2DBufferCode(result.texture2DCount);
		const std::string texture2DSamplersCode = getTexture2DSamplersCode(result.texture2DCount);
		const std::string helperCode = getHelperCode(uniformInfos, result.texture2DCount);
		const std::string kernelCode = stringop::replaceAllOccurrences(originSourceCode, "kernel ", "");

		templateCode = stringop::replaceAllOccurrences(templateCode, "$texture2Ds", texture2DBufferCode);
		templateCode = stringop::replaceAllOccurrences(templateCode, "$constantBuffer", constantBufferCode);
		templateCode = stringop::replaceAllOccurrences(templateCode, "$texture2DSamplers", texture2DSamplersCode);
		templateCode = stringop::replaceAllOccurrences(templateCode, "$helper", helperCode);
		templateCode = stringop::replaceAllOccurrences(templateCode, "$kernel", kernelCode);

		{
			std::vector<std::string> parameters;
			std::string parametersLine;

			unsigned int textureIndex = 0;

			for (int i = 0; i < result.uniformInfos.size(); i++)
			{
				const KernelUniform::Info uniform = result.uniformInfos[i];
				if (uniform.type == KernelUniform::ValueType::texture2d)
				{
					parameters.push_back(ShareInfo::texture2DName(textureIndex));
					textureIndex += 1;
				}
				else
				{
					std::unordered_map<KernelUniform::ValueType, std::string> dic;
					dic[KernelUniform::ValueType::f32] = "float";
					dic[KernelUniform::ValueType::vec2] = "float2";
					dic[KernelUniform::ValueType::vec3] = "float3";
					dic[KernelUniform::ValueType::vec4] = "float4";
					dic[KernelUniform::ValueType::mat3] = "float3x3";
					dic[KernelUniform::ValueType::mat4] = "float4x4";
					assert(dic.find(uniform.type) != dic.end());

					parameters.push_back(fmt::format("{}", uniform.name));
				}
			}
			parameters.push_back("input");

			for (int i = 0; i < parameters.size(); i++)
			{
				bool isLast = (parameters.size() - 1) == i;
				parametersLine = parametersLine + parameters[i] + std::string(isLast ? "" : ", ");
			}

			const std::string mainCode = fmt::format("float4 color = {}({});\n	return color;", result.entryFuncName, parametersLine);
			templateCode = stringop::replaceAllOccurrences(templateCode, "$mainCode", mainCode);
		}

		return templateCode;
	}

	std::string FragmentAnalysis::getConstantBufferCode(const std::vector<KernelUniform::Info>& uniformInfos) const
	{
		std::string templateCode = R"(
cbuffer Uniforms : register(b0)
{
$members
}
)";
		struct largeThan
		{
			inline bool operator() (const KernelUniform::Info& struct1, const KernelUniform::Info& struct2)
			{
				std::unordered_map<KernelUniform::ValueType, unsigned int> indexDic;
				indexDic[KernelUniform::ValueType::f32] = 0;
				indexDic[KernelUniform::ValueType::vec2] = 1;
				indexDic[KernelUniform::ValueType::vec3] = 2;
				indexDic[KernelUniform::ValueType::vec4] = 3;
				indexDic[KernelUniform::ValueType::mat3] = 4;
				indexDic[KernelUniform::ValueType::mat4] = 5;
				indexDic[KernelUniform::ValueType::texture2d] = 6;
				assert(indexDic.find(struct1.type) != indexDic.end());
				assert(indexDic.find(struct2.type) != indexDic.end());

				return indexDic.at(struct1.type) > indexDic.at(struct2.type);
			}
		};
		std::vector<KernelUniform::Info> sortedUniformInfos = uniformInfos;
		std::sort(sortedUniformInfos.begin(), sortedUniformInfos.end(), largeThan());

		std::string members;
		std::unordered_map<KernelUniform::ValueType, std::string> dic;
		dic[KernelUniform::ValueType::f32] = "float";
		dic[KernelUniform::ValueType::vec2] = "float2";
		dic[KernelUniform::ValueType::vec3] = "float3";
		dic[KernelUniform::ValueType::vec4] = "float4";
		dic[KernelUniform::ValueType::mat3] = "float3x3";
		dic[KernelUniform::ValueType::mat4] = "float4x4";

		for (size_t i = 0; i < sortedUniformInfos.size(); i++)
		{
			const KernelUniform::Info uniform = sortedUniformInfos[i];
			if (uniform.type == KernelUniform::ValueType::texture2d)
			{
				continue;
			}
			std::string member;
			assert(dic.find(uniform.type) != dic.end());
			member = fmt::format("	{} {};\n", dic.at(uniform.type), uniform.name);
			members = members + member;
		}

		templateCode = stringop::replaceAllOccurrences(templateCode, "$members", members);

		return templateCode;
	}

	std::string FragmentAnalysis::getTexture2DBufferCode(const std::vector<std::string>& names) const
	{
		std::string templateCode;
		for (size_t i = 0; i < names.size(); i++)
		{
			std::string name = names[i];
			name = fmt::format("Texture2D {} : register(t{});\n", name, i);
			templateCode = templateCode + name;
		}
		return templateCode;
	}

	std::string FragmentAnalysis::getTexture2DBufferCode(const unsigned int texture2DCount) const
	{
		std::vector<std::string> names;
		for (unsigned int i = 0; i < texture2DCount; i++)
		{
			names.push_back(fmt::format("ks_inputImage{}", i));
		}
		return getTexture2DBufferCode(names);
	}

	std::string FragmentAnalysis::getTexture2DSamplersCode(const std::vector<std::string>& names) const
	{
		std::string templateCode;
		for (size_t i = 0; i < names.size(); i++)
		{
			std::string name = names[i];
			name = fmt::format("SamplerState {}Sampler : register(s{});\n", name, i);
			templateCode = templateCode + name;
		}
		return templateCode;
	}

	std::string FragmentAnalysis::getTexture2DSamplersCode(const unsigned int texture2DCount) const
	{
		std::vector<std::string> names;
		for (unsigned int i = 0; i < texture2DCount; i++)
		{
			names.push_back(fmt::format("ks_inputImage{}", i));
		}
		return getTexture2DSamplersCode(names);
	}

	std::string FragmentAnalysis::getHelperCode(const std::vector<KernelUniform::Info>& uniformInfos, 
		const unsigned int texture2DCount) const
	{
		std::string helperCode = getAdditionalCode();

		for (int number = 0; number < texture2DCount; number++)
		{
			helperCode = helperCode + fmt::format(R"(
float4 ks_getSampler{}Space()
{}
	return ks_sampler{}Space;
{})", number, "{", number, "}");

			helperCode = helperCode + fmt::format(R"(
float2 ks_sampler{}Transform(PS_INPUT input)
{}
	float2 t;
	t.x = ks_remap(input.texcoord.x, ks_sampler{}Space.x, ks_sampler{}Space.z, 0.0, 1.0);
	t.y = ks_remap(input.texcoord.y, ks_sampler{}Space.y, ks_sampler{}Space.w, 0.0, 1.0);
	return t;
{})", number, "{", number, number, number, number, "}");

			helperCode = helperCode + fmt::format(R"(
SamplerState ks_sampler{}()
{}
	return ks_inputImage{}Sampler;
{}
)", number, "{", number, "}");
		}
		return helperCode;
	}
}