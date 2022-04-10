#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <spdlog/spdlog.h>
#include <stb_image_write.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <Foundation/Foundation.hpp>
#include <KSImage/KSImage.hpp>
#include "Platform/WindowsPlatform.hpp"
#include "ImGuibgfxRender/OcornutImguiContext.hpp"

struct DataSource
{
	float intensity = 0.5;
};

static DataSource dataSouce;

static std::unique_ptr<WindowsPlatform> windowsPlatformPtr;

const unsigned short kDefaultWidth = 1280;
const unsigned short kDefaultHeight = 720;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void ImGuiInit()
{
	windowsPlatformPtr->WndProcCallback = [](HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, bool& intercept)
	{
		intercept = ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
		return true;
	};

	imguiCreate();
	ImGui_ImplWin32_Init(windowsPlatformPtr->hwnd);
}

void ImGuiDraw()
{
	imguiBeginFrame(kDefaultWidth, kDefaultHeight);
	defer { imguiEndFrame(); };

	ImGui::Begin("Debug");

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::SliderFloat("intensity", &dataSouce.intensity, 0.0, 1.0);

	ImGui::End();
}

void ImGuiDestroy()
{
	ImGui_ImplWin32_Shutdown();
	imguiDestroy();
}

void frameTick() noexcept
{
	static std::shared_ptr<ks::Image> inputImage = ks::Image::create(fd::Application::getResourcePath("cat.jpg"));
	static std::shared_ptr<ks::Image> inputTargetImage = ks::Image::create(fd::Application::getResourcePath("environmental.jpg"));
	static std::shared_ptr<ks::MixTwoImageFilter> filterPtr = ks::MixTwoImageFilter::create();
	filterPtr->inputImage = inputImage;
	filterPtr->inputTargetImage = inputTargetImage;
	filterPtr->u_intensity = dataSouce.intensity;
	std::shared_ptr<ks::Image> outputImage = filterPtr->outputImage();
	std::shared_ptr<ks::FilterContext> context = ks::FilterContext::create();
	std::shared_ptr<ks::PixelBuffer> bufferPtr = context->render(outputImage.get());

	const bool isSaveToDisk = false;
	if (isSaveToDisk)
	{
		static int image_number = 0;
		defer
		{
			image_number++;
			image_number = image_number % 60;
		};
		unsigned char* data = (unsigned char*)bufferPtr->getMutableData();
		std::string targetPath = fmt::format("{}/{}.png", fd::Application::getAppDir(), image_number);
		int writeStatus = stbi_write_png(targetPath.c_str(), bufferPtr->getWidth(), bufferPtr->getHeight(), bufferPtr->getChannels(), data, bufferPtr->getWidth() * bufferPtr->getChannels());
		spdlog::debug(targetPath);
	}

	const bool isDraw = true;
	bgfx::ViewId mainViewId = 0;
	bgfx::touch(mainViewId);
	bgfx::setViewRect(mainViewId, 0, 0, kDefaultWidth, kDefaultHeight);
	bgfx::setViewClear(mainViewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x000000ff, 1.0f, 0);
	bgfx::setViewFrameBuffer(mainViewId, BGFX_INVALID_HANDLE);
	if (isDraw)
	{
		//bgfx::reset(kDefaultWidth, kDefaultHeight, BGFX_RESET_FLUSH_AFTER_RENDER, bgfx::TextureFormat::RGBA8);
		static std::shared_ptr<ks::Kernel> kernel = ks::Kernel::create("image", { ks::KernelUniform::Info("s_texColor", ks::KernelUniform::ValueType::texture2d) });
		kernel->bindIndices(std::vector<unsigned int>{
			0, 1, 2,
			1, 2, 3
		});
		std::vector<ks::ImageVertex> vertexs = {
			ks::ImageVertex(glm::vec3(-1,  1, 0), glm::vec2(0.0, 0.0)), // Top-Left
			ks::ImageVertex(glm::vec3(1,  1, 0), glm::vec2(1.0, 0.0)), // Top-Right
			ks::ImageVertex(glm::vec3(-1, -1, 0), glm::vec2(0.0, 1.0)), // Bottom-Left
			ks::ImageVertex(glm::vec3(1, -1, 0), glm::vec2(1.0, 1.0)), // Bottom-Right
		};
		kernel->bindVertex(vertexs.data(), vertexs.size() * sizeof(ks::ImageVertex), ks::ImageVertex::vertexLayout, 0);
		kernel->bindTexture2D("s_texColor", bufferPtr);

		bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A);
		bgfx::submit(mainViewId, kernel->getBgfxProgram());
	}
}

int main(int argc, char** argv)
{
	spdlog::set_level(spdlog::level::trace);
	fd::Application::Init(argc, argv);

	WindowsPlatform::Configuration cfg;
	cfg.windowHeight = kDefaultHeight;
	cfg.windowWidth = kDefaultWidth;
	cfg.windowName = "KSImage Example";
	windowsPlatformPtr = std::make_unique<WindowsPlatform>();
	windowsPlatformPtr->Init(cfg);

	ks::FilterContext::Init(windowsPlatformPtr->hwnd, kDefaultWidth, kDefaultHeight);
	ImGuiInit();

	while (windowsPlatformPtr->shouldClose() == false)
	{
		ImGuiDraw();
		frameTick();
	}

	ImGuiDestroy();
	return 0;
}