#pragma once

#include "../EngineBase.h"
#include "../UserInterface/ShapeBase.h"

namespace Engine
{
	namespace Reflection
	{
		enum class PropertyType
		{
			Integer = 0,
			Double = 1,
			Boolean = 2,
			Color = 3,
			String = 4,
			Texture = 5,
			Font = 6,
			Application = 7,
			Dialog = 8,
			Rectangle = 9,
			Unknown = -1
		};
		struct PropertyInfo
		{
			void* Address;
			PropertyType Type;
			string Name;

			template <class V>
			void Set(const V& value)
			{
				*reinterpret_cast<V*>(Address) = value;
			}
			template <class V>
			const V& Get() const
			{
				return *reinterpret_cast<const V*>(Address);
			}
			template <class V>
			V& Get()
			{
				return *reinterpret_cast<V*>(Address);
			}
		};
		class IPropertyEnumerator
		{
		public:
			virtual void EnumerateProperty(const string& name, void* address, PropertyType type) = 0;
		};
		class GetPropertyEnumerator : public IPropertyEnumerator
		{
		public:
			PropertyInfo Result;
			GetPropertyEnumerator(const string& property_name);
			GetPropertyEnumerator(const GetPropertyEnumerator& src) = gd_delete;
			GetPropertyEnumerator& operator=(const GetPropertyEnumerator& src) = gd_delete;
			virtual void EnumerateProperty(const string& name, void* address, PropertyType type) override;
		};
		class PropertyZeroInitializer : public IPropertyEnumerator
		{
		public:
			virtual void EnumerateProperty(const string& name, void* address, PropertyType type) override;
		};
		class Reflected
		{
		public:
			virtual void EnumerateProperties(IPropertyEnumerator& enumerator);
			virtual PropertyInfo GetProperty(const string& name) = 0;
		};

#define ENGINE_REFLECTED_CLASS(class_name, parent_class)                                                                                                                           \
	class class_name : public parent_class                                                                                                                                         \
	{                                                                                                                                                                              \
	private:                                                                                                                                                                       \
		template <UInt32 __index>                                                                                                                                                    \
		void _mirror_property(::Engine::Reflection::IPropertyEnumerator& enumerator)                                                                                               \
		{                                                                                                                                                                          \
		}                                                                                                                                                                          \
                                                                                                                                                                                   \
	public:                                                                                                                                                                        \
		virtual void EnumerateProperties(::Engine::Reflection::IPropertyEnumerator& enumerator) override                                                                           \
		{                                                                                                                                                                          \
			parent_class::EnumerateProperties(enumerator);                                                                                                                         \
			_mirror_property<__COUNTER__ + 1>(enumerator);                                                                                                                         \
		}                                                                                                                                                                          \
                                                                                                                                                                                   \
	public:                                                                                                                                                                        \
		virtual ::Engine::Reflection::PropertyInfo GetProperty(const string& name) override                                                                                        \
		{                                                                                                                                                                          \
			::Engine::Reflection::GetPropertyEnumerator enumerator(name);                                                                                                          \
			EnumerateProperties(enumerator);                                                                                                                                       \
			return enumerator.Result;                                                                                                                                              \
		}

#define ENGINE_END_REFLECTED_CLASS                                                                                                                                                 \
	}                                                                                                                                                                              \
	;

#define ENGINE_DEFINE_REFLECTED_PROPERTY(reflected_type, property_name)                                                                                                            \
private:                                                                                                                                                                           \
	template <>                                                                                                                                                                    \
	void _mirror_property<__COUNTER__>(::Engine::Reflection::IPropertyEnumerator & enumerator)                                                                                     \
	{                                                                                                                                                                              \
		enumerator.EnumerateProperty(ENGINE_STRING(property_name), &property_name, ENGINE_REFLECTED_TYPE_##reflected_type);                                                        \
		_mirror_property<__COUNTER__ + 1>(enumerator);                                                                                                                             \
	}                                                                                                                                                                              \
                                                                                                                                                                                   \
public:                                                                                                                                                                            \
	ENGINE_CXX_TYPE_##reflected_type property_name;

#define ENGINE_STRING(macro) L#macro

#define ENGINE_CXX_TYPE_INTEGER Int32
#define ENGINE_CXX_TYPE_DOUBLE Float64
#define ENGINE_CXX_TYPE_BOOLEAN bool
#define ENGINE_CXX_TYPE_COLOR ::Engine::UI::Color
#define ENGINE_CXX_TYPE_STRING ::Engine::ImmutableString
#define ENGINE_CXX_TYPE_TEXTURE ::Engine::SafePointer<::Engine::UI::ITexture>
#define ENGINE_CXX_TYPE_FONT ::Engine::SafePointer<::Engine::UI::IFont>
#define ENGINE_CXX_TYPE_APPLICATION ::Engine::SafePointer<::Engine::UI::Template::Shape>
#define ENGINE_CXX_TYPE_DIALOG ::Engine::SafePointer<::Engine::UI::Template::ControlTemplate>
#define ENGINE_CXX_TYPE_RECTANGLE ::Engine::UI::Rectangle
#define ENGINE_REFLECTED_TYPE_INTEGER ::Engine::Reflection::PropertyType::Integer
#define ENGINE_REFLECTED_TYPE_DOUBLE ::Engine::Reflection::PropertyType::Double
#define ENGINE_REFLECTED_TYPE_BOOLEAN ::Engine::Reflection::PropertyType::Boolean
#define ENGINE_REFLECTED_TYPE_COLOR ::Engine::Reflection::PropertyType::Color
#define ENGINE_REFLECTED_TYPE_STRING ::Engine::Reflection::PropertyType::String
#define ENGINE_REFLECTED_TYPE_TEXTURE ::Engine::Reflection::PropertyType::Texture
#define ENGINE_REFLECTED_TYPE_FONT ::Engine::Reflection::PropertyType::Font
#define ENGINE_REFLECTED_TYPE_APPLICATION ::Engine::Reflection::PropertyType::Application
#define ENGINE_REFLECTED_TYPE_DIALOG ::Engine::Reflection::PropertyType::Dialog
#define ENGINE_REFLECTED_TYPE_RECTANGLE ::Engine::Reflection::PropertyType::Rectangle
	}
}
