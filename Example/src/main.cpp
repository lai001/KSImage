#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_win32.h>
#include <imgui/backends/imgui_impl_dx11.h>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/bundled/format.h>
#include <glm/glm.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <Foundation/Foundation.hpp>
#include <KSImage/KSImage.hpp>
#include "Platform/WindowsPlatform.hpp"
#include "ImageDecoder.hpp"
#include "ImageIO.hpp"

std::unique_ptr<ImageDecoder> imageDecoder = std::make_unique<ImageDecoder>();

struct Transform
{
	float angle = 0.0;
	glm::vec2 offset = glm::vec2(0.0, 0.0);
	float scale = 1.0;
};

struct DataSource
{
	float intensity = 0.5;
	float progress = 0.5;
	Transform transform0;
	Transform transform1;
	bool isEnableDraw = true;
	std::function<bool()> isSaveImage;
	DataSource()
	{
	}

} dataSource;

static std::unique_ptr<WindowsPlatform> windowsPlatformPtr;

unsigned int currentWindowWidth = 1280;
unsigned int currentWindowHeight = 720;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void ImGuiInit()
{
	windowsPlatformPtr->WndProcCallback = [](HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, bool& intercept)
	{
		intercept = ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
		return true;
	};
	windowsPlatformPtr->sizeChange = [](unsigned int width, unsigned int height)
	{
		currentWindowWidth = width;
		currentWindowHeight = height;
	};
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(windowsPlatformPtr->hwnd);
	ImGui_ImplDX11_Init(windowsPlatformPtr->pd3dDevice, windowsPlatformPtr->pd3dDeviceContext);
}

void ImGuiDraw()
{

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Debug");

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::NewLine();
	ImGui::SliderFloat("Intensity", &dataSource.intensity, 0.0, 1.0);
	ImGui::SliderFloat("Progress", &dataSource.progress, 0.0, 1.0);
	ImGui::NewLine();
	ImGui::DragFloat("Angle", &dataSource.transform0.angle);
	ImGui::DragFloat("Scale", &dataSource.transform0.scale, 0.01, 0.1, 3.0);
	ImGui::DragFloat2("Offset", glm::value_ptr(dataSource.transform0.offset));
	ImGui::NewLine();
	ImGui::DragFloat("Angle1", &dataSource.transform1.angle);
	ImGui::DragFloat("Scale1", &dataSource.transform1.scale, 0.01, 0.1, 3.0);
	ImGui::DragFloat2("Offset1", glm::value_ptr(dataSource.transform1.offset));

	if (ImGui::Button("Save image"))
	{
		dataSource.isSaveImage = []()
		{
			dataSource.isSaveImage = std::function<bool()>();
			return true;
		};
	}
	ImGui::Checkbox("IsEnableDraw", &dataSource.isEnableDraw);
	ImGui::End();

	ImGui::Render();
	windowsPlatformPtr->setRenderTarget();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiDestroy()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void drawResult(const std::shared_ptr<ks::PixelBuffer> pixelBuffer)
{
	windowsPlatformPtr->setRenderTarget();

	static const std::string vert = ks::File::read(ks::Application::getResourcePath("Shader/vert.hlsl"), nullptr);
	static const std::string frag = ks::File::read(ks::Application::getResourcePath("Shader/frag.hlsl"), nullptr);

	ks::D3D11RenderEngineCreateInfo createInfo;
	createInfo.device = windowsPlatformPtr->pd3dDevice;
	createInfo.context = windowsPlatformPtr->pd3dDeviceContext;
	static ks::IRenderEngine& engine = *ks::RenderEngine::create(createInfo);

	struct Vertex
	{
		glm::vec2 aPosition;
		glm::vec2 texCoord;
	};

	std::vector<unsigned int> indexBufferData = { 0, 1, 2, 2, 1, 3 };

	std::vector<Vertex> vertexBuffer;
	{
		Vertex topLeft;
		Vertex topRight;
		Vertex bottomLeft;
		Vertex bottomRight;

		topLeft.aPosition = glm::vec2(-1.0, 1.0);
		topLeft.texCoord = glm::vec2(0.0, 0.0);
		topRight.aPosition = glm::vec2(1.0, 1.0);
		topRight.texCoord = glm::vec2(1.0, 0.0);
		bottomLeft.aPosition = glm::vec2(-1.0, -1.0);
		bottomLeft.texCoord = glm::vec2(0.0, 1.0);
		bottomRight.aPosition = glm::vec2(1.0, -1.0);
		bottomRight.texCoord = glm::vec2(1.0, 1.0);

		vertexBuffer.push_back(topLeft);
		vertexBuffer.push_back(topRight);
		vertexBuffer.push_back(bottomLeft);
		vertexBuffer.push_back(bottomRight);
	}
	
	ks::ITexture2D* colorMap = engine.createTexture2D(pixelBuffer->getWidth(),
		pixelBuffer->getHeight(),
		ks::TextureFormat::R8G8B8A8_UNORM,
		pixelBuffer->getImmutableData()[0]);
	static ks::IShader* shader = engine.createShader(vert, frag);
	shader->setTexture2D("colorMap", *colorMap);
	ks::IRenderBuffer * renderBuffer = engine.createRenderBuffer(vertexBuffer.data(), vertexBuffer.size(), sizeof(Vertex),
		*shader,
		indexBufferData.data(), indexBufferData.size(), ks::IIndexBuffer::IndexDataType::uint32);
	ks::IBlendState* blendState = engine.createBlendState(ks::BlendStateDescription::Addition::getDefault(), ks::BlendStateDescription::getDefault());
	ks::IDepthStencilState* depthStencilState = engine.createDepthStencilState(ks::DepthStencilStateDescription::getDefault());
	ks::IRasterizerState* rasterizerState = engine.createRasterizerState(ks::RasterizerStateDescription::getDefault());
	renderBuffer->setViewport(0, 0, currentWindowWidth, currentWindowHeight);
	renderBuffer->setBlendState(*blendState);
	renderBuffer->setDepthStencilState(*depthStencilState);
	renderBuffer->setRasterizerState(*rasterizerState);
	renderBuffer->setPrimitiveTopologyType(ks::PrimitiveTopologyType::trianglelist);
	renderBuffer->commit(nullptr);

	engine.erase(renderBuffer);
	engine.erase(blendState);
	engine.erase(depthStencilState);
	engine.erase(rasterizerState);
	engine.erase(colorMap);
}

void frameTick()
{
	
	static std::shared_ptr<ks::Image> inputImage = std::shared_ptr<ks::Image>(ks::Image::create(ks::Application::getResourcePath("cat.jpg")));

	static std::shared_ptr<ks::Image> inputTargetImage = std::shared_ptr<ks::Image>(ks::Image::create(ks::Application::getResourcePath("environmental.jpg")));
	
	static std::shared_ptr<ks::TransformFilter> transformFilter0 = std::shared_ptr<ks::TransformFilter>(ks::TransformFilter::create());
	transformFilter0->name = "transformFilter0";
	transformFilter0->inputImage = inputImage.get();
	transformFilter0->transform = ks::RectTransDescription(inputImage->getRect())
		.scaleAroundCenter(glm::vec2(dataSource.transform0.scale))
		.rotateAroundCenter(glm::radians<float>(dataSource.transform0.angle))
		.translate(dataSource.transform0.offset)
		.getTransform();

	static std::shared_ptr<ks::TransformFilter> transformFilter1 = std::shared_ptr<ks::TransformFilter>(ks::TransformFilter::create());
	transformFilter1->name = "transformFilter1";
	transformFilter1->inputImage = inputTargetImage.get();
	transformFilter1->transform = ks::RectTransDescription(inputTargetImage->getRect())
		.scaleAroundCenter(glm::vec2(dataSource.transform1.scale))
		.rotateAroundCenter(glm::radians<float>(dataSource.transform1.angle))
		.translate(dataSource.transform1.offset)
		.getTransform();

	static std::shared_ptr<ks::MixTwoImageFilter> mixTwoImageFilter0 = std::shared_ptr<ks::MixTwoImageFilter>(ks::MixTwoImageFilter::create());
	mixTwoImageFilter0->name = "mixTwoImageFilter0";
	mixTwoImageFilter0->inputImage = transformFilter0->outputImage();
	mixTwoImageFilter0->inputTargetImage = transformFilter1->outputImage();
	mixTwoImageFilter0->u_intensity = dataSource.intensity;

	static std::unique_ptr<ks::FilterContext> context = std::unique_ptr<ks::FilterContext>(ks::FilterContext::create());

	std::shared_ptr<ks::PixelBuffer> bufferPtr = 
		std::shared_ptr<ks::PixelBuffer>(context->render(*mixTwoImageFilter0->outputImage(), ks::Rect(0.0, 0.0, 1280, 720)));
	  
	if (dataSource.isSaveImage && dataSource.isSaveImage())
	{
		std::string targetPath = fmt::format("{}/{}.png", ks::Application::getAppDir(), "KSImage");
		bool writeStatus = ImageIO::saveImage(*bufferPtr.get(), targetPath);
		spdlog::debug("{}, {}", writeStatus, targetPath);
	}

	if (dataSource.isEnableDraw)
	{
		drawResult(bufferPtr);
	}
}

int main(int argc, char** argv)
{
	spdlog::set_level(spdlog::level::trace);
	ks::Application::Init(argc, argv);

	ks::IImageDecoder::shared = imageDecoder.get();

	WindowsPlatform::Configuration cfg;
	cfg.windowHeight = currentWindowHeight;
	cfg.windowWidth = currentWindowWidth;
	cfg.windowName = "KSImage Example";
	windowsPlatformPtr = std::make_unique<WindowsPlatform>();
	windowsPlatformPtr->Init(cfg);

	ks::D3D11RenderEngineCreateInfo info;
	info.device = windowsPlatformPtr->pd3dDevice;
	info.context = windowsPlatformPtr->pd3dDeviceContext;
	ks::FilterContext::Init(info);

	ImGuiInit();
	defer
	{
		ImGuiDestroy();
	};

	while (windowsPlatformPtr->shouldClose() == false)
	{
		windowsPlatformPtr->clearColor();
		frameTick();
		ImGuiDraw();
		windowsPlatformPtr->present();
	}

	return 0;
}