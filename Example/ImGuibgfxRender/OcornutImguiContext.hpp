#ifndef OCORNUTIMGUICONTEXT_HPP
#define OCORNUTIMGUICONTEXT_HPP

#include <vector>
#include <bgfx/bgfx.h>
#include <bgfx/embedded_shader.h>
#include <bgfx/defines.h>
#include <bgfx/platform.h>
#include <bx/bx.h>
#include <bx/allocator.h>
#include <bx/math.h>
#include <bx/timer.h>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

struct OcornutImguiContext
{
	static const std::vector<bgfx::EmbeddedShader> s_embeddedShaders;

	ImGuiContext*       m_imgui;
	bx::AllocatorI*     m_allocator;
	bgfx::VertexLayout  m_layout;
	bgfx::ProgramHandle m_program;
	bgfx::ProgramHandle m_imageProgram;
	bgfx::TextureHandle m_texture;
	bgfx::UniformHandle s_tex;
	bgfx::UniformHandle u_imageLodEnabled;
	//int64_t m_last;
	//int32_t m_lastScroll;
	bgfx::ViewId m_viewId;

	bool checkAvailTransientBuffers(uint32_t _numVertices, const bgfx::VertexLayout& _layout, uint32_t _numIndices) const noexcept;

	static void* memAlloc(size_t _size, void* _userData) noexcept;

	static void memFree(void* _ptr, void* _userData) noexcept;

	void render(ImDrawData* _drawData) noexcept;

	void create(float _fontSize, bx::AllocatorI* _allocator) noexcept;

	void destroy() noexcept;

	void setupStyle(bool _dark) noexcept;

	void beginFrame(int _width, int _height, bgfx::ViewId _viewId) noexcept;

	void endFrame() noexcept;
};

static OcornutImguiContext s_ctx;

void imguiCreate(float _fontSize = 18.0f, bx::AllocatorI* _allocator = NULL) noexcept;
void imguiDestroy() noexcept;

void imguiBeginFrame(uint16_t _width, uint16_t _height, bgfx::ViewId _view = 255) noexcept;
void imguiEndFrame() noexcept;

#endif // !OCORNUTIMGUICONTEXT_HPP