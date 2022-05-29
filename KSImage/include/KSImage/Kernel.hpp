#ifndef KSImage_Kernel_hpp
#define KSImage_Kernel_hpp

#include <string>
#include <Foundation/Foundation.hpp>
#include <KSRenderEngine/KSRenderEngine.hpp>
#include "KernelUniform.hpp"
#include "defs.hpp"
#include "FragmentAnalysis.hpp"

namespace ks
{
	class KSImage_API Kernel : public noncopyable
	{
	public:
		static IRenderEngine* renderEngine;

	private:
		IShader* shader = nullptr;
		IRenderBuffer* renderBuffer = nullptr;
		FragmentAnalysis::AnalysisResult analysisResult;
		void setUniform(const std::string& name, const ks::UniformValue& value) noexcept;
		void setTexture2D(const std::string& name, const ks::ITexture2D& texture2D) noexcept;

	public:
		~Kernel();

		static std::shared_ptr<Kernel> create(const std::string& kernelFragmentShader) noexcept;

		void setVertexObject(const void* vertexBuffer, 
			const unsigned int vertexCount, 
			const unsigned int stride,
			const void* indexBuffer,
			const unsigned int indexCount,
			ks::IIndexBuffer::IndexDataType indexDataType) noexcept;

		void setUniform(const std::vector<ks::KernelUniform::Value>& uniformValues, 
			std::function<ITexture2D*(unsigned int, glm::vec4*)> unretainTexture2D) noexcept;
		void commit(std::shared_ptr<ks::IFrameBuffer> frameBuffer);
	};
}

#endif // !KSImage_Kernel_hpp