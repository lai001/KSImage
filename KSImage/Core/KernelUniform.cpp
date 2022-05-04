#include "KernelUniform.hpp"

namespace ks
{
	bgfx::UniformType::Enum getBgfxType(const KernelUniform::ValueType & type) noexcept
	{
		std::unordered_map<KernelUniform::ValueType, bgfx::UniformType::Enum> dic;
		dic[KernelUniform::ValueType::f32] = bgfx::UniformType::Enum::Vec4;
		dic[KernelUniform::ValueType::vec2] = bgfx::UniformType::Enum::Vec4;
		dic[KernelUniform::ValueType::vec3] = bgfx::UniformType::Enum::Vec4;
		dic[KernelUniform::ValueType::vec4] = bgfx::UniformType::Enum::Vec4;
		dic[KernelUniform::ValueType::mat3] = bgfx::UniformType::Enum::Mat3;
		dic[KernelUniform::ValueType::mat4] = bgfx::UniformType::Enum::Mat4;
		dic[KernelUniform::ValueType::texture2d] = bgfx::UniformType::Enum::Sampler;
		assert(dic.find(type) != dic.end());
		return dic[type];
	}

	KernelUniform::KernelUniform(const std::string & name, const ValueType & type)
		: name(name), type(type), u_handle(bgfx::createUniform(name.c_str(), getBgfxType(type), 1))
	{
		assert(bgfx::isValid(u_handle));
	}

	KernelUniform::KernelUniform(const Info& Info)
		:name(Info.name), type(Info.type), u_handle(bgfx::createUniform(Info.name.c_str(), getBgfxType(Info.type), 1))
	{
		assert(bgfx::isValid(u_handle));
	}

	KernelUniform::~KernelUniform()
	{
		assert(bgfx::isValid(u_handle));
		bgfx::destroy(u_handle);
	}

	void KernelUniform::setValue(const Value& value) noexcept
	{
		assert(bgfx::isValid(u_handle));
		assert(value.type != ks::KernelUniform::ValueType::texture2d);
		assert(value.type == this->type);

		Value tmpValue = value;
		
		switch (value.type)
		{
		case KernelUniform::ValueType::f32:
			tmpValue.vec4 = glm::vec4(value.f32);
			break;

		case KernelUniform::ValueType::vec2:
			tmpValue.vec4 = glm::vec4(value.vec2, 0.0, 0.0);
			break;

		case KernelUniform::ValueType::vec3:
			tmpValue.vec4 = glm::vec4(value.vec3, 0.0);
			break;

		case KernelUniform::ValueType::vec4:
			tmpValue.vec4 = value.vec4;
			break;

		case KernelUniform::ValueType::mat3:
			tmpValue.mat3 = value.mat3;
			break;

		case KernelUniform::ValueType::mat4:
			tmpValue.mat4 = value.mat4;
			break;

		default:
			break;
		}

		bgfx::setUniform(u_handle, tmpValue.getData(), 1);
	}

	std::string KernelUniform::getName() const noexcept
	{
		return name;
	}

	ks::KernelUniform::ValueType KernelUniform::getType() const noexcept
	{
		return type;
	}

	bgfx::UniformHandle KernelUniform::getHandle() const noexcept
	{
		return u_handle;
	}
}