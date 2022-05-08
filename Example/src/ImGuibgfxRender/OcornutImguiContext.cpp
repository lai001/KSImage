#include "OcornutImguiContext.hpp"
#include "vs_ocornut_imgui.bin.h"
#include "fs_ocornut_imgui.bin.h"
#include "vs_imgui_image.bin.h"
#include "fs_imgui_image.bin.h"

#define IMGUI_FLAGS_NONE        UINT8_C(0x00)
#define IMGUI_FLAGS_ALPHA_BLEND UINT8_C(0x01)

const std::vector<bgfx::EmbeddedShader> OcornutImguiContext::s_embeddedShaders =
{
	BGFX_EMBEDDED_SHADER(vs_ocornut_imgui),
	BGFX_EMBEDDED_SHADER(fs_ocornut_imgui),
	BGFX_EMBEDDED_SHADER(vs_imgui_image),
	BGFX_EMBEDDED_SHADER(fs_imgui_image),

	BGFX_EMBEDDED_SHADER_END()
};

void imguiCreate(float _fontSize, bx::AllocatorI* _allocator) noexcept
{
	s_ctx.create(_fontSize, _allocator);
}

void imguiDestroy() noexcept
{
	s_ctx.destroy();
}

void imguiBeginFrame(uint16_t _width, uint16_t _height, bgfx::ViewId _viewId) noexcept
{
	s_ctx.beginFrame(_width, _height, _viewId);
}

void imguiEndFrame() noexcept
{
	s_ctx.endFrame();
}

bool OcornutImguiContext::checkAvailTransientBuffers(uint32_t _numVertices, const bgfx::VertexLayout & _layout, uint32_t _numIndices) const noexcept
{
	bool condition0 = _numVertices == bgfx::getAvailTransientVertexBuffer(_numVertices, _layout);
	bool condition1 = (0 == _numIndices || _numIndices == bgfx::getAvailTransientIndexBuffer(_numIndices));
	return condition0 && condition1;
}

void * OcornutImguiContext::memAlloc(size_t _size, void * _userData) noexcept
{
	OcornutImguiContext* s_ctx_ptr = reinterpret_cast<OcornutImguiContext*>(_userData);
	return BX_ALLOC(s_ctx_ptr->m_allocator, _size);
}

void OcornutImguiContext::memFree(void * _ptr, void * _userData) noexcept
{
	OcornutImguiContext* s_ctx_ptr = reinterpret_cast<OcornutImguiContext*>(_userData);
	BX_FREE(s_ctx_ptr->m_allocator, _ptr);
}

void OcornutImguiContext::render(ImDrawData * _drawData) noexcept
{
	ImGuiIO& io = ImGui::GetIO();
	
	// Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
	int fb_width = (int)(_drawData->DisplaySize.x * _drawData->FramebufferScale.x);
	int fb_height = (int)(_drawData->DisplaySize.y * _drawData->FramebufferScale.y);
	if (fb_width <= 0 || fb_height <= 0)
		return;

	bgfx::setViewName(m_viewId, "ImGui");
	bgfx::setViewMode(m_viewId, bgfx::ViewMode::Sequential);

	const bgfx::Caps* caps = bgfx::getCaps();
	{
		float ortho[16];
		float x = _drawData->DisplayPos.x;
		float y = _drawData->DisplayPos.y;
		float width = _drawData->DisplaySize.x;
		float height = _drawData->DisplaySize.y;

		bx::mtxOrtho(ortho, x, x + width, y + height, y, 0.0f, 1000.0f, 0.0f, caps->homogeneousDepth);
		bgfx::setViewTransform(m_viewId, NULL, ortho);
		bgfx::setViewRect(m_viewId, 0, 0, uint16_t(width), uint16_t(height));
	}

	const ImVec2 clipPos = _drawData->DisplayPos;       // (0,0) unless using multi-viewports
	const ImVec2 clipScale = _drawData->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

	// Render command lists
	for (int32_t ii = 0, num = _drawData->CmdListsCount; ii < num; ++ii)
	{
		bgfx::TransientVertexBuffer tvb;
		bgfx::TransientIndexBuffer tib;

		const ImDrawList* drawList = _drawData->CmdLists[ii];
		uint32_t numVertices = (uint32_t)drawList->VtxBuffer.size();
		uint32_t numIndices = (uint32_t)drawList->IdxBuffer.size();

		if (!checkAvailTransientBuffers(numVertices, m_layout, numIndices))
		{
			// not enough space in transient buffer just quit drawing the rest...
			break;
		}

		bgfx::allocTransientVertexBuffer(&tvb, numVertices, m_layout);
		bgfx::allocTransientIndexBuffer(&tib, numIndices, sizeof(ImDrawIdx) == 4);

		ImDrawVert* verts = (ImDrawVert*)tvb.data;
		bx::memCopy(verts, drawList->VtxBuffer.begin(), numVertices * sizeof(ImDrawVert));

		ImDrawIdx* indices = (ImDrawIdx*)tib.data;
		bx::memCopy(indices, drawList->IdxBuffer.begin(), numIndices * sizeof(ImDrawIdx));

		bgfx::Encoder* encoder = bgfx::begin();

		for (const ImDrawCmd* cmd = drawList->CmdBuffer.begin(), *cmdEnd = drawList->CmdBuffer.end(); cmd != cmdEnd; ++cmd)
		{
			if (cmd->UserCallback)
			{
				cmd->UserCallback(drawList, cmd);
			}
			else if (0 != cmd->ElemCount)
			{
				uint64_t state = 0
					| BGFX_STATE_WRITE_RGB
					| BGFX_STATE_WRITE_A
					| BGFX_STATE_MSAA
					;

				bgfx::TextureHandle th = m_texture;
				bgfx::ProgramHandle program = m_program;

				if (NULL != cmd->TextureId)
				{
					union
					{
						ImTextureID ptr;
						struct
						{
							bgfx::TextureHandle handle; uint8_t flags; uint8_t mip;
						} s;
					} texture = { cmd->TextureId };
					state |= 0 != (IMGUI_FLAGS_ALPHA_BLEND & texture.s.flags)
						? BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA)
						: BGFX_STATE_NONE
						;
					th = texture.s.handle;
					if (0 != texture.s.mip)
					{
						const float lodEnabled[4] = { float(texture.s.mip), 1.0f, 0.0f, 0.0f };
						bgfx::setUniform(u_imageLodEnabled, lodEnabled);
						program = m_imageProgram;
					}
				}
				else
				{
					state |= BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA);
				}

				// Project scissor/clipping rectangles into framebuffer space
				ImVec4 clipRect;
				clipRect.x = (cmd->ClipRect.x - clipPos.x) * clipScale.x;
				clipRect.y = (cmd->ClipRect.y - clipPos.y) * clipScale.y;
				clipRect.z = (cmd->ClipRect.z - clipPos.x) * clipScale.x;
				clipRect.w = (cmd->ClipRect.w - clipPos.y) * clipScale.y;

				if (clipRect.x < fb_width
					&&  clipRect.y < fb_height
					&&  clipRect.z >= 0.0f
					&&  clipRect.w >= 0.0f)
				{
					const uint16_t xx = uint16_t(bx::max(clipRect.x, 0.0f));
					const uint16_t yy = uint16_t(bx::max(clipRect.y, 0.0f));
					encoder->setScissor(xx, yy
						, uint16_t(bx::min(clipRect.z, 65535.0f) - xx)
						, uint16_t(bx::min(clipRect.w, 65535.0f) - yy)
					);

					encoder->setState(state);
					encoder->setTexture(0, s_tex, th);
					encoder->setVertexBuffer(0, &tvb, cmd->VtxOffset, numVertices);
					encoder->setIndexBuffer(&tib, cmd->IdxOffset, cmd->ElemCount);
					encoder->submit(m_viewId, program);
				}
			}
		}

		bgfx::end(encoder);
	}
}

void OcornutImguiContext::create(float _fontSize, bx::AllocatorI * _allocator) noexcept
{
	IMGUI_CHECKVERSION();

	m_allocator = _allocator;

	if (NULL == _allocator)
	{
		static bx::DefaultAllocator allocator;
		m_allocator = &allocator;
	}

	m_viewId = 255;
	//m_lastScroll = 0;
	//m_last = bx::getHPCounter();

	ImGui::SetAllocatorFunctions(memAlloc, memFree, this);

	m_imgui = ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();

	//io.DisplaySize = ImVec2(1280.0f, 720.0f);
	//io.DeltaTime = 1.0f / 60.0f;
	//io.IniFilename = NULL;

	//setupStyle(true);

	io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;


	bgfx::RendererType::Enum type = bgfx::getRendererType();
	m_program = bgfx::createProgram(
		bgfx::createEmbeddedShader(s_embeddedShaders.data(), type, "vs_ocornut_imgui")
		, bgfx::createEmbeddedShader(s_embeddedShaders.data(), type, "fs_ocornut_imgui")
		, true
	);

	u_imageLodEnabled = bgfx::createUniform("u_imageLodEnabled", bgfx::UniformType::Vec4);
	m_imageProgram = bgfx::createProgram(
		bgfx::createEmbeddedShader(s_embeddedShaders.data(), type, "vs_imgui_image")
		, bgfx::createEmbeddedShader(s_embeddedShaders.data(), type, "fs_imgui_image")
		, true
	);

	m_layout
		.begin()
		.add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
		.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
		.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
		.end();

	s_tex = bgfx::createUniform("s_tex", bgfx::UniformType::Sampler);

	uint8_t* data;
	int32_t width;
	int32_t height;
	{
		ImFontConfig config;
		config.FontDataOwnedByAtlas = false;
		config.MergeMode = false;
		const ImWchar* ranges = io.Fonts->GetGlyphRangesCyrillic();
		config.MergeMode = true;
	}

	io.Fonts->GetTexDataAsRGBA32(&data, &width, &height);

	m_texture = bgfx::createTexture2D(
		(uint16_t)width
		, (uint16_t)height
		, false
		, 1
		, bgfx::TextureFormat::BGRA8
		, 0
		, bgfx::copy(data, width*height * 4)
	);

	//ImGui::InitDockContext();
}

void OcornutImguiContext::destroy() noexcept
{
	//ImGui::ShutdownDockContext();
	ImGui::DestroyContext(m_imgui);

	bgfx::destroy(s_tex);
	bgfx::destroy(m_texture);

	bgfx::destroy(u_imageLodEnabled);
	bgfx::destroy(m_imageProgram);
	bgfx::destroy(m_program);

	m_allocator = NULL;
}

void OcornutImguiContext::setupStyle(bool _dark) noexcept
{
	// Doug Binks' darl color scheme
// https://gist.github.com/dougbinks/8089b4bbaccaaf6fa204236978d165a9
	ImGuiStyle& style = ImGui::GetStyle();
	if (_dark)
	{
		ImGui::StyleColorsDark(&style);
	}
	else
	{
		ImGui::StyleColorsLight(&style);
	}

	style.FrameRounding = 4.0f;
	style.WindowBorderSize = 0.0f;
}

void OcornutImguiContext::beginFrame(int _width, int _height, bgfx::ViewId _viewId) noexcept
{
	m_viewId = _viewId;
	//ImGuiIO& io = ImGui::GetIO();
	//io.DisplaySize = ImVec2((float)_width, (float)_height);
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void OcornutImguiContext::endFrame() noexcept
{
	ImGui::Render();
	render(ImGui::GetDrawData());
}