// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/Object/StructInstanceInstance.h
 * File contains base declaration for structures in the entities system.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Object/ObjectVisitor.h>

GD_NAMESPACE_BEGIN

	// **------------------------------------------------------------------------------------------**
	//! Zero-overhead base class for all reflectable structures.
	// **------------------------------------------------------------------------------------------**
	struct GD_OBJECT_KERNEL Struct
	{
	public:

		/*!
		 * Visits all properties of this structure.
		 * 
		 * @param objectVisitor Visitor to process this structure.
		 * @see IObjectVisitor interface.
		 */
		GDINL void GD_OBJECT_GENERATED Reflect(ObjectVisitor& objectVisitor)
		{
			GD_NOT_USED(objectVisitor);
			GD_NOT_USED(this);
		}

	};	// struct Struct
	
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                Very scary macros for generating boilerplate code.                ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	/*! Declares very a base struct information. Implements reflection algorithms.  */
	#define GD_DECLARE_STRUCT_BASE(TThisClass, TSuperClass, ...) GD_OBJECT_KERNEL \
		public: using This = TThisClass; using Super = TSuperClass; \

	/*! Declares a very base property information.  */
	#define GD_PROPERTY_NO_DECL_BASE(TPropertyName) GD_OBJECT_KERNEL

	/*! Declares a very base function information.  */
	#define GD_FUNCTION_NO_DECL_BASE(TFunctionName) GD_OBJECT_KERNEL

	#if GD_COMPILER_MSVC

		// Visual Studio Compiler allows us to use explicit template specialization.
		// So using MSVC compiler we can generate less code and save some compilation time.

		/*! 
		 * Declares a struct information. Implements reflection algorithms. 
		 *
		 * @param TThisClass Name of this class.
		 * @param TSuperClass Name of super class.
		 */
		//! @{
		#define GD_DECLARE_STRUCT(TThisClass, TSuperClass, ...) GD_OBJECT_KERNEL \
				GD_DECLARE_STRUCT_BASE(TThisClass, TSuperClass, ##__VA_ARGS__) \
				private: template<unsigned TFieldIndex> void ReflectPrivate(ObjectVisitor&) { } \
				public: void GD_OBJECT_GENERATED Reflect(ObjectVisitor& objectVisitor) /*NOLINT*/ \
				{ \
					TSuperClass::Reflect(objectVisitor); \
					ReflectPrivate<__COUNTER__ + 1>(objectVisitor); \
				}
		#define GD_DECLARE_STRUCT_VIRTUAL(TThisClass, TSuperClass, ...) GD_OBJECT_KERNEL \
				GD_DECLARE_STRUCT_BASE(TThisClass, TSuperClass, ##__VA_ARGS__) \
				private: template<unsigned TFieldIndex> void ReflectPrivate(ObjectVisitor&) { } \
				public: virtual void GD_OBJECT_GENERATED Reflect(ObjectVisitor& objectVisitor) override /*NOLINT*/ \
				{ \
					TSuperClass::Reflect(objectVisitor); \
					ReflectPrivate<__COUNTER__ + 1>(objectVisitor); \
				} 
		//! @}

		/*! 
		 * Declares a reflectable property information. Can be used for both classes and structures. 
		 * @param TPropertyName Name of the property.
		 */
		#define GD_PROPERTY_NO_DECL(TPropertyName, ...) GD_OBJECT_KERNEL \
				GD_PROPERTY_NO_DECL_BASE(TPropertyName); \
				private: template<> void ReflectPrivate<__COUNTER__>(ObjectVisitor& objectVisitor) /*NOLINT*/ \
				{ \
					static PropertyMetaInfo const propertyMetaInfo(#TPropertyName, ##__VA_ARGS__); \
					objectVisitor.VisitProperty(&propertyMetaInfo, TPropertyName); \
					ReflectPrivate<__COUNTER__ + 1>(objectVisitor); \
				} \

		/*! 
		 * Declares a reflectable function information. Can be used for both classes and structures. 
		 * @param TFunctionName Name of the function.
		 */
		#define GD_FUNCTION_NO_DECL(TFunctionName, ...) GD_OBJECT_KERNEL \
				GD_FUNCTION_NO_DECL_BASE(TFunctionName); \
				private: template<> void ReflectPrivate<__COUNTER__>(ObjectVisitor& objectVisitor) /*NOLINT*/ \
				{ \
					static ObjectFunctionMetaInfo const objectFunctionMetaInfo(#TFunctionName, ##__VA_ARGS__); \
					objectVisitor.VisitFunction(&objectFunctionMetaInfo, &This::TFunctionName); \
					ReflectPrivate<__COUNTER__ + 1>(objectVisitor); \
				} \

	#else	// if GD_COMPILER_MSVC

		// We cannot explicitly specialize template function. So we will have to declare inner struct
		// that replaces explicit template specialization with partial template specialization.
		// See https://gist.github.com/mejedi/5875719

		/*! 
		 * Declares a struct information. Implements reflection algorithms. 
		 *
		 * @param TThisClass Name of this class.
		 * @param TSuperClass Name of super class.
		 */
		//! @{
		#define GD_DECLARE_STRUCT(TThisClass, TSuperClass, ...) GD_OBJECT_KERNEL \
				GD_DECLARE_STRUCT_BASE(TThisClass, TSuperClass, ##__VA_ARGS__) \
				private: template<typename TObjectVisitor, unsigned TFieldIndex> struct ReflectPrivate { static void Invoke(TObjectVisitor&, This* const) { } };\
				public: void GD_OBJECT_GENERATED Reflect(ObjectVisitor& objectVisitor) /*NOLINT*/ \
				{ \
					TSuperClass::Reflect(objectVisitor); \
					ReflectPrivate<ObjectVisitor, __COUNTER__ + 1>::Invoke(objectVisitor, this); \
				}
		#define GD_DECLARE_STRUCT_VIRTUAL(TThisClass, TSuperClass, ...) GD_OBJECT_KERNEL \
				GD_DECLARE_STRUCT_BASE(TThisClass, TSuperClass, ##__VA_ARGS__) \
				private: template<typename TObjectVisitor, unsigned TFieldIndex> struct ReflectPrivate { static void Invoke(TObjectVisitor&, This* const) { } };\
				public: virtual void GD_OBJECT_GENERATED Reflect(ObjectVisitor& objectVisitor) override /*NOLINT*/ \
				{ \
					TSuperClass::Reflect(objectVisitor); \
					ReflectPrivate<ObjectVisitor, __COUNTER__ + 1>::Invoke(objectVisitor, this); \
				}
		//! @}

		/*! 
		 * Declares a reflectable property information. Can be used for both classes and structures. 
		 * @param TPropertyName Name of the property.
		 */
		#define GD_PROPERTY_NO_DECL(TPropertyName, ...) \
				GD_PROPERTY_NO_DECL_BASE(TPropertyName); \
				private: template<typename TObjectVisitor> struct ReflectPrivate<TObjectVisitor, __COUNTER__> \
				{ \
					static void Invoke(TObjectVisitor& objectVisitor, This* const self) \
					{ \
						static PropertyMetaInfo const propertyMetaInfo(#TPropertyName, ##__VA_ARGS__); \
						objectVisitor.VisitProperty(&propertyMetaInfo, self->TPropertyName); \
						ReflectPrivate<TObjectVisitor, __COUNTER__ + 1>::Invoke(objectVisitor, self); \
					} \
				}; \

		/*! 
		 * Declares a reflectable function information. Can be used for both classes and structures. 
		 * @param TFunctionName Name of the function.
		 */
		#define GD_FUNCTION_NO_DECL(TFunctionName, ...) \
			GD_PROPERTY_NO_DECL_BASE(TFunctionName); \
			private: template<typename TObjectVisitor> struct ReflectPrivate<TObjectVisitor, __COUNTER__> \
			{ \
				static void Invoke(TObjectVisitor& objectVisitor, This* const self) \
				{ \
					static ObjectFunctionMetaInfo const objectFunctionMetaInfo(#TFunctionName, ##__VA_ARGS__); \
					objectVisitor.VisitFunction(&objectFunctionMetaInfo, &This::TFunctionName); \
					ReflectPrivate<__COUNTER__ + 1>::Invoke(objectVisitor, self); \
				} \
			}; \

	#endif	// if GD_COMPILER_MSVC

	/*! 
	 * Declares a reflectable property. Can be used for both classes and structures. 
	 *
	 * @param TPropertyAccess Access modifier for this property.
	 * @param TPropertyType Type of the property.
	 * @param TPropertyName Name of the property.
	 */
	#define GD_PROPERTY(TPropertyAccess, TPropertyType, TPropertyName, ...) GD_OBJECT_KERNEL \
			GD_PROPERTY_NO_DECL(TPropertyName, ##__VA_ARGS__); \
			TPropertyAccess: TPropertyType TPropertyName

	/*! 
	 * Declares a reflectable function. Can be used for both classes and structures. 
	 *
	 * @param TFunctionAccess Access modifier for this property.
	 * @param TEAPI Export modifier for this property.
	 * @param TFunctionReturnType Return type of the property.
	 * @param TFunctionName Name of the function.
	 */
	#define GD_FUNCTION(TFunctionAccess, TEAPI, TFunctionReturnType, TFunctionName, ...) GD_OBJECT_KERNEL \
			GD_FUNCTION_NO_DECL(TFunctionName, ##__VA_ARGS__); \
			TFunctionAccess: TEAPI TFunctionReturnType TFunctionName

GD_NAMESPACE_END
