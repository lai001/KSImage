#include "KernelUniform.hpp"

namespace ks
{
	KernelUniform::KernelUniform(const std::string & name, const ValueType & type)
		: name(name), type(type)
	{
	}

	KernelUniform::KernelUniform(const Info& Info)
		:name(Info.name), type(Info.type)
	{
	}

	KernelUniform::~KernelUniform()
	{
	}

	void KernelUniform::setValue(const Value& value) noexcept
	{
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

	}

	std::string KernelUniform::getName() const noexcept
	{
		return name;
	}

	ks::KernelUniform::ValueType KernelUniform::getType() const noexcept
	{
		return type;
	}
}