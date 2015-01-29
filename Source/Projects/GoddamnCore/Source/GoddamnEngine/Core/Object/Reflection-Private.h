/// ==========================================================================================
/// Reflection-Private.h: Common interface for RTTI/Introspection/Reflection implementation.
/// Copyright (C) Goddamn Industries 2011 - Present. All Rights Reserved.
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_OBJECT_REFLECTION_PRIVATE
#define GD_CORE_OBJECT_REFLECTION_PRIVATE

// Normally, this header does not requires WinAPI, but we include it and undefine conflict
// macros to make sure that if this header would be included in future, it would not cause
// compilation errors.
#if (defined(GD_PLATFORM_API_WINAPI))
#	include <Windows.h>
#	undef GetClassInfo
#endif	// if (defined(GD_PLATFORM_API_WINAPI))

#if (!defined(GD_REFLINFO_DUMMY))

GD_NAMESPACE_BEGIN
	
	// ==========================================================================================
	// IFieldInfo ...
	// ==========================================================================================

	#pragma region IFieldInfo
		
	/// @brief Contains some basic interface implementations that does not require to be exported
	namespace FieldInfoDetails
	{
		/// @brief Represents basic information about fields.
		/// @tparam FieldOwnerClass Type of the owner class.
		/// @tparam FieldTypeClass Type of the field.
		template<typename FieldOwnerClass, typename FieldTypeClass>
		class BasicFieldInfo : public IFieldInfo
		{	
		private:
			GD_CLASS_UNASSIGNABLE(BasicFieldInfo);
			GD_CLASS_UNCOPIABLE(BasicFieldInfo);

		protected:
			GDINL BasicFieldInfo()
			{
			}

		public:
			typedef typename TypeTraits::RemoveConst<FieldTypeClass>::Type FieldTypeClassNonConst;

			/// @brief Returns type information of field.
			/// @returns Type information of field.
			GDINL virtual ITypeInfo const* GetFieldType() const override final
			{ 
				return TypeOf<FieldTypeClass>(); 
			}

			/// @brief Returns type information of class that owns this field.
			/// @returns Type information of class that owns this field.
			GDINL virtual ITypeInfo const* GetOwnerType() const override final
			{
				return TypeOf<FieldOwnerClass>();
			}

		};	// class BasicFieldInfo

		/// @brief Represents basic information about READABLE fields.
		/// @tparam FieldSignatureType Type of field signature.
		template<typename FieldSignatureType> 
		class FieldInfoReadable;

		/// @brief STATIC POINTER Specialization.
		/// @tparam FieldValueType Type of type of the static pointer.
		/// @todo Class does not consider owner class of the static pointer.
		template<typename FieldValueType>
		class FieldInfoReadable<FieldValueType*> : public BasicFieldInfo<nullptr_t, FieldValueType>
		{
		private:
			typedef BasicFieldInfo<nullptr_t, FieldValueType> BaseType;
			typedef FieldValueType* FieldPtrType;
			FieldPtrType const FieldPtr = nullptr;

		public:
			/// @brief Initializes a field information.
			/// @param FieldPtr Pointer to field.
			GDINL FieldInfoReadable(FieldPtrType const FieldPtr) 
				: FieldPtr(FieldPtr) 
			{
			}

			/// @brief Returns true if this field is static.
			/// @returns True if this field is static.
			GDINL virtual bool IsStatic() const override final 
			{ 
				return true; 
			}

			/// @brief Returns pointer on on the field of specific object.
			/// @param Reflectable Object for which pointer would be queried.
			/// @returns Pointer on on the field of specific object.
			GDINL virtual chandle GetPointer(chandle const Reflectable) const override final
			{
				GD_NOT_USED(Reflectable);
				return FieldPtr;
			}

			/// @brief Copies value of the member to output.
			/// @param Reflectable Reflectable object we are operating on.
			/// @param OutputValuePtr Here would be copied value of the member.
			GDINL virtual void GetValue(chandle const Reflectable, handle const OutputValuePtr) const override final
			{
				GD_NOT_USED(Reflectable);
				typedef typename BaseType::FieldTypeClassNonConst FieldTypeClassNonConst;
				FieldTypeClassNonConst& OutputValueReference = *(reinterpret_cast<FieldTypeClassNonConst*>(OutputValuePtr));
				OutputValueReference = Forward<FieldTypeClassNonConst>(const_cast<FieldTypeClassNonConst&>(*this->FieldPtr));
			}
		};	// class FieldInfoReadable<FieldValueType*>

		/// @brief MEMBER POINTER Specialization
		template<typename FieldOwnerClass, typename FieldValueType>
		class FieldInfoReadable<FieldValueType FieldOwnerClass::*> : public BasicFieldInfo<FieldOwnerClass, FieldValueType>
		{
		protected:
            typedef BasicFieldInfo<FieldOwnerClass, FieldValueType> BaseType;
			typedef FieldValueType FieldOwnerClass::* FieldPtrType;
			FieldPtrType const FieldPtr = nullptr;

		public:
			/// @brief Initializes a field information.
			/// @param FieldPtr Pointer to field.
			GDINL FieldInfoReadable(FieldPtrType const FieldPtr)
				: FieldPtr(FieldPtr) 
			{
			}

			/// @brief Returns true if this field is static.
			/// @returns True if this field is static.
			GDINL virtual bool IsStatic() const override final 
			{ 
				return false; 
			}

			/// @brief Returns pointer on on the field of specific object.
			/// @param Reflectable Object for which pointer would be queried.
			/// @returns Pointer on on the field of specific object.
			GDINL virtual chandle GetPointer(chandle const Reflectable) const override final
			{
				return &(object_cast<FieldOwnerClass const*>(Reflectable)->*this->FieldPtr);
			}

			/// @brief Copies value of the member to output.
			/// @param Reflectable Reflectable object we are operating on.
			/// @param OutputValuePtr Here would be copied value of the member.
			GDINL virtual void GetValue(chandle const Reflectable, handle const OutputValuePtr) const override final
			{
                typedef typename BaseType::FieldTypeClassNonConst FieldTypeClassNonConst;
				FieldTypeClassNonConst& OutputValueReference = *(reinterpret_cast<FieldTypeClassNonConst*>(OutputValuePtr));
				OutputValueReference = Forward<FieldTypeClassNonConst>(const_cast<FieldTypeClassNonConst&>(object_cast<FieldOwnerClass const*>(Reflectable)->*this->FieldPtr));
			}
		};	// class FieldInfoReadable<FieldValueType*>

		template<typename FieldSignatureType> 
		class FieldInfoWritable;

		/// @brief STATIC FIELD READ/WRITE Specialization
		template<typename FieldValueType>
		class FieldInfoWritable<FieldValueType*> : public FieldInfoReadable<FieldValueType*>
		{
		public:
			/// @brief Initializes a field information.
			/// @param FieldPtr Pointer to field.
			template<typename FieldSignatureType>
			GDINL FieldInfoWritable(FieldSignatureType const FieldPtr) 
				: FieldInfoReadable<FieldValueType*>(FieldPtr) 
			{
			}

			/// @brief Returns true if this field is read-only (it`s definition contains const qualifier).
			/// @returns True if this field is read-only (it`s definition contains const qualifier).
			GDINL virtual bool IsReadOnly() const override final 
			{ 
				return false; 
			}

			/// @brief Copies value of input into member.
			/// @param Reflectable Reflectable object we are operating on.
			/// @param InputValuePtr New value of the member would be copied out of here.
            GDINL virtual void SetValue(handle const Reflectable, handle const InputValuePtr) const override final
			{
				GD_NOT_USED(Reflectable);
				FieldValueType& FieldReference = (*this->FieldPtr);
				FieldReference = Forward<FieldValueType>(*(reinterpret_cast<FieldValueType*>(InputValuePtr)));
			}
		};	// class FieldInfoWritable<FieldValueType*>

		/// @brief STATIC FIELD READ/ONLY Specialization
		template<typename FieldValueType>
		class FieldInfoWritable<FieldValueType const*> : public FieldInfoReadable<FieldValueType const*>
		{
		public:
			/// @brief Initializes a field information.
			/// @param FieldPtr Pointer to field.
			template<typename FieldSignatureType>
			GDINL FieldInfoWritable(FieldSignatureType const FieldPtr) 
				: FieldInfoReadable<FieldValueType const*>(FieldPtr)
			{
			}

			/// @brief Returns true if this field is read-only (it`s definition contains const qualifier).
			/// @returns True if this field is read-only (it`s definition contains const qualifier).
			GDINL virtual bool IsReadOnly() const override final
			{
				return true;
			}

		};	// class FieldInfoWritable<FieldValueType const*>

		/// @brief MEMBER FIELD READ/WRITE Specialization
		template<typename FieldOwnerClass, typename FieldValueType>
		class FieldInfoWritable<FieldValueType FieldOwnerClass::*> : public FieldInfoReadable<FieldValueType FieldOwnerClass::*>
		{
		public:
			/// @brief Initializes a field information.
			/// @param FieldPtr Pointer to field.
			template<typename FieldSignatureType>
			GDINL FieldInfoWritable(FieldSignatureType const FieldPtr) 
				: FieldInfoReadable<FieldValueType FieldOwnerClass::*>(FieldPtr) 
			{
			}

			/// @brief Returns true if this field is read-only (it`s definition contains const qualifier).
			/// @returns True if this field is read-only (it`s definition contains const qualifier).
			GDINL virtual bool IsReadOnly() const override final 
			{ 
				return false; 
			}

			/// @brief Copies value of input into member.
			/// @param Reflectable Reflectable object we are operating on.
			/// @param InputValuePtr New value of the member would be copied out of here.
            GDINL virtual void SetValue(handle const Reflectable, handle const InputValuePtr) const override final
			{
				FieldValueType& FieldReference = (object_cast<FieldOwnerClass*>(Reflectable)->*this->FieldPtr);
				FieldReference = Forward<FieldValueType>(*(reinterpret_cast<FieldValueType*>(InputValuePtr)));
			}
		};	// class FieldInfoWritable<FieldValueType FieldValueType::*>

		/// @brief MEMBER FIELD READ/ONLY Specialization
		template<typename FieldOwnerClass, typename FieldValueType>
		class FieldInfoWritable<FieldValueType const FieldOwnerClass::*> : public FieldInfoReadable<FieldValueType const FieldOwnerClass::*>
		{
		public:
			/// @brief Initializes a field information.
			/// @param FieldPtr Pointer to field.
			template<typename FieldSignatureType>
			GDINL FieldInfoWritable(FieldSignatureType const FieldPtr) 
				: FieldInfoReadable<FieldValueType const FieldOwnerClass::*>(FieldPtr) 
			{
			}

			/// @brief Returns true if this field is read-only (it`s definition contains const qualifier).
			/// @returns True if this field is read-only (it`s definition contains const qualifier).
			GDINL virtual bool IsReadOnly() const override final
			{
				return true;
			}

		};	// class FieldInfoWritable<FieldValueType const FieldValueType::*>
	}	// namespace FieldInfoDetails

	/// @brief Provides information about field. Some of the interface functions are implemented.
	/// @code
	/// 	GDINT virtual ITypeInfo const* GetFieldType() const override final;
	///		GDINT virtual ITypeInfo const* GetOwnerType() const override final;
	///	 	GDINT virtual bool IsStatic() const override final;
	///	 	GDINT virtual bool IsReadOnly() const override final;
	///	 	GDINT virtual void GetValue(chandle const Reflectable, handle const OutputValuePtr) const override final;
	///	 	GDINT virtual void SetValue(handle const Reflectable, handle const InputValuePtr) const override final;
	/// @endcode
	/// @tparam FieldSignatureType Type of the field signature.
	/// @tparam FieldPointer Pointer to the field.
	template<typename FieldSignatureType, FieldSignatureType const FieldPointer>
	class SpecializedFieldInfo : public FieldInfoDetails::FieldInfoWritable<FieldSignatureType>
	{
	public:
	
		/// @brief Initializes a field information.
		GDINL SpecializedFieldInfo() 
			: FieldInfoDetails::FieldInfoWritable<FieldSignatureType>(FieldPointer) 
		{
		}
		
	};	// class SpecializedFieldInfo
	
	#pragma endregion
	
	// ==========================================================================================
	// IMethodInfo ...
	// ==========================================================================================

	#pragma region IMethodInfo
	
	/// @brief Namespace includes some support stuff for working on methods reflections.
	namespace MethodInfoDetails
	{
#define GD_METHOD_CTORS(ThisClass, BaseClass) \
		/** @brief Initializes a method information. */ \
		/** @param Method Method Object */\
		template<typename MethodSignature> \
		GDINL ThisClass(MethodSignature const& Method) \
			: BaseClass(Method) \
		{ \
		} \

		/// @brief Type information and invocation.
		template<typename OwnerType, typename ReturnType, typename... ArgumentTypes>
		class BasicMethodInfo : public IMethodInfo, private Delegate<ReturnType(ArgumentTypes...)>
		{
		protected:
			typedef Delegate<ReturnType(ArgumentTypes...)> DelegateType;
			GD_METHOD_CTORS(BasicMethodInfo, DelegateType);

		public:
			/// @brief Returns type information for class that contains this method.
			/// @returns Type information for class that contains this method.
			GDINL virtual ITypeInfo const* GetOwnerType()  const override final
			{
				return TypeOf<OwnerType>();
			}

			/// @brief Returns type information for method return type.
			/// @returns Type information for method return type.
			GDINL virtual ITypeInfo const* GetReturnType() const override final 
			{ 
				return TypeOf<ReturnType>(); 
			}

			/// @brief Returns list or type information for method arguments list.
			/// @returns List or type information for method arguments list.
			GDINL virtual Vector<ITypeInfo const*> const& GetArgumentsTypes() const override final
			{
				static Vector<ITypeInfo const*> const ArgumentTypesList = { TypeOf<ArgumentTypes>()... };
				return ArgumentTypesList;
			}

			/// @brief Invokes method for Instance and copies return result to output with arguments specified as variable arguments list.
			/// @param Reflectable Reflectable object we are operating on.
			/// @param ReturnValueOutputPtr Pointer to the methods execution result. Null pointer may be specified for voids.
			GDINL virtual void Invoke(IClass* const Instance, handle const ReturnValueOutputPtr, ...) const override final
			{
				va_list ArgumentsList; 
				va_start(ArgumentsList, ReturnValueOutputPtr);
				this->DelegateType::Invoke(Instance, ReturnValueOutputPtr, ArgumentsList);
				va_end(ArgumentsList);
			}

		};	// class BasicMethodInfo

		template<typename MethodSignatureType> 
		class MethodInfo;

		/// @brief STATIC Method information.
		template<typename ReturnType, typename... ArgumentTypes>
		class MethodInfo<ReturnType(*)(ArgumentTypes...)> : public BasicMethodInfo<nullptr_t, ReturnType, ArgumentTypes...>
		{
		protected:
			typedef MethodInfoDetails::BasicMethodInfo<nullptr_t, ReturnType, ArgumentTypes...> BasicMethodType;
			GD_METHOD_CTORS(MethodInfo, BasicMethodType);

		public:

			/// @brief Returns true if this method was defined as static.
			/// @returns True if this method was defined as static.
			GDINL virtual bool IsStatic() const override final 
			{ 
				return true; 
			}

			/// @brief Returns true if this methods does not modifies instance that invokes it.
			/// @returns True if this methods does not modifies instance that invokes it.
			GDINL virtual bool IsConst()  const override final 
			{ 
				return false; 
			}
		};	// class MethodInfo<ReturnType(*)(ArgumentTypes...)>

		/// @brief MUTABLE MEMBER Method information.
		template<typename ClassType, typename ReturnType, typename... ArgumentTypes>
		class MethodInfo<ReturnType(ClassType::*)(ArgumentTypes...)> : public BasicMethodInfo<ClassType, ReturnType, ArgumentTypes...>
		{
		protected:
			typedef MethodInfoDetails::BasicMethodInfo<ClassType, ReturnType, ArgumentTypes...> BasicMethodType;
			GD_METHOD_CTORS(MethodInfo, BasicMethodType);

		public:

			/// @brief Returns true if this method was defined as static.
			/// @returns True if this method was defined as static.
			GDINL virtual bool IsStatic() const override final 
			{ 
				return false; 
			}

			/// @brief Returns true if this methods does not modifies instance that invokes it.
			/// @returns True if this methods does not modifies instance that invokes it.
			GDINL virtual bool IsConst()  const override final 
			{ 
				return false; 
			}
		};	// class MethodInfo<ReturnType(ClassType::*)(ArgumentTypes...)>

		/// @brief CONSTANT MEMBER Method information.
		template<typename ClassType, typename ReturnType, typename... ArgumentTypes>
		class MethodInfo<ReturnType(ClassType::*)(ArgumentTypes...) const> : public BasicMethodInfo<ClassType, ReturnType, ArgumentTypes...>
		{
		protected:
			typedef MethodInfoDetails::BasicMethodInfo<ClassType, ReturnType, ArgumentTypes...> BasicMethodType;
			GD_METHOD_CTORS(MethodInfo, BasicMethodType);

		public:

			/// @brief Returns true if this method was defined as static.
			/// @returns True if this method was defined as static.
			GDINL virtual bool IsStatic() const override final 
			{ 
				return false; 
			}

			/// @brief Returns true if this methods does not modifies instance that invokes it.
			/// @returns True if this methods does not modifies instance that invokes it.
			GDINL virtual bool IsConst()  const override final 
			{ 
				return true; 
			}
		};	// class MethodInfo<ReturnType(ClassType::*)(ArgumentTypes...) const>

#undef GD_METHOD_CTORS
	}	// namespace MethodInfoDetails

	/// @brief	Provides information about method. Some of the interface functions are implemented.
	/// @code
	/// 	GDINT virtual ITypeInfo const* GetReturnType() const override final;
	///		GDINT virtual ITypeInfo const* GetOwnerType()  const override final;
	///		GDINT virtual Vector<ITypeInfo const*> const& GetArgumentsTypes() const override final;
	///		GDINT virtual void Invoke(IClass* const Instance, handle const ReturnValueOutputPtr, ...) override final;
	///		GDINT virtual bool IsStatic() const override final;
	///		GDINT virtual bool IsConst()  const override final;
	/// @endcode
	/// @tparam MethodSignatureType Type of the method signature.
	/// @tparam MethodPointer Pointer to the method.
	template<typename MethodSignatureType, MethodSignatureType const MethodPointer>
	class SpecializedMethodInfo : public MethodInfoDetails::MethodInfo<MethodSignatureType>
	{
	public:
	
		/// @brief Initializes a method information.
		GDINL SpecializedMethodInfo() 
			: MethodInfoDetails::MethodInfo<MethodSignatureType>(MethodPointer)
		{
		}
		
	};	// class SpecializedMethodInfo

	#pragma endregion
	
GD_NAMESPACE_END

#endif	// if (!defined(GD_REFLINFO_DUMMY))

// Here be dragons...

// ==========================================================================================
// Common macros ...
// ==========================================================================================

#pragma region CommonMacros

/// @brief Define this to use dummy macros for the RTTI system. 
///        Dummy macros just hide all internal stuff from the developer and documentation tool.
#if (defined(GD_DOCUMENTATION) || defined(__INTELLISENSE__))
#	define GD_REFLINFO_DUMMY 1
#endif	// if (defined(GD_DOCUMENTATION) || defined(__INTELLISENCE__))

#if (!defined(GD_REFLINFO_DUMMY))

/// @brief Defines a final overridden name getter function.
/// @param Name The name.
#define GD_DEFINE_NAME_GETTER(Name) \
	/** @brief Name getter implementation. */ \
	/** @returns A reference to the name of the member. */ \
	GDINT virtual GD::String const& GetName() const override final \
	{ \
		String static const TheName(Name); \
		return TheName; \
	} \

/// @brief Defines a final overridden size getter function.
/// @param ThisType The type.
#define GD_DEFINE_SIZE_GETTER(ThisType) \
	/** @brief Name getter implementation. */ \
	/** @returns A reference to the name of the member. */ \
	GDINT virtual GD::size_t GetSize() const override final \
	{ \
		return sizeof(ThisType); \
	} \

/// @brief Defines a final overridden base type getter function.
/// @param BaseType The base type.
#define GD_DEFINE_BASE_TYPE_GETTER(BaseType) \
	/** @brief Name getter implementation. */ \
	/** @returns A reference to the name of the member. */ \
	GDINT virtual GD::ITypeInfo const* GetBaseType() const override final \
	{ \
		return GD::TypeOf<BaseType>(); \
	} \

/// @brief Defines a final overridden name getter function.
/// @param Type Some reflectable type to collect information.
/// @param Member Type of the member (field, method, etc..).
#define GD_DEFINE_MEMBERS_GETTER(ThisType, Member) \
	/** @brief Returns collection of public members in type. */ \
	/** @returns Collection of public members in type. */ \
	GDINL virtual GD::Vector<GD::I##Member##Info const*> const& Get##Member##s() const override final \
	{ \
		GD_USING_NAMESPACE; \
		typedef Vector<I##Member##Info const*> MembersCollectionType; \
		MembersCollectionType const static MembersCollection = []() -> MembersCollectionType { \
			MembersCollectionType MembersCollection; \
			__Refl_ReflectionInfoCollector::__Refl_CollectReflectionInfo(&MembersCollection, MemberType::Member); \
			\
			return MembersCollection; \
		}(); \
		\
		return MembersCollection; \
	} \

#endif	// if (!defined(GD_REFLINFO_DUMMY))

#pragma endregion

// ==========================================================================================
// IEnumInfo ...
// ==========================================================================================

#pragma region IEnumInfo

/// 
/// Reflectable enum definition example:
/// @code
///		// MyEnum.h:
///		GD_ENUMINFO_DEFINITION_BEGIN(MyEnum, UInt32, GDINT)
///			GD_DEFINE_ENUM_VALUE_AUTO(Zero)		// MyEnum::Zero == 0
///			GD_DEFINE_ENUM_VALUE(A, 123)		// MyEnum::A    == 123,
///			GD_DEFINE_ENUM_VALUE_AUTO(Auto)		// MyEnum::Auto == 124,
///			GD_DEFINE_ENUM_VALUE(B, 1234)		// MyEnum::B    == 1234
///		GD_ENUMINFO_DEFINITION_END();
///		...
///		/// @brief Some type.
///		...
///		// MyEnum.cpp:
///		...
///		GD_ENUMINFO_IMPLEMENTATION(MyEnum, UInt32, GDINT);
///		...
/// @endcode
/// 
/// Reflectable enum usage example:
/// @code
///		MyEnum::Type Value = ...;
///		IEnumInfo const* EnumInfo = static_cast<IEnumInfo const*>(TypeOf(Value));
///		...
/// @endcode
/// 

#if (defined(GD_REFLINFO_DUMMY))

/// @brief Defines some internal data for reflection algorithms for enum.
/// @param ThisType Name of this enum.
/// @param BaseType Base type of this enum.
/// @param EXPORT_SPECIFICATION Export specification of the static methods.
#define GD_ENUMINFO_DEFINITION_BEGIN(ThisType, BaseType, EXPORT_SPECIFICATION) \
	class ThisType final : public IEnum \
	{ \
	public: \
		/** @brief Internal representation of the enumeration. */\
		enum Type : BaseType \
		{ \

#else	// if (defined(GD_REFLINFO_DUMMY))

/// @brief Defines some internal data for reflection algorithms for enum.
/// @param ThisType Name of this enum.
/// @param BaseType Base type of this enum.
/// @param EXPORT_SPECIFICATION Export specification of the static methods.
#define GD_ENUMINFO_DEFINITION_BEGIN(ThisType, BaseType, EXPORT_SPECIFICATION) \
	\
	/** @brief Namespace that contains enum-related stuff. */ \
	class ThisType final : public IEnum \
	{ \
	private: \
		\
		/* Making this class accessible to the reflection manager. */ \
		friend class __Refl_##ThisType##EnumInfo;\
		\
		/** @brief Stores base type of the enum. */ \
		typedef BaseType __Refl_BaseType; \
		\
		/** @brief Defines a safe workaround for start of the counter macro. */ \
		GD::size_t constexpr static __Refl_CounterStart = __COUNTER__; \
		\
		/** @brief Type that wraps value of the enum per it's definition index */ \
		/** @tparam EnumIndex Index of the enum definition */\
		template<size_t const EnumIndex> \
		struct __Refl_EnumValueAccessor final \
		{ \
			/** @brief Generic value of the enum. */ \
			__Refl_BaseType constexpr static __Refl_Value = static_cast<__Refl_BaseType>(0); \
		}; /* struct __Refl_EnumValueAccessor<I> */ \
		\
		/** @brief Type that collects enum information. */ \
		/** @tparam EnumIndex Index of the member. Function recursively invokes self with incremented index. */ \
		template<size_t const EnumIndex> \
		struct __Refl_EnumInfoCollector final \
		{ \
			/** @brief Method that collects enum information. */ \
			/** @param Collection Container that would store collected enum information. */ \
			GDINL static void __Refl_CollectEnumInfo(GD::EnumMap& Collection) \
			{ \
				/* Nothing to do here, just a dummy for end of recursion... */ \
				GD_NOT_USED(Collection); \
			} \
		}; /* struct __Refl_EnumInfoCollector<I> */ \
		\
	public: \
		\
		/** @brief Returns pointer to RTTI data of this enum. */ \
		/** @returns Pointer to RTTI data of this enum. */ \
		EXPORT_SPECIFICATION static GD::IEnumInfo const* GetEnumInfo(); \
		\
	private: \

#endif	// if (!defined(GD_REFLINFO_DUMMY))

#if (defined(GD_REFLINFO_DUMMY))

/// @brief Defines some internal data for reflection algorithms for enum.
#define GD_ENUMINFO_DEFINITION_END() \
		\
		}; /* enum Type */ \
	} /* class ThisType */ \

#else	// if (defined(GD_REFLINFO_DUMMY))

/// @brief Defines some internal data for reflection algorithms for enum.
#define GD_ENUMINFO_DEFINITION_END() \
		\
	} /* namespace ThisType */ \

#endif	// if (!defined(GD_REFLINFO_DUMMY))

#if (defined(GD_REFLINFO_DUMMY))

/// @brief Defines some internal data for reflection algorithms for enum.
/// @param ThisType Name of this type.
/// @param BaseType Base type of this one.
/// @param EXPORT_SPECIFICATION Export specification of the static methods.
#define GD_ENUMINFO_IMPLEMENTATION(ThisType, BaseType, EXPORT_SPECIFICATION)

#else	// if (defined(GD_REFLINFO_DUMMY))

/// @brief Defines some internal data for reflection algorithms for enum.
/// @param ThisType Name of this type.
/// @param BaseType Base type of this one.
/// @param EXPORT_SPECIFICATION Export specification of the static methods.
#define GD_ENUMINFO_IMPLEMENTATION(ThisType, BaseType, EXPORT_SPECIFICATION) \
	\
	/** @brief Enum information class. */ \
	class __Refl_##ThisType##EnumInfo final : public GD::IEnumInfo \
	{ \
		GD_DEFINE_NAME_GETTER(#ThisType); \
		GD_DEFINE_SIZE_GETTER(ThisType); \
		GD_DEFINE_BASE_TYPE_GETTER(BaseType); \
		\
	public: \
		\
		/** @brief Returns the map of this enum. */ \
		GDINL virtual GD::EnumMap const& GetValues() const override final \
		{ \
			GD_USING_NAMESPACE; \
			EnumMap const static EnumCollection = []() -> EnumMap { \
				EnumMap EnumCollection; \
				ThisType::__Refl_EnumInfoCollector<0>::__Refl_CollectEnumInfo(EnumCollection); \
				\
				return EnumCollection; \
			}(); \
			\
			return EnumCollection; \
		} \
		\
	} static const __Refl_##ThisType##EnumInfoInstance; \
	\
	/** @brief Returns pointer to RTTI data of this enum. */ \
	/** @returns Pointer to RTTI data of this enum. */ \
	EXPORT_SPECIFICATION GD::IEnumInfo const* ThisType::GetEnumInfo() \
	{ \
		return &__Refl_##ThisType##EnumInfoInstance; \
	} \
	\

#endif	// if (!defined(GD_REFLINFO_DUMMY))

#if (defined(GD_REFLINFO_DUMMY))

/// @brief Defines an element of the enumeration with name and specified value. 
/// @param Name Name of the enum element.
/// @param Value Value of the enum element.
#define GD_DEFINE_ENUM_VALUE(Name, Value) \
	Name = Value,

#else	// if (defined(GD_REFLINFO_DUMMY))

/// @brief Defines an element of the enumeration with name and specified value.
/// @param Name Name of the enum element.
/// @param Value Value of the enum element.
#define GD_DEFINE_ENUM_VALUE(Name, Value) \
	\
	/** @brief Defines a safe workaround for this element index. */ \
	GD::size_t constexpr static __Refl_##Name##EnumIndex = __COUNTER__ - __Refl_CounterStart - 1; \
	\
public: \
	\
	/** @brief Defines a element of the enum. */ \
	__Refl_BaseType constexpr static Name = (Value); \
	\
private: \
	\
	/** @brief Type that wraps value of the enum per it's definition index */ \
	/** @tparam EnumIndex Index of the enum definition */ \
	template<> \
	struct __Refl_EnumValueAccessor<__Refl_##Name##EnumIndex> final \
	{ \
		/** @brief Value of the enum element with current index. */ \
		__Refl_BaseType constexpr static __Refl_Value = Name; \
	}; /* struct __Refl_EnumValueAccessor */ \
	\
	/** @brief Type that collects enum information. */ \
	/** @tparam EnumIndex Index of the member. Function recursively invokes self with incremented index. */ \
	template<> \
	struct __Refl_EnumInfoCollector<__Refl_##Name##EnumIndex> final \
	{ \
		/** @brief Method that collects member information. */ \
		/** @param Collection Container that would store collected enum information. */ \
		GDINL static void __Refl_CollectEnumInfo(GD::EnumMap& Collection) \
		{ \
			Collection.InsertKeyValue(Name, #Name); \
			__Refl_EnumInfoCollector<__Refl_##Name##EnumIndex + 1>::__Refl_CollectEnumInfo(Collection); /* Invoking the next collector. */ \
		} \
	}; /* struct __Refl_EnumInfoCollector */ \

#endif	// if (defined(GD_REFLINFO_DUMMY))

#if (defined(GD_REFLINFO_DUMMY))

/// @brief Defines an element of the enumeration with name and deduced value.
/// @param Name Name of the enum element.
/// @param Value Value of the enum element.
#define GD_DEFINE_ENUM_VALUE_AUTO(Name) Name,

#else	// if (defined(GD_REFLINFO_DUMMY))

/// @brief Defines an element of the enumeration with name and deduced value.
/// @param Name Name of the enum element.
/// @param Value Value of the enum element.
#define GD_DEFINE_ENUM_VALUE_AUTO(Name) \
	GD_DEFINE_ENUM_VALUE(Name, __Refl_EnumValueAccessor<__Refl_##Name##EnumIndex - 1>::__Refl_Value + 1)

#endif	// if (defined(GD_REFLINFO_DUMMY))
	
#pragma endregion

// ==========================================================================================
// IClassInfo ...
// ==========================================================================================

#pragma region IClassInfo

/// 
/// Reflectable class definition example:
/// Reflection information can be embedded into header file...:
/// @code
///		// MyClass.h:
///		...
///		/// @brief Some type.
///		class MyClass : public SomeOtherReflectableType // or IClass
///		{
///			GD_CLASSINFO_DEFINITION(MyClass, SomeOtherReflectableType, GDINT);
///			...
///			...
///		public:
///			/// @brief Player's moving speed.
///			GD_DEFINE_FIELD(Float32, MovingSpeed) = 2.6f;	// Can be intialized here...
///			... /** OR **/ ...
///			/// @brief Player's moving speed.
///			GD_DEFINE_FIELD(Float32, MovingSpeed);			// Initializes in some other place...
///			...
///			...
///			/// @brief Computes number of items in the inventory.
///			/// @param IncludeExtraItems If true, also counts extra items.
///			/// @returns Number of items in the inventory.
///			GD_DEFINE_METHOD(GDINL, UInt64, ComputeNumberOfItems)(bool const IncludeExtraItems)
///			{
///				...	// Implementation can be placed here...
///				if (IncludeExtraItems) {
///					MovingSpeed *= 2.0f;
///					return 34;
///				} else {
///					return 42;
///				}
///			}
///			... /** OR **/ ...
///			/// @brief Computes number of items in the inventory.
///			/// @param IncludeExtraItems If true, also counts extra items.
///			/// @returns Number of items in the inventory.
///			GD_DEFINE_METHOD(GDINT, UInt64, ComputeNumberOfItems)(bool const IncludeExtraItems);	// Implemented in some other place...
///			...
///			...
///		};	// class MyClass
///		...
///		...
///		// MyClass.cpp:
///		...
///		GD_CLASSINFO_IMPLEMENTATION(MyClass, SomeOtherReflectableType, GDINT);
///		...
/// @endcode
/// ... Or been defined separately:
/// @code
///		// MyClass.h:
///		...
///		/// @brief Some type.
///		class MyClass : public SomeOtherReflectableType // or IClass
///		{
///			...
///			...
///		public:	// Exported members should be public.
///			...
///			/// @brief Player's moving speed.
///			Float32 MovingSpeed = 2.6f;
///			...
///			/// @brief Computes number of items in the inventory.
///			/// @param IncludeExtraItems If true, also counts extra items.
///			/// @returns Number of items in the inventory.
///			GDINT, UInt64, ComputeNumberOfItems(bool const IncludeExtraItems);
///			...
///			...
///		};	// class MyClass
///		...
///		...
///		// MyOtherHeader.h:
///		...
///		...
///		GD_CLASSINFO_DEFINITION_EXTERNAL(MyClass, SomeOtherReflectableType, GDINT);
///		...
///		...
///		// MyOtherSource.cpp:
///		...
///		GD_CLASSINFO_IMPLEMENTATION_EXTERNAL_BEGIN(MyClass, SomeOtherReflectableType, GDINT);
///			GD_IMPLEMENT_FIELD(MovingSpeed)
///			GD_IMPLEMENT_METHOD(ComputeNumberOfItems)
///		GD_CLASSINFO_IMPLEMENTATION_EXTERNAL_END();
///		...
/// @endcode
/// 
/// Reflectable class usage example:
/// @code
///		IClass const* Obj = ...;
///		IClassInfo const* Type = TypeOf(Obj);
///		...
///		// Fields access example:
///		IFieldInfo const* Field = Type->GetFields()[0];	// Or search it in array by name..
///		Float32 MovingSpeed;
///		Field->GetValue(Obj, &MovingSpeed);	// MovingSpeed is 2.6 now.
///		if (!Field->IsReadonly()) {
///			MovingSpeed = 3.0f;
///			Field->SetValue(Obj, &MovingSpeed);	// Obj->MovingSpeed is 3.0 now.
///		}
///		...
///		// Methods invocation example:
///		IMethodInfo const* Method = Type->GetMethods()[0];	// Or search it in array by name..
///		bool const IncludeExtraItems = true;
///		UInt64 Result;
///		Method->Invoke(Obj, &Result, &IncludeExtraItems);	// Result is 34 now, MovingSpeed is 5.2.
/// @endcode
/// 

// ------------------------------------------------------------------------------------------
// GD_CLASSINFO_DEFINITION
// ------------------------------------------------------------------------------------------

#if (defined(GD_REFLINFO_DUMMY))

/// @brief Defines some internal data for reflection algorithms.
/// @param ThisType Name of this type.
/// @param BaseType Base type of this one.
/// @param EXPORT_SPECIFICATION Export specification of the static methods.
#define GD_CLASSINFO_DEFINITION(ThisType, BaseType, EXPORT_SPECIFICATION)

#else	// if (defined(GD_REFLINFO_DUMMY))

/// @brief Defines some internal data for reflection algorithms.
/// @param ThisType Name of this type.
/// @param BaseType Base type of this one.
/// @param EXPORT_SPECIFICATION Export specification of the static methods.
#define GD_CLASSINFO_DEFINITION(ThisType, BaseType, EXPORT_SPECIFICATION) \
private: \
	static_assert(GD::TypeTraits::IsBase<GD::IClass, ThisType>::Value, "'GD_CLASSINFO_DEFINITION_BEGIN' error: 'ThisType' should be derived from 'IClass'"); \
	static_assert(GD::TypeTraits::IsBase<BaseType, ThisType>::Value, "'GD_CLASSINFO_DEFINITION_BEGIN' error: 'ThisType' should be derived from 'BaseType'"); \
	\
	/* Making this class accessible to the reflection manager. */ \
	friend class __Refl_##ThisType##ClassInfo; \
    friend GD::IClass* __Refl_VirtuallyContruct##ThisType(GD::IClass* const Parent, handle const Argument); \
	\
	/** @brief Stores a name of this type. */ \
	typedef ThisType __Refl_ThisType; \
	/** @brief Stores a name of this base type. */ \
	typedef BaseType __Refl_ThisBaseType; \
	\
	/** @brief Defines a safe workaround for start of the counter macro. */ \
	GD::size_t constexpr static __Refl_CounterStart = __COUNTER__; \
	\
	/** @brief Type that collects member information. */ \
	/** @tparam MemberIndex Index of the member. Function recursively invokes self with incremented index. */ \
	template<size_t const MemberIndex> \
	struct __Refl_ClassInfoCollector final \
	{ \
		/** @brief Method that collects member information. */ \
		/** @param CollectionPtr Pointer to container that would store collected member information. */ \
		/** @param CollectionType Type of information data required to be collected. */ \
		GDINL static void __Refl_CollectReflectionInfo(GD::handle const CollectionPtr, GD::MemberType const CollectionType) \
		{ \
			/* Nothing to do here, just a dummy for the end of recursion... */ \
			GD_NOT_USED(CollectionPtr); \
			GD_NOT_USED(CollectionType); \
		} \
	}; /* struct __Refl_ClassInfoCollector<I> */\
	\
public: \
	\
	/** @brief Returns pointer to RTTI data of this type. */ \
	/** @returns Pointer to RTTI data of this type. */ \
	EXPORT_SPECIFICATION GD::IClassInfo const static* GetTypeClassInfo(); \
	\
	/** @brief Returns pointer to RTTI data of this type. */ \
	/** @returns Pointer to RTTI data of this type. */ \
	GDINL virtual GD::IClassInfo const* GetClassInfo() const override \
	{ \
		return __Refl_ThisType::GetTypeClassInfo(); \
	} \
public: \

#endif	// if (defined(GD_REFLINFO_DUMMY))

// ------------------------------------------------------------------------------------------
// GD_CLASSINFO_IMPLEMENTATION
// ------------------------------------------------------------------------------------------

#if (defined(GD_REFLINFO_DUMMY))

/// @brief Defines some internal data for reflection algorithms for types with accessible members.
/// @param ThisType Name of this type.
/// @param BaseType Base type of this one.
/// @param EXPORT_SPECIFICATION Export specification of the static methods.
#define GD_CLASSINFO_IMPLEMENTATION(ThisType, BaseType, EXPORT_SPECIFICATION)

#else	// if (defined(GD_REFLINFO_DUMMY))

/// @brief Defines some internal data for reflection algorithms for types with accessible members.
/// @param ThisType Name of this type.
/// @param BaseType Base type of this one.
/// @param EXPORT_SPECIFICATION Export specification of the static methods.
#define GD_CLASSINFO_IMPLEMENTATION(ThisType, BaseType, EXPORT_SPECIFICATION, ...) \
	\
	static_assert(GD::TypeTraits::IsBase<GD::IClass, ThisType>::Value, "'GD_CLASSINFO_IMPLEMENTATION' error: 'ThisType' should be derived from 'IClass'"); \
	static_assert(GD::TypeTraits::IsBase<BaseType, ThisType>::Value, "'GD_CLASSINFO_IMPLEMENTATION' error: 'ThisType' should be derived from 'BaseType'");	\
	\
	/** @brief Type information class. */ \
	class __Refl_##ThisType##ClassInfo final : public GD::IClassInfo \
	{ \
		GD_DEFINE_NAME_GETTER(#ThisType); \
		GD_DEFINE_SIZE_GETTER(ThisType); \
		GD_DEFINE_BASE_TYPE_GETTER(BaseType); \
		GD_DEFINE_MEMBERS_GETTER(ThisType, Field); \
		GD_DEFINE_MEMBERS_GETTER(ThisType, Method); \
		\
		typedef ThisType::__Refl_ClassInfoCollector<0> __Refl_ReflectionInfoCollector; \
		\
	public: \
		\
		/** @brief Initializes a new type information and registers it in the assembly.*/ \
		GDINL __Refl_##ThisType##ClassInfo() \
		{ \
			GD_USING_NAMESPACE; \
			Reflection::RegisterTypeInfo(this); \
		} \
		\
		/** @brief Deinitializes a new type information and unregisters it from the assembly.*/ \
		GDINL ~__Refl_##ThisType##ClassInfo() \
		{ \
			GD_USING_NAMESPACE; \
			Reflection::UnregisterTypeInfo(this); \
		} \
		\
	} static const __Refl_##ThisType##ClassInfoInstance; \
	\
	/** @brief Returns pointer to RTTI data of this type. */ \
	/** @returns Pointer to RTTI data of this type. */ \
	EXPORT_SPECIFICATION GD::IClassInfo const* ThisType::GetTypeClassInfo() \
	{ \
		return &__Refl_##ThisType##ClassInfoInstance; \
	} \

#endif	// if (defined(GD_REFLINFO_DUMMY))

// ------------------------------------------------------------------------------------------
// GD_DEFINE_MEMBER
// ------------------------------------------------------------------------------------------

#if (!defined(GD_REFLINFO_DUMMY))

/// @brief Defines a generic body for member information collector function. Internal usage only.
/// @param Name Name of the member.
/// @param Member Type of the member (field, method, etc..).
/// @param VaArgs Body of the collector function.
#define GD_DEFINE_MEMBER(Name, Member, ...) \
private: \
	/** @brief Defines a safe workaround for this member index. */ \
	GD::size_t constexpr static __Refl_##Name##MemberIndex = __COUNTER__ - __Refl_CounterStart - 1; \
	\
	/** @brief Type that collects member information. */ \
	/** @tparam MemberIndex Index of the member. Function recursively invokes self with incremented index. */ \
	template<> \
	struct __Refl_ClassInfoCollector<__Refl_##Name##MemberIndex> final \
	{ \
		/** @brief Method that collects member information. */ \
		/** @param CollectionPtr Pointer to container that would store collected member information. */ \
		/** @param CollectionType Type of information data required to be collected. */ \
		GDINL static void __Refl_CollectReflectionInfo(GD::handle const CollectionPtr, GD::MemberType const CollectionType) \
		{ \
			GD_USING_NAMESPACE; \
			if(CollectionType == MemberType::Member) { /* Invoking the real code only on some specific collection pass. */ \
				auto& Collection = *reinterpret_cast<Vector<I##Member##Info const*>*>(CollectionPtr); \
				__VA_ARGS__; /* Placing a real body of the function. */ \
			} \
			\
			__Refl_ClassInfoCollector<__Refl_##Name##MemberIndex + 1>::__Refl_CollectReflectionInfo(CollectionPtr, CollectionType); /* Invoking the next collector. */ \
		} \
	}; /* struct __Refl_ClassInfoCollector */ \
public: \

#endif	// if (!defined(GD_REFLINFO_DUMMY))

// ------------------------------------------------------------------------------------------
// GD_DEFINE_FIELD
// ------------------------------------------------------------------------------------------

#if (defined(GD_REFLINFO_DUMMY))

/// @brief Defines a information about field.
/// @param Type Type of the field.
/// @param Name Name of the field.
#define GD_DEFINE_FIELD(Type, Name) \
	Type Name

#else	// if (defined(GD_REFLINFO_DUMMY))

/// @brief Defines a information about field.
/// @param Type Type of the field.
/// @param Name Name of the field.
#define GD_DEFINE_FIELD(Type, Name) \
	/** @brief Collects a field information. */\
	GD_DEFINE_MEMBER(Name, Field, \
	{ \
		/** @brief Field description implantation class. */ \
		class Name##FieldInfo final : public SpecializedFieldInfo<decltype(&__Refl_ThisType::Name), &__Refl_ThisType::Name> \
		{ \
			GD_DEFINE_NAME_GETTER(#Name); \
		} static const Name##FieldInfo; \
		Collection.PushLast(&Name##FieldInfo); \
	}) \
	\
	/* Defining a field... */ \
	Type Name \

#endif	// if (defined(GD_REFLINFO_DUMMY))

// ------------------------------------------------------------------------------------------
// GD_DEFINE_METHOD
// ------------------------------------------------------------------------------------------

#if (defined(GD_REFLINFO_DUMMY))

/// @brief Defines a information about method.
/// @param ExportSpecification Export specification of the method definition.
/// @param Type Type of the method.
/// @param Name Name of the method.
#define GD_DEFINE_METHOD(ExportSpecification, ReturnType, Name) \
	ExportSpecification ReturnType Name

#else	// if (defined(GD_REFLINFO_DUMMY))

/// @brief Defines a information about method.
/// @param ExportSpecification Export specification of the method definition.
/// @param Type Type of the method.
/// @param Name Name of the method.
#define GD_DEFINE_METHOD(ExportSpecification, ReturnType, Name) \
	/** @brief Collects a method information. */\
	GD_DEFINE_MEMBER(Name, Method, \
	{ \
		/** @brief Method description implantation class. */ \
		class Name##MethodInfo final : public SpecializedMethodInfo<decltype(&__Refl_ThisType::Name), &__Refl_ThisType::Name> \
		{ \
			GD_DEFINE_NAME_GETTER(#Name); \
		} static const Name##MethodInfo; \
		Collection.PushLast(&Name##MethodInfo); \
	}) \
	\
	/* Defining a method... */ \
	ExportSpecification ReturnType Name \

#endif	// if (defined(GD_REFLINFO_DUMMY))
	
#pragma endregion	

#pragma region IClassInfoExtenal

// ------------------------------------------------------------------------------------------
// GD_CLASSINFO_DEFINITION_EXTERNAL
// ------------------------------------------------------------------------------------------

#if (defined(GD_REFLINFO_DUMMY))

/// @brief Defines some internal data for reflection algorithms for types with accessible members.
/// @param ThisType Name of this type.
#define GD_CLASSINFO_DEFINITION_EXTERNAL(ThisType)

#else	// if (defined(GD_REFLINFO_DUMMY))

/// @brief Defines some internal data for reflection algorithms for types with accessible members.
/// @param ThisType Name of this type.
#define GD_CLASSINFO_DEFINITION_EXTERNAL(ThisType)

#endif	// if (defined(GD_REFLINFO_DUMMY))

// ------------------------------------------------------------------------------------------
// GD_CLASSINFO_IMPLEMENTATION_EXTERNAL_BEGIN
// ------------------------------------------------------------------------------------------

#if (defined(GD_REFLINFO_DUMMY))

/// @brief Implements some internal data for reflection algorithms for types with accessible members.
/// @param ThisType Name of this type.
/// @param BaseType Base type of this one.
/// @param EXPORT_SPECIFICATION Export specification of the static methods.
#define GD_CLASSINFO_IMPLEMENTATION_EXTERNAL_BEGIN(ThisType, BaseType, EXPORT_SPECIFICATION)

#else	// if (defined(GD_REFLINFO_DUMMY))

/// @brief Implements some internal data for reflection algorithms for types with accessible members.
/// @param ThisType Name of this type.
/// @param BaseType Base type of this one.
/// @param EXPORT_SPECIFICATION Export specification of the static methods.
#define GD_CLASSINFO_IMPLEMENTATION_EXTERNAL_BEGIN(ThisType, BaseType, EXPORT_SPECIFICATION)

#endif	// if (defined(GD_REFLINFO_DUMMY))

// ------------------------------------------------------------------------------------------
// GD_CLASSINFO_IMPLEMENTATION_EXTERNAL_END
// ------------------------------------------------------------------------------------------

#if (defined(GD_REFLINFO_DUMMY))

/// @brief Implements some internal data for reflection algorithms for types with accessible members.
#define GD_CLASSINFO_IMPLEMENTATION_EXTERNAL_END()

#else	// if (defined(GD_REFLINFO_DUMMY))

/// @brief Implements some internal data for reflection algorithms for types with accessible members.
#define GD_CLASSINFO_IMPLEMENTATION_EXTERNAL_END()

#endif	// if (defined(GD_REFLINFO_DUMMY))

// ------------------------------------------------------------------------------------------
// GD_IMPLEMENT_FIELD
// ------------------------------------------------------------------------------------------

#if (defined(GD_REFLINFO_DUMMY))

/// @brief Implements a information about field.
/// @param Name Name of the field.
#define GD_IMPLEMENT_FIELD(Name)

#else	// if (defined(GD_REFLINFO_DUMMY))

/// @brief Implements a information about field.
/// @param Name Name of the field.
#define GD_IMPLEMENT_FIELD(Name)

#endif	// if (defined(GD_REFLINFO_DUMMY))


// ------------------------------------------------------------------------------------------
// GD_IMPLEMENT_METHOD
// ------------------------------------------------------------------------------------------

#if (defined(GD_REFLINFO_DUMMY))

/// @brief Implements a information about method.
/// @param Name Name of the method.
#define GD_IMPLEMENT_METHOD(Name)

#else	// if (defined(GD_REFLINFO_DUMMY))

/// @brief Implements a information about method.
/// @param Name Name of the method.
#define GD_IMPLEMENT_METHOD(Name)

#endif	// if (defined(GD_REFLINFO_DUMMY))

#pragma endregion

// ==========================================================================================
// IContainerInfo ...
// ==========================================================================================

#pragma region IContainerInfo
#pragma endregion

// Here be dragons no longer...


#endif	// ifndef GD_CORE_OBJECT_REFLECTION_PRIVATE
