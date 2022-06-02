#include "KernelUniform.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <KSRenderEngine/KSRenderEngine.hpp>

namespace ks
{
	KernelUniform::Info::Info(const std::string & name, const ValueType type)
		:name(name), type(type)
	{

	}
}

namespace ks
{
	KernelUniform::Value::Value(float v) : type(ValueType::f32), f32(v) { }
	KernelUniform::Value::Value(glm::vec2 v) : type(ValueType::vec2), vec2(v) { }
	KernelUniform::Value::Value(glm::vec3 v) : type(ValueType::vec3), vec3(v) { }
	KernelUniform::Value::Value(glm::vec4 v) : type(ValueType::vec4), vec4(v) { }
	KernelUniform::Value::Value(glm::mat3 v) : type(ValueType::mat3), mat3(v) { }
	KernelUniform::Value::Value(glm::mat4 v) : type(ValueType::mat4), mat4(v) { }
	KernelUniform::Value::Value(ks::Image* v) : type(ValueType::texture2d), texture2d(v) { }

	const void * KernelUniform::Value::getData() const noexcept
	{
		return &this->f32;
	}
}