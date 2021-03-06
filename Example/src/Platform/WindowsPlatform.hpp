#ifndef KSImage_Example_src_Platform_WindowsPlatform_hpp
#define KSImage_Example_src_Platform_WindowsPlatform_hpp

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#include <d3d11.h>
#include <tchar.h>
#include <Foundation/Foundation.hpp>

class WindowsPlatform : public ks::noncopyable
{
public:
	struct Configuration
	{
		std::string windowName = "";
		unsigned int windowWidth = 1024;
		unsigned int windowHeight = 720;
		int x = 0;
		int y = 0;
	};

public:
	WindowsPlatform();
	~WindowsPlatform();

	std::function<bool(HWND, UINT, WPARAM, LPARAM, bool&)> WndProcCallback;
	std::function<void(unsigned int width, unsigned int height)> sizeChange;

	void Init(const Configuration &cfg);
	bool shouldClose();
	void present(const int vsync = 1) const noexcept;
	void setRenderTarget() const noexcept;
	void clearColor();

public:
	Configuration cfg;

	HWND hwnd = nullptr;
	WNDCLASSEX wc;

	ID3D11Device*            pd3dDevice = nullptr;
	ID3D11DeviceContext*     pd3dDeviceContext = nullptr;
	IDXGISwapChain*          pSwapChain = nullptr;
	ID3D11RenderTargetView*  mainRenderTargetView = nullptr;

private:
	bool createDeviceD3D(HWND hWnd);
	void cleanupDeviceD3D();
	void createRenderTarget();
	void cleanupRenderTarget();

	static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif // _WIN32
#endif // !KSImage_Example_src_Platform_WindowsPlatform_hpp
