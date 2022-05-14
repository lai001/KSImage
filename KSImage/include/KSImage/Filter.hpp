#ifndef FILTER_HPP
#define FILTER_HPP

#include <Foundation/Foundation.hpp>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>
#include "Image.hpp"
#include "Kernel.hpp"
#include "Util.hpp"
#include "defs.hpp"

namespace ks
{
	struct KSImage_API KernelRenderInstruction
	{
	public:
		std::vector<glm::vec4> sampleSapceRectsNorm;
	};

	class KSImage_API Filter: public boost::noncopyable
	{
	protected:
		std::shared_ptr<ks::Kernel> kernel = std::shared_ptr<ks::Kernel>(nullptr);
		std::shared_ptr<ks::Image> currentOutputImage = std::shared_ptr<ks::Image>(nullptr);
		std::vector<ks::KernelUniform::Value> uniformValues;

	public:
		virtual KernelRenderInstruction onPrepare(const ks::Rect& renderRect);
		virtual std::shared_ptr<ks::Image> outputImage(const ks::Rect* rect = nullptr);
		void setValues(const std::vector<ks::KernelUniform::Value>& uniformValues) noexcept;
		std::shared_ptr<ks::Kernel> getKernel() const noexcept;
		std::vector<ks::KernelUniform::Value> getUniformValues() const noexcept;
		std::vector<std::shared_ptr<ks::Image>> getInputImages() const noexcept;
		
		const std::shared_ptr<ks::Image> getCurrentOutputImage() const noexcept;
	};
}

#endif // !FILTER_HPP