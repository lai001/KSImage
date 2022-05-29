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
	windowsPlatformPtr->clearColor();

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Debug");

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::NewLine();
	ImGui::SliderFloat("Intensity", &dataSource.intensity, 0.0, 1.0);
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
	windowsPlatformPtr->present();
}

void ImGuiDestroy()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void frameTick()
{
	static std::shared_ptr<ks::Image> inputImage = ks::Image::create(ks::Application::getResourcePath("cat.jpg"));
	static std::shared_ptr<ks::Image> inputTargetImage = ks::Image::create(ks::Application::getResourcePath("environmental.jpg"));

	static std::shared_ptr<ks::TransformFilter> transformFilter0 = ks::TransformFilter::create();
	transformFilter0->inputImage = inputImage;
	transformFilter0->transform = ks::RectTransDescription(inputImage->getRect())
		.scaleAroundCenter(glm::vec2(dataSource.transform0.scale))
		.rotateAroundCenter(glm::radians<float>(dataSource.transform0.angle))
		.translate(dataSource.transform0.offset)
		.getTransform();

	static std::shared_ptr<ks::TransformFilter> transformFilter1 = ks::TransformFilter::create();
	transformFilter1->inputImage = inputTargetImage;
	transformFilter1->transform = ks::RectTransDescription(inputTargetImage->getRect())
		.scaleAroundCenter(glm::vec2(dataSource.transform1.scale))
		.rotateAroundCenter(glm::radians<float>(dataSource.transform1.angle))
		.translate(dataSource.transform1.offset)
		.getTransform();

	static std::shared_ptr<ks::MixTwoImageFilter> mixTwoImageFilter0 = ks::MixTwoImageFilter::create();
	mixTwoImageFilter0->inputImage = transformFilter0->outputImage();
	mixTwoImageFilter0->inputTargetImage = transformFilter1->outputImage();
	mixTwoImageFilter0->u_intensity = dataSource.intensity;

	static std::unique_ptr<ks::FilterContext> context = std::unique_ptr<ks::FilterContext>(ks::FilterContext::create());

	std::unique_ptr<ks::PixelBuffer> bufferPtr = 
		std::unique_ptr<ks::PixelBuffer>(context->render(*mixTwoImageFilter0->outputImage().get(), ks::Rect(0.0, 0.0, 1280, 720)));

	if (dataSource.isSaveImage && dataSource.isSaveImage())
	{
		std::string targetPath = fmt::format("{}/{}.png", ks::Application::getAppDir(), "KSImage");
		bool writeStatus = ImageIO::saveImage(*bufferPtr.get(), targetPath);
		spdlog::debug("{}, {}", writeStatus, targetPath);
	}

	if (dataSource.isEnableDraw)
	{

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
		ImGuiDraw();
		frameTick();
	}

	return 0;
}