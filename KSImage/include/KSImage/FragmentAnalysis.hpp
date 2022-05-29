#ifndef KSImage_FragmentAnalysis_hpp
#define KSImage_FragmentAnalysis_hpp

#include <Foundation/Foundation.hpp>
#include <string>
#include <vector>
#include <spdlog/fmt/fmt.h>
#include "KernelUniform.hpp"
#include "defs.hpp"

namespace ks
{
	class KSImage_API FragmentAnalysis
	{
	public:
		struct ShareInfo
		{
			static std::string cBufferBlockName() noexcept
			{
				return "Uniforms";
			}

			static std::string internalPrefix() noexcept
			{
				return "ks_";
			}

			static std::string workingSpacePixelSizeUniformName() noexcept
			{
				return internalPrefix() + "workingSpacePixelSize";
			}

			static std::string uniformSamplerSpaceName(const unsigned int number) noexcept
			{
				return fmt::format("{}sampler{}Space", internalPrefix(), number);
			}

			static std::string texture2DName(const unsigned int number) noexcept
			{
				return fmt::format("{}inputImage{}", internalPrefix(), number);
			}

			static std::string texture2DSamplerName(const unsigned int number) noexcept
			{
				return fmt::format("{}inputImage{}Sampler", internalPrefix(), number);
			}
		};

		struct AnalysisResult
		{
			std::string fragmentShaderCode = "";
			std::string entryFuncName = "";
			std::vector<ks::KernelUniform::Info> uniformInfos;
			std::vector<ks::KernelUniform::Info> internalUniformInfos;
			int texture2DCount = 0;
		};

	public:
		AnalysisResult analysis(const std::string& code) const noexcept;
		std::string generateFragmentShaderCode(const AnalysisResult& result, const std::string& originSourceCode) const noexcept;

	private:
		std::string getConstantBufferCode(const std::vector<ks::KernelUniform::Info>& uniformInfos) const;
		std::string getTexture2DBufferCode(const std::vector<std::string>& names) const;
		std::string getTexture2DBufferCode(const unsigned int texture2DCount) const;
		std::string getTexture2DSamplersCode(const std::vector<std::string>& names) const;
		std::string getTexture2DSamplersCode(const unsigned int texture2DCount) const;

		std::string getHelperCode(const std::vector<ks::KernelUniform::Info>& uniformInfos,
			const unsigned int texture2DCount) const;
	};
}

#endif // !KSImage_FragmentAnalysis_hpp