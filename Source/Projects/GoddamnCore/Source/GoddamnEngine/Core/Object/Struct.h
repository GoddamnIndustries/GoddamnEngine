// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/Object/Struct.h
 * File contains base declaration for structures in the entities system.
 */
#pragma once

// ReSharper disable CppUnusedIncludeDirective
#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Object/ObjectVisitor.h>
// ReSharper restore CppUnusedIncludeDirective

GD_NAMESPACE_BEGIN

	// **------------------------------------------------------------------------------------------**
	//! Base class for all reflectable structures.
	// **------------------------------------------------------------------------------------------**
	GD_OBJECT_KERNEL struct Struct
	{
		// ReSharper disable CppMemberFunctionMayBeConst
		// ReSharper disable CppMemberFunctionMayBeStatic

		/*!
		 * Visits all properties of this structure.
		 * 
		 * @param objectVisitor Visitor to process this structure.
		 * @see IObjectVisitor interface.
		 */
		GDINL GD_OBJECT_GENERATED void Reflect(ObjectVisitor& objectVisitor)
		{
			GD_NOT_USED(objectVisitor);
		}
		
		/*!
		 * Visits meta information of this structure.
		 * 
		 * @param objectClassVisitor Visitor to process this structure.
		 * @see IObjectClassVisitor interface.
		 */
		GDINL GD_OBJECT_GENERATED static void ReflectStatic(ObjectClassVisitor& objectClassVisitor)
		{
			GD_NOT_USED(objectClassVisitor);
		}

		// ReSharper restore CppMemberFunctionMayBeStatic
		// ReSharper restore CppMemberFunctionMayBeConst

	};	// struct_ Struct
	
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**
	// ******                Very scary macros for generating boilerplate code.                ******
	// **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**

	/*! Declares very a base struct information. Implements reflection algorithms.  */
	#define GD_DECLARE_STRUCT_BASE(TThisClass, TSuperClass, ...) GD_OBJECT_KERNEL \
		public: using This = TThisClass; using Super = TSuperClass; \
		public:	static GD_OBJECT_GENERATED void ReflectStatic(ObjectClassVisitor& objectClassVisitor) \
		{ \
			static StructMetaInfo const structMetaInfo(#TThisClass, ##__VA_ARGS__); \
			/*objectClassVisitor.VisitClass(structMetaInfo);*/ \
		} \

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
		#define GD_DECLARE_STRUCT(TThisClass, TSuperClass, ...) GD_OBJECT_KERNEL \
				GD_DECLARE_STRUCT_BASE(TThisClass, TSuperClass, ##__VA_ARGS__) \
				private: template<unsigned TFieldIndex> void ReflectPrivate(ObjectVisitor&) { } \
				public: GD_OBJECT_GENERATED /*virtual?*/ void Reflect(ObjectVisitor& objectVisitor) /*override?*/ \
				{ \
					TSuperClass::Reflect(objectVisitor); \
					ReflectPrivate<__COUNTER__ + 1>(objectVisitor); \
				} \

		/*! 
		 * Declares a reflectable property information. Can be used for both classes and structures. 
		 * @param TPropertyName Name of the property.
		 */
		#define GD_PROPERTY_NO_DECL(TPropertyName, ...) GD_OBJECT_KERNEL \
				GD_PROPERTY_NO_DECL_BASE(TPropertyName); \
				private: template<> void ReflectPrivate<__COUNTER__>(ObjectVisitor& objectVisitor) \
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
				private: template<> void ReflectPrivate<__COUNTER__>(ObjectVisitor& objectVisitor) \
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
		#define GD_DECLARE_STRUCT(TThisClass, TSuperClass, ...) GD_OBJECT_KERNEL \
				GD_DECLARE_STRUCT_BASE(TThisClass, TSuperClass, ##__VA_ARGS__) \
				private: template<typename TObjectVisitor, unsigned TFieldIndex> struct ReflectPrivate { static void Invoke(TObjectVisitor&, This* const) { } };\
				public: GD_OBJECT_GENERATED /*virtual?*/ void Reflect(ObjectVisitor& objectVisitor) /*override?*/ \
				{ \
					TSuperClass::Reflect(objectVisitor); \
					ReflectPrivate<__COUNTER__ + 1>(objectVisitor); \
				} \

		/*! 
		 * Declares a reflectable property information. Can be used for both classes and structures. 
		 * @param TPropertyName Name of the property.
		 */
		#define GD_PROPERTY_NO_DECL(TPropertyType, TPropertyName, ...) \
				GD_PROPERTY_NO_DECL_BASE(TPropertyType, TPropertyName); \
				private: template<typename TObjectVisitor> struct ReflectPrivate<TObjectVisitor, __COUNTER__> \
				{ \
					static void Invoke(TObjectVisitor& objectVisitor, This* const self) \
					{ \
						static PropertyMetaInfo const propertyMetaInfo(#TPropertyName, ##__VA_ARGS__); \
						objectVisitor.VisitProperty(&propertyMetaInfo, self->TPropertyName); \
						ReflectPrivate<TObjectVisitor, __COUNTER__ + 1>(objectVisitor, self); \
					} \
				}; \

		/*! 
		 * Declares a reflectable function information. Can be used for both classes and structures. 
		 * @param TFunctionName Name of the function.
		 */
		#define GD_FUNCTION_NO_DECL(TFunctionReturnType, TFunctionName, ...) \
			GD_PROPERTY_NO_DECL_BASE(TFunctionReturnType, TFunctionName); \
			private: template<typename TObjectVisitor> struct ReflectPrivate<TObjectVisitor, __COUNTER__> \
			{ \
				static void Invoke(TObjectVisitor& objectVisitor, This* const self) \
				{ \
					static ObjectFunctionMetaInfo const objectFunctionMetaInfo(#TFunctionName, ##__VA_ARGS__); \
					objectVisitor.VisitFunction(&objectFunctionMetaInfo, &This::TFunctionName); \
					ReflectPrivate<__COUNTER__ + 1>(objectVisitor, self); \
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