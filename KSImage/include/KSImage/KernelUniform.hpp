#ifndef KSImage_KernelUniform_hpp
#define KSImage_KernelUniform_hpp

#include <Foundation/Foundation.hpp>
#include <KSRenderEngine/KSRenderEngine.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Image.hpp"
#include "defs.hpp"

namespace ks
{
	class KSImage_API KernelUniform : public noncopyable
	{
	public:
		enum class ValueType
		{
			f32,
			vec2,
			vec3,
			vec4,
			mat3,
			mat4,
			texture2d,
		};

		struct Value
		{
		public:
			ValueType type;
			union
			{
				float f32;
				glm::vec2 vec2;
				glm::vec3 vec3;
				glm::vec4 vec4;
				glm::mat3 mat3;
				glm::mat4 mat4;
				ks::Image* texture2d = nullptr;
			};

			Value(float v)						:type(ValueType::f32),			f32(v) { }
			Value(glm::vec2 v)					:type(ValueType::vec2),			vec2(v) { }
			Value(glm::vec3 v)					:type(ValueType::vec3),			vec3(v) { }
			Value(glm::vec4 v)					:type(ValueType::vec4),			vec4(v) { }
			Value(glm::mat3 v)					:type(ValueType::mat3),			mat3(v) { }
			Value(glm::mat4 v)					:type(ValueType::mat4),			mat4(v) { }
			Value(ks::Image* v) :type(ValueType::texture2d),	texture2d(v) { }
		
			const void* getData() const noexcept
			{
				return &this->f32;
			}
		};

		struct Info
		{
			std::string name;
			ValueType type;
			explicit Info(const std::string& name, const ValueType type) :name(name), type(type) { }
		};

	private:
		std::string name;
		ValueType type;

	public:
		KernelUniform(const std::string& name, const ValueType& type);
		KernelUniform(const Info& Info);
		~KernelUniform();
		void setValue(const Value& value) noexcept;
		std::string getName() const noexcept;
		ks::KernelUniform::ValueType getType() const noexcept;
	};
}

#endif // !KSImage_KernelUniform_hpp