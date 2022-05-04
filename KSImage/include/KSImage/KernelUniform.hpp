#ifndef KERNELUNIFORM_HPP
#define KERNELUNIFORM_HPP

#include <Foundation/Foundation.hpp>
#include <bgfx/bgfx.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Image.hpp"
#include "defs.hpp"

namespace ks
{
	class KSImage_API KernelUniform : public boost::noncopyable
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
				std::shared_ptr<ks::Image> texture2d = std::shared_ptr<ks::Image>(nullptr);
			};
			Value()
			{

			}
			~Value()
			{

			}

			Value& operator=(const Value & value)
			{
				Value v;
				v.type = value.type;
				switch (value.type)
				{
				case ValueType::f32:
					v.f32 = value.f32;
					break;

				case ValueType::vec2:
					v.vec2 = value.vec2;
					break;

				case ValueType::vec3:
					v.vec3 = value.vec3;
					break;

				case ValueType::vec4:
					v.vec4 = value.vec4;
					break;

				case ValueType::mat3:
					v.mat3 = value.mat3;
					break;

				case ValueType::mat4:
					v.mat4 = value.mat4;
					break;

				case ValueType::texture2d:
					v.texture2d = value.texture2d;
					break;

				default:
					break;
				}
				return v;
			}

			Value(const Value& value)
			{
				this->type = value.type;
				switch (value.type)
				{
				case ValueType::f32:
					this->f32 = value.f32;
					break;

				case ValueType::vec2:
					this->vec2 = value.vec2;
					break;

				case ValueType::vec3:
					this->vec3 = value.vec3;
					break;

				case ValueType::vec4:
					this->vec4 = value.vec4;
					break;


				case ValueType::mat3:
					this->mat3 = value.mat3;
					break;

				case ValueType::mat4:
					this->mat4 = value.mat4;
					break;

				case ValueType::texture2d:
					this->texture2d = value.texture2d;
					break;

				default:
					break;
				}
			}

			Value(float v)							:type(ValueType::f32),			f32(v) { }
			Value(glm::vec2 v)						:type(ValueType::vec2),			vec2(v) { }
			Value(glm::vec3 v)						:type(ValueType::vec3),			vec3(v) { }
			Value(glm::vec4 v)						:type(ValueType::vec4),			vec4(v) { }
			Value(glm::mat3 v)						:type(ValueType::mat3),			mat3(v) { }
			Value(glm::mat4 v)						:type(ValueType::mat4),			mat4(v) { }
			Value(std::shared_ptr<ks::Image> v)		:type(ValueType::texture2d),	texture2d(v) { }
		
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
		bgfx::UniformHandle u_handle = BGFX_INVALID_HANDLE;

	public:
		KernelUniform(const std::string& name, const ValueType& type);
		KernelUniform(const Info& Info);
		~KernelUniform();
		void setValue(const Value& value) noexcept;
		std::string getName() const noexcept;
		ks::KernelUniform::ValueType getType() const noexcept;
		bgfx::UniformHandle getHandle() const noexcept;
	};
}

#endif // !KERNELUNIFORM_HPP