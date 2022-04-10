#include "Util.hpp"

namespace ks
{
	std::string shaderDir() noexcept
	{
		std::string AppDir = fd::Application::getAppDir();
		std::string path = AppDir + "/" + "Shader";
		return path;
	}

	ShaderPaire getShaderPaire(const std::string& name) noexcept
	{
		ShaderPaire p;
		p.vertexPath = shaderDir() + "/" + name + ".vert";
		p.fragmentPath = shaderDir() + "/" + name + ".frag";
		return p;
	}

	Rect getUnionRect(const std::vector<ks::Image>& images) noexcept
	{
		Rect rect;
		for (const auto& image : images)
		{
			rect = rect.unionWithOther(image.getRect());
		}
		return rect;
	}

	Rect getUnionRect(const std::vector<std::shared_ptr<ks::Image>>& images) noexcept
	{
		Rect rect;
		for (const auto& image : images)
		{
			rect = rect.unionWithOther(image->getRect());
		}
		return rect;
	}
}