#include "ImageVertex.hpp"
#include "CreateVertext.hpp"

namespace ks
{
	bgfx::VertexLayout ImageVertex::vertexLayout = createVertextLayout([](bgfx::VertexLayout& layout) {
		layout
		.begin()
		.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
		.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
		.end();
	});
}