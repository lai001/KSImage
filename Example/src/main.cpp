#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_win32.h>
#include <imgui/backends/imgui_impl_dx11.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/bundled/format.h>
#include <glm/glm.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <Foundation/Foundation.hpp>
#include <KSImage/KSImage.hpp>
#include "Platform/WindowsPlatform.hpp"
#include "ImGuibgfxRender/OcornutImguiContext.hpp"
#include "ImageDecoder.hpp"

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
	bool isSaveImage = false;
	bool isEnableDraw = true;

	DataSource()
	{
	}

} dataSouce;

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

	imguiCreate();
	ImGui_ImplWin32_Init(windowsPlatformPtr->hwnd);
}

void ImGuiDraw()
{
	imguiBeginFrame(currentWindowWidth, currentWindowHeight);
	defer{ imguiEndFrame(); };

	ImGui::Begin("Debug");

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::NewLine();
	ImGui::SliderFloat("Intensity", &dataSouce.intensity, 0.0, 1.0);
	ImGui::NewLine();
	ImGui::DragFloat("Angle", &dataSouce.transform0.angle);
	ImGui::DragFloat("Scale", &dataSouce.transform0.scale, 0.01, 0.1, 3.0);
	ImGui::DragFloat2("Offset", glm::value_ptr(dataSouce.transform0.offset));
	ImGui::NewLine();
	ImGui::DragFloat("Angle1", &dataSouce.transform1.angle);
	ImGui::DragFloat("Scale1", &dataSouce.transform1.scale, 0.01, 0.1, 3.0);
	ImGui::DragFloat2("Offset1", glm::value_ptr(dataSouce.transform1.offset));

	if (ImGui::Button("Save"))
	{
		dataSouce.isSaveImage = true;
	}
	ImGui::Checkbox("IsEnableDraw", &dataSouce.isEnableDraw);
	ImGui::End();
}

void ImGuiDestroy()
{
	ImGui_ImplWin32_Shutdown();
	imguiDestroy();
}

void frameTick()
{
	static std::shared_ptr<ks::Image> inputImage = ks::Image::create(ks::Application::getResourcePath("cat.jpg"));
	static std::shared_ptr<ks::Image> inputTargetImage = ks::Image::create(ks::Application::getResourcePath("environmental.jpg"));

	static std::shared_ptr<ks::TransformFilter> transformFilter0 = ks::TransformFilter::create();
	transformFilter0->inputImage = inputImage;
	transformFilter0->transform = ks::RectTransDescription(inputImage->getRect())
		.scaleAroundCenter(glm::vec2(dataSouce.transform0.scale))
		.rotateAroundCenter(glm::radians<float>(dataSouce.transform0.angle))
		.translate(dataSouce.transform0.offset)
		.getTransform();

	static std::shared_ptr<ks::TransformFilter> transformFilter1 = ks::TransformFilter::create();
	transformFilter1->inputImage = inputTargetImage;
	transformFilter1->transform = ks::RectTransDescription(inputTargetImage->getRect())
		.scaleAroundCenter(glm::vec2(dataSouce.transform1.scale))
		.rotateAroundCenter(glm::radians<float>(dataSouce.transform1.angle))
		.translate(dataSouce.transform1.offset)
		.getTransform();

	static std::shared_ptr<ks::MixTwoImageFilter> mixTwoImageFilter0 = ks::MixTwoImageFilter::create();
	mixTwoImageFilter0->inputImage = transformFilter0->outputImage();
	mixTwoImageFilter0->inputTargetImage = transformFilter1->outputImage();
	mixTwoImageFilter0->u_intensity = dataSouce.intensity;

	static std::unique_ptr<ks::FilterContext> context = std::unique_ptr<ks::FilterContext>(ks::FilterContext::create());

	std::unique_ptr<ks::PixelBuffer> bufferPtr = std::unique_ptr<ks::PixelBuffer>(context->render(*mixTwoImageFilter0->outputImage().get(), ks::Rect(0.0, 0.0, 1280.0, 720.0)));

	if (dataSouce.isSaveImage)
	{
		unsigned char* data = reinterpret_cast<unsigned char*>(bufferPtr->getMutableData()[0]);
		std::string targetPath = fmt::format("{}/{}.png", ks::Application::getAppDir(), "KSImage");
		int writeStatus = stbi_write_png(targetPath.c_str(), bufferPtr->getWidth(), bufferPtr->getHeight(), bufferPtr->getChannels(), data, bufferPtr->getWidth() * bufferPtr->getChannels());
		spdlog::debug("{}, {}", bool(writeStatus), targetPath);
		dataSouce.isSaveImage = false;
	}

	bool isDraw = dataSouce.isEnableDraw;
	bgfx::ViewId mainViewId = 0;
	bgfx::touch(mainViewId);
	bgfx::setViewRect(mainViewId, 0, 0, currentWindowWidth, currentWindowHeight);
	bgfx::setViewClear(mainViewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x000000ff, 1.0f, 0);
	bgfx::setViewFrameBuffer(mainViewId, BGFX_INVALID_HANDLE);
	if (isDraw)
	{
		static std::shared_ptr<ks::Kernel> kernel = ks::Kernel::create("image", { ks::KernelUniform::Info("s_texColor", ks::KernelUniform::ValueType::texture2d) });
		const std::vector<unsigned int> indices = {
			0, 1, 2,
			1, 2, 3
		};
		const ks::Rect fitRect = ks::makeRectAspectFit(bufferPtr->getWidth(), bufferPtr->getHeight(), ks::Rect(0.0, 0.0, currentWindowWidth, currentWindowHeight));
		ks::RectTransDescription des(fitRect);
		des = ks::convertToNDC(des, ks::Rect(0.0, 0.0, currentWindowWidth, currentWindowHeight));

		kernel->bindIndices(indices);
		const std::vector<ks::ImageVertex> vertexs = {
			ks::ImageVertex(glm::vec3(des.getQuad().topLeft, 1.0), glm::vec2(0.0, 0.0)), // Top-Left
			ks::ImageVertex(glm::vec3(des.getQuad().topRight, 1.0),  glm::vec2(1.0, 0.0)), // Top-Right
			ks::ImageVertex(glm::vec3(des.getQuad().bottomLeft, 1.0), glm::vec2(0.0, 1.0)), // Bottom-Left
			ks::ImageVertex(glm::vec3(des.getQuad().bottomRight, 1.0),  glm::vec2(1.0, 1.0)), // Bottom-Right
		};
		kernel->bindVertex(vertexs.data(), vertexs.size() * sizeof(ks::ImageVertex), ks::ImageVertex::vertexLayout, 0);
		kernel->bindTexture2D("s_texColor", *bufferPtr.get());

		bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A);
		bgfx::submit(mainViewId, kernel->getBgfxProgram());
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

	ks::FilterContext::Init(windowsPlatformPtr->hwnd, currentWindowWidth, currentWindowHeight);
	ImGuiInit();
	defer
	{
		ImGuiDestroy();
		ks::FilterContext::shutdown();
	};

	while (windowsPlatformPtr->shouldClose() == false)
	{
		ImGuiDraw();
		frameTick();
	}

	return 0;
}