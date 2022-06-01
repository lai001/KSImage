#ifndef KSImage_Kernel_hpp
#define KSImage_Kernel_hpp

#include <string>
#include <glm/glm.hpp>
#include <Foundation/Foundation.hpp>
#include <KSRenderEngine/KSRenderEngine.hpp>
#include "KernelUniform.hpp"
#include "defs.hpp"
#include "FragmentAnalysis.hpp"

namespace ks
{
	struct KSImage_API KernelRenderInstruction
	{
	public:
		std::vector<glm::vec4> sampleSapceRectsNorm;
		glm::vec2 workingSpacePixelSize;
	};

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
			std::vector<ks::ITexture2D*> textureHandles,
			const KernelRenderInstruction& renderInstruction) noexcept;

		void commit(ks::IFrameBuffer& frameBuffer);
	};
}

#endif // !KSImage_Kernel_hpp