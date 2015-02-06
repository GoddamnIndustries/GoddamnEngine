/// ==========================================================================================
/// Reflection.h: Common interface for RTTI/Introspection/Reflection.
/// Copyright (C) Goddamn Industries 2011 - Present. All Rights Reserved.
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_OBJECT_REFLECTION
#define GD_CORE_OBJECT_REFLECTION

#include <GoddamnEngine/Include.h>

#include <GoddamnEngine/Core/Object/Reflection-Base.h>

#include <GoddamnEngine/Core/Containers/Containers.h>
#include <GoddamnEngine/Core/Containers/String.h>
#include <GoddamnEngine/Core/Containers/List.h>
#include <GoddamnEngine/Core/Containers/Map.h>

#include <GoddamnEngine/Core/Templates/UniquePtr.h>
#include <GoddamnEngine/Core/Templates/Delegate.h>

GD_NAMESPACE_BEGIN

	class InputStream;
	class OutputStream;
	class Assembly;

	/// @brief RTTI/Reflection/Introspection manager class.
	class Reflection final
	{
	private:
		UniquePtr<HashMap<ITypeInfo*>> static RegisteredTypes;
		UniquePtr<LinkedList<Assembly*>> static RegisteredAssembiles;

	public:

		/// @brief Registers a new type information. This method should not been invoked directly,
		///        Use 'GD_CLASSINFO_IMPLEMENTATION' macro instead. Note, that this method 
		///        is marked as 'internal', so each assembly should contain it's own implementation
		///        of this method. Use 'GD_CLASSINFO_ASSEMBLY_DEFINITION' macro to provide 
		///	       the generic implementation of this function.
		/// @param TypeInfo Type information that would be registered.
		GDAPI static void RegisterTypeInfo(ITypeInfo* const TypeInfo);

		/// @brief Unregisters existing type information. This method should not been invoked directly,
		///        Use 'GD_CLASSINFO_IMPLEMENTATION' macro instead. Note, that this method 
		///        is marked as 'internal', so each assembly should contain it's own implementation
		///        of this method. Use 'GD_CLASSINFO_ASSEMBLY_DEFINITION' macro to provide 
		///	       the generic implementation of this function.
		/// @param TypeInfo Type information that would be unregistered.
		GDAPI static void UnregisterTypeInfo(ITypeInfo const* const TypeInfo);

		GDINT ITypeInfo const static* const FindType(String const& TypeName);

	};	// class Reflection
	
	// ==========================================================================================
	// ITypeInfo ...
	// ==========================================================================================
	
	#pragma region ITypeInfo

	/// @brief Returns type information for specified type or nullptr if type was not registered.
	/// @tparam Type Type for which information would be queried.
	/// @returns Type information for specified type or nullptr if type was not registered.
	template<class Type>
	GDINT ITypeInfo const* TypeOf()
	{
		return nullptr;
	}

	/// @brief 'Class' of type info.
	enum class TypeInfoClass : UInt8
	{
		BoolType,			///< @brief Boolean types.
		NumericType,		///< @brief Integral and floating-point types.
		EnumType,			///< @brief Enumeration types.
		PointerType,		///< @brief Pointer types.
		ClassType,			///< @brief Classes and structures.
		ContainerType,		///< @brief Container types.
	};	// enum class TypeInfoClass

	/// @brief 'Class' of numeric type info.
	enum class NumericTypeInfoClass : UInt8
	{
		IntegralType,		///< @brief Integral types.
		FloatingPointType,	///< @brief Floating-point types.
	};	// enum class NumericTypeInfoClass

	/// @brief Stores information about types.
	class ITypeInfo
	{
	protected:

		/// @brief Initializes a new type information and registers it in the assembly.
		GDINL ITypeInfo() 
		{ 
			Reflection::RegisterTypeInfo(this); 
		} 
			
		/// @brief Deinitializes a new type information and unregisters it from the assembly.
		GDINL virtual ~ITypeInfo() 
		{ 
			Reflection::UnregisterTypeInfo(this); 
		} 

	public:

		/// @brief Returns the name of the current type.
		/// @returns The name of the current type.
		GDAPI virtual String const& GetName() const abstract;

		/// @brief Returns 'class' of this type information.
		/// @returns 'class' of this type information.
		GDAPI virtual TypeInfoClass GetClass() const abstract;

		/// @brief Returns true, is this type is Bool type.
		/// @returns True, is this type is Bool type.
		GDINL bool IsNumericType() const
		{
			return this->GetClass() == TypeInfoClass::NumericType;
		}

		/// @brief Returns true, is this type is Enum type.
		/// @returns True, is this type is Enum type.
		GDINL bool IsEnumType() const
		{
			return this->GetClass() == TypeInfoClass::EnumType;
		}

		/// @brief Returns true, is this type is Class type.
		/// @returns True, is this type is Class type.
		GDINL bool IsClassType() const
		{
			return this->GetClass() == TypeInfoClass::ClassType;
		}

		/// @brief Returns true, is this type is Container type.
		/// @returns True, is this type is Container type.
		GDINL bool IsContainerType() const
		{
			return this->GetClass() == TypeInfoClass::ContainerType;
		}

		/// @brief Returns size of instance of this type.
		/// @returns Size of instance of this type.
		GDAPI virtual size_t GetSize() const abstract;

		/// @brief Returns true if objects of this type can be serialized and deserialized.
		/// @returns True if objects of this type can be serialized, false otherwise.
		GDAPI virtual bool IsSerializable() const
		{
			return false;
		}

		/// @brief Serializes specified object.
		/// @param Pointer Pointer to object that would be serialized.
		/// @param Output Stream to which data would be written.
		GDAPI virtual void Serialize(CHandle const Pointer, OutputStream* const Output) const
		{
			GD_NOT_USED(Pointer);
			GD_NOT_USED(Output);
			GD_NOT_SUPPORTED();
		}

		/// @brief Deserializes specified object.
		/// @param Pointer Pointer to object that would be deserialized.
		/// @param Input Stream from which data would be read.
		GDAPI virtual void Deserialize(Handle const Pointer, InputStream* const Input) const
		{
			GD_NOT_USED(Pointer);
			GD_NOT_USED(Input);
			GD_NOT_SUPPORTED();
		}

	};	// class ITypeInfo

	/// @brief Stores pre-classified information about types.
	template<TypeInfoClass const TheTypeInfoClass>
	class IClassifiedTypeInfo : public ITypeInfo
	{
	public:

		/// @brief Returns 'class' of this type information.
		/// @returns 'class' of this type information.
		GDAPI virtual TypeInfoClass GetClass() const override final
		{
			return TheTypeInfoClass;
		}

	};	// class IClassifiedTypeInfo<TypeInfoClass>

	// ==========================================================================================
	// Boolean TypeInfo ...
	// ==========================================================================================

	/// @brief Stores information about boolean types.
	class IBoolInfo : public IClassifiedTypeInfo<TypeInfoClass::BoolType>
	{
	public:

		/// @brief Returns true if objects of this type can be serialized and deserialized.
		/// @returns True if objects of this type can be serialized, false otherwise.
		GDAPI virtual bool IsSerializable() const override final
		{
			return true;
		}

		/// @brief Serializes specified object.
		/// @param Pointer Pointer to object that would be serialized.
		/// @param Output Stream to which data would be written.
		GDAPI virtual void Serialize(CHandle const Pointer, OutputStream* const Output) const override final;

		/// @brief Deserializes specified object.
		/// @param Pointer Pointer to object that would be deserialized.
		/// @param Input Stream from which data would be read.
		GDAPI virtual void Deserialize(Handle const Pointer, InputStream* const Input) const override final;

	};	// class INumericType

	template<>
	GDAPI extern ITypeInfo const* TypeOf<bool>();

	// ==========================================================================================
	// Numeric TypeInfo ...
	// ==========================================================================================

	/// @brief Stores information about numeric types.
	class INumericType : public IClassifiedTypeInfo<TypeInfoClass::NumericType>
	{
	public:

		/// @brief Returns numeric 'class' of this type information.
		/// @returns Numeric 'class' of this type information.
		GDAPI virtual NumericTypeInfoClass GetNumericClass() const abstract;

		/// @brief Returns true if this is Integral Numeric Type.
		/// @returns True if this is Integral Numeric Type.
		GDAPI bool IsIntegral() const
		{
			return this->GetNumericClass() == NumericTypeInfoClass::IntegralType;
		}

		/// @brief Returns true if this is Floating-point Numeric Type.
		/// @returns True if this is Floating-point Numeric Type.
		GDAPI bool IsFloatingPoint() const
		{
			return this->GetNumericClass() == NumericTypeInfoClass::FloatingPointType;
		}

		/// @brief Returns true if objects of this type can be serialized and deserialized.
		/// @returns True if objects of this type can be serialized, false otherwise.
		GDAPI virtual bool IsSerializable() const override final
		{
			return true;
		}

		/// @brief Serializes specified object.
		/// @param Pointer Pointer to object that would be serialized.
		/// @param Output Stream to which data would be written.
		GDAPI virtual void Serialize(CHandle const Pointer, OutputStream* const Output) const override final;

		/// @brief Deserializes specified object.
		/// @param Pointer Pointer to object that would be deserialized.
		/// @param Input Stream from which data would be read.
		GDAPI virtual void Deserialize(Handle const Pointer, InputStream* const Input) const override final;

	};	// class INumericType

	template<>
	GDAPI extern ITypeInfo const* TypeOf<Int8>();

	template<>
	GDAPI extern ITypeInfo const* TypeOf<UInt8>();

	template<>
	GDAPI extern ITypeInfo const* TypeOf<Int16>();

	template<>
	GDAPI extern ITypeInfo const* TypeOf<UInt16>();

	template<>
	GDAPI extern ITypeInfo const* TypeOf<Int32>();

	template<>
	GDAPI extern ITypeInfo const* TypeOf<UInt32>();

	template<>
	GDAPI extern ITypeInfo const* TypeOf<Int64>();

	template<>
	GDAPI extern ITypeInfo const* TypeOf<UInt64>();

	template<>
	GDAPI extern ITypeInfo const* TypeOf<Float32>();

	template<>
	GDAPI extern ITypeInfo const* TypeOf<Float64>();

	// ==========================================================================================
	// Enumeration TypeInfo ...
	// ==========================================================================================

	/// @brief Collection that stores mapped enum elements.
	typedef Map<UInt64, String> EnumMap;

	/// @brief Stores information about numeric types.
	class IEnumInfo : public IClassifiedTypeInfo<TypeInfoClass::EnumType>
	{
	public:

		/// @brief Returns base type of this enumeration.
		/// @returns Base type of this enumeration.
		GDAPI virtual ITypeInfo const* GetBaseType() const abstract;

		/// @brief Returns mapped values of this enumeration.
		/// @returns Mapped values of this enumeration.
		GDAPI virtual EnumMap const& GetValues() const abstract;

	};	// class IEnumInfo

	/*template<typename EnumType>
	GDINL ITypeInfo const* TypeOf<EnableIf<TypeTraits::IsBase<IEnum, EnumType>::Value, EnumType>::Type>()
	{
	return EnumType::GetEnumInfo();
	}*/

	// ==========================================================================================
	// Pointer TypeInfo ...
	// ==========================================================================================

	/// @brief Stores information about pointer types.
	class IPointerInfo : public IClassifiedTypeInfo<TypeInfoClass::PointerType>
	{
	public:

		/// @brief Returns the type beyond the pointer.
		/// @returns The type beyond the pointer.
		GDAPI virtual ITypeInfo const* GetReferencedType() const abstract;

	};	// class INumericType

//	template<typename PointerType>
//	GDINL ITypeInfo const* TypeOf<PointerType*>()
//	{
//		/// @brief Contains specialized pointer information.
//		class PointerInfo final : public IPointerInfo
//		{
//		public:
//
//			/// @brief Returns the type beyond the pointer.
//			/// @returns The type beyond the pointer.
//			GDAPI virtual ITypeInfo const* GetReferencedType() const override final
//			{
//				return TypeOf<PointerType>();
//			}
//
//		} static const PointerInfoInstance;
//
//		return &PointerInfoInstance;
//	}

	// ==========================================================================================
	// Class/Structure TypeInfo ...
	// ==========================================================================================

	typedef Vector<IFieldInfo const*> FieldsVector;
	typedef Vector<IMethodInfo const*> MethodsVector;

	/// @brief Represents a type of collectible member information.
	enum class MemberType : UInt8
	{
		Field,
		Method,
	};	// enum class MemberType

	/// @brief Provides information about classes.
	class IClassInfo : public IClassifiedTypeInfo<TypeInfoClass::ClassType>
	{
	private:
		friend class Reflection;
		Assembly const* HostAssembly = nullptr;

	public:

		/// @brief Returns assembly in which this type is defined.
		/// @returns Assembly in which this type is defined.
		GDINL Assembly const* GetAssembly() const
		{
			return this->HostAssembly;
		}

		/// @brief Returns type information for base type of this class
		/// @returns Type information for base type of this class
		GDAPI virtual ITypeInfo const* GetBaseType() const abstract;

		/// @brief Returns collection of public fields in type.
		/// @returns Collection of public fields in type.
		GDAPI virtual FieldsVector const& GetFields() const abstract;

		/// @brief Returns collection of public methods in type.
		/// @returns Collection of public methods in type.
		GDAPI virtual MethodsVector const& GetMethods() const abstract;

		/// @brief Checks if this type is derived from specified.
		/// @param BaseClassInfo Possible base type.
		/// @returns True if this type is derived from specified, false otherwise.
		GDAPI bool IsDerivedFrom(IClassInfo const* BaseClassInfo) const;

	};	// class IClassInfo

//	template<typename ClassType>
//	GDINL ITypeInfo const* TypeOf<EnableIf<TypeTraits::IsBase<IClass, ClassType>::Value, ClassType>::Type>()
//	{
//		return ClassType::GetTypeClassInfo();
//	}

	#pragma endregion

	/// @brief Provides information about field.
	class IFieldInfo
	{
	public:

		/// @brief Returns the name of the current field.
		/// @returns The name of the current field.
		GDAPI virtual String const& GetName() const abstract;
	
		/// @brief Returns type information of field.
		/// @returns Type information of field.
		GDAPI virtual ITypeInfo const* GetFieldType() const abstract;

		/// @brief Returns type information of class that owns this field.
		/// @returns Type information of class that owns this field.
		GDAPI virtual ITypeInfo const* GetOwnerType() const abstract;

		/// @brief Returns true if this field is static.
		/// @returns True if this field is static.
		GDAPI virtual bool IsStatic() const abstract;

		/// @brief Returns true if this field is read-only (it`s definition contains const qualifier).
		/// @returns True if this field is read-only (it`s definition contains const qualifier).
		GDAPI virtual bool IsReadOnly() const abstract;

		/// @brief Returns pointer on on the field of specific object.
		/// @param Reflectable Object for which pointer would be queried.
		/// @returns Pointer on on the field of specific object.
		GDAPI virtual CHandle GetPointer(CHandle const Reflectable) const abstract;

		/// @brief Copies value of the member to output.
		/// @param Reflectable Reflectable object we are operating on.
		/// @param OutputValuePtr Here would be copied value of the member.
		GDAPI virtual void GetValue(CHandle const Reflectable, Handle const OutputValuePtr) const abstract;

		/// @brief Copies value of input into member.
		/// @param Reflectable Reflectable object we are operating on.
		/// @param InputValuePtr New value of the member would be copied out of here.
		GDAPI virtual void SetValue(Handle const Reflectable, Handle const InputValuePtr) const
		{
			GD_NOT_USED(Reflectable);
			GD_NOT_USED(InputValuePtr);
			GD_NOT_SUPPORTED();
		}

	};	// class IFieldInfo

	/// @brief Provides information about method.
	class IMethodInfo
	{
	public:

		/// @brief Returns type information for class that contains this method.
		/// @returns Type information for class that contains this method.
		GDAPI virtual ITypeInfo const* GetOwnerType() const abstract;

		/// @brief Returns type information for method return type.
		/// @returns Type information for method return type.
		GDAPI virtual ITypeInfo const* GetReturnType() const abstract;

		/// @brief Returns list or type information for method arguments list.
		/// @returns List or type information for method arguments list.
		GDAPI virtual Vector<ITypeInfo const*> const& GetArgumentsTypes() const abstract;

		/// @brief Returns the name of the current field.
		/// @returns The name of the current field.
		GDAPI virtual String const& GetName() const abstract;
		
		/// @brief Returns true if this method was defined as static.
		/// @returns True if this method was defined as static.
		GDAPI virtual bool IsStatic() const abstract;

		/// @brief Returns true if this methods does not modifies instance that invokes it.
		/// @returns True if this methods does not modifies instance that invokes it.
		GDAPI virtual bool IsConst() const abstract;

		/// @brief Invokes method for Instance and copies return result to output with arguments specified as variable arguments list.
		/// @param Reflectable Reflectable object we are operating on.
		/// @param ReturnValueOutputPtr Pointer to the methods execution result. Null pointer may be specified for voids.
		GDAPI virtual void Invoke(IClass* const Instance, Handle const ReturnValueOutputPtr, ...) const abstract;

	};	// class IMethodInfo

GD_NAMESPACE_END

// ==========================================================================================
// Common type definitions.
// ==========================================================================================

#pragma region CommonTypeDefinitions

GD_NAMESPACE_BEGIN

	

GD_NAMESPACE_END

#pragma endregion

#include <GoddamnEngine/Core/Object/Reflection-Private.h>
#endif	// ifndef GD_CORE_OBJECT_REFLECTION
