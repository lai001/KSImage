#ifndef FRAGMENTANALYSISHPP
#define FRAGMENTANALYSISHPP

#include <Foundation/Foundation.hpp>
#include <string>
#include <vector>
#include <spdlog/fmt/fmt.h>
#include "KernelUniform.hpp"
#include "KernelTexture2D.hpp"
#include "defs.hpp"

namespace ks
{
	class KSImage_API FragmentAnalysis
	{
	public:
		struct ShareInfo
		{
			std::string uniformPrefix() const noexcept
			{
				return "__u_";
			}

			std::string workingSpacePixelSizeUniformName() const noexcept
			{
				return uniformPrefix() + "workingSpacePixelSize";
			}

			std::string uniformSamplerSpaceName(const unsigned int number) const noexcept
			{
				return fmt::format("__u_sampler{}Space", number);
			}
		};

		struct AnalysisResult
		{
			std::string fragmentShaderCode = "";
			std::string entryFuncName = "";
			std::vector<ks::KernelUniform::Info> uniformInfos;
			int texture2DCount = 0;
		};

	public:
		AnalysisResult analysis(const std::string& code) const noexcept;
		AnalysisResult analysisTest() const noexcept;
		std::string generateFragmentShaderCode(const AnalysisResult& result, const std::string& originSourceCode) const noexcept;

	private:
		bool replace(std::string& str, const std::string& from, const std::string& to) const noexcept;
	};
}

#endif // !FRAGMENTANALYSISHPP