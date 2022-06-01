#ifndef KSImage_Filter_hpp
#define KSImage_Filter_hpp

#include <Foundation/Foundation.hpp>
#include <KSRenderEngine/KSRenderEngine.hpp>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>
#include "KernelUniform.hpp"
#include "Image.hpp"
#include "Kernel.hpp"
#include "Util.hpp"
#include "defs.hpp"

namespace ks
{
	class KSImage_API Filter: public noncopyable
	{
	protected:
		std::shared_ptr<ks::Kernel> kernel = std::shared_ptr<ks::Kernel>(nullptr);
		ks::Image* currentOutputImage = nullptr;
		std::vector<ks::KernelUniform::Value> uniformValues;

		~Filter();

	public:
		virtual KernelRenderInstruction onPrepare(const ks::Rect& renderRect);
		virtual ks::Image* outputImage(const ks::Rect* rect = nullptr);
		void setValues(const std::vector<ks::KernelUniform::Value>& uniformValues) noexcept;
		std::shared_ptr<ks::Kernel> getKernel() const noexcept;
		std::vector<ks::KernelUniform::Value> getUniformValues() const noexcept;
		std::vector<ks::Image*> getInputImages() const noexcept;
		
		const ks::Image* getCurrentOutputImage() const noexcept;

		std::string name;
	};
}

#endif // !KSImage_Filter_hpp