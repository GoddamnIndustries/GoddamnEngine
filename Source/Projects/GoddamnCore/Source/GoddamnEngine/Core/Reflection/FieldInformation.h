/// ==========================================================================================
/// FieldInformation.h: Common interface for field information.
/// Copyright (C) Goddamn Industries 2011 - Present. All Rights Reserved.
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_REFLECTION_FIELDINFORMATION
#define GD_CORE_REFLECTION_FIELDINFORMATION

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Containers/String.h>
#include <GoddamnEngine/Core/Containers/Vector.h>

#include <GoddamnEngine/Core/Reflection/MemberInformation.h>

GD_NAMESPACE_BEGIN

	/// @brief Provides information about field.
	class IFieldInformation : public IAccessableMemberInformation
	{
	public:
		/// @brief Returns true if this field is static.
		/// @returns True if this field is static.
		GDAPI virtual bool IsStatic() const abstract;

		/// @brief Returns true if this field is read-only (it`s definition contains const qualifier).
		/// @returns True if this field is read-only (it`s definition contains const qualifier).
		GDAPI virtual bool IsReadOnly() const abstract;

		/// @brief Returns type information of field.
		/// @returns Type information of field.
		GDAPI virtual ITypeInformation const* GetFieldType() const abstract;

		/// @brief Returns type information of class that owns this field.
		/// @returns Type information of class that owns this field.
		GDAPI virtual ITypeInformation const* GetOwnerType() const abstract;
	};	// class IFieldInformation

	// Contains some basic interface implementations that does not require to be exported
	namespace FieldInformationDetails
	{
		/// @brief Represents basic information about fields.
		/// @tparam FieldOwnerClass Type of the owner class.
		/// @tparam FieldTypeClass Type of the field.
		template<typename FieldOwnerClass, typename FieldTypeClass>
		class BasicFieldInformation : public IFieldInformation
		{	
		private:
			GD_CLASS_UNASSIGNABLE(BasicFieldInformation);
			GD_CLASS_UNCOPIABLE(BasicFieldInformation);

		protected:
			GDINL BasicFieldInformation()
			{
			}

		public:
			typedef typename TypeTraits::RemoveConst<FieldTypeClass>::Type FieldTypeClassNonConst;

			/// @brief Returns type information of field.
			/// @returns Type information of field.
			GDINL virtual ITypeInformation const* GetFieldType() const override final
			{ 
				return TypeOf<FieldTypeClass>(); 
			}

			/// @brief Returns type information of class that owns this field.
			/// @returns Type information of class that owns this field.
			GDINL virtual ITypeInformation const* GetOwnerType() const override final
			{
				return TypeOf<FieldOwnerClass>();
			}

		};	// class BasicFieldInformation

		/// @brief Represents basic information about READABLE fields.
		/// @tparam FieldSignatureType Type of field signature.
		template<typename FieldSignatureType> 
		class FieldInformationReadable;

		/// @brief STATIC POINTER Specialization.
		/// @tparam FieldValueType Type of type of the static pointer.
		/// @todo Class does not consider owner class of the static pointer.
		template<typename FieldValueType>
		class FieldInformationReadable<FieldValueType*> : public BasicFieldInformation<nullptr_t, FieldValueType>
		{
		private:
			typedef BasicFieldInformation<nullptr_t, FieldValueType> BaseType;
			typedef FieldValueType* FieldPtrType;
			FieldPtrType const FieldPtr = nullptr;

		public:
			/// @brief Initializes a field information.
			/// @param FieldPtr Pointer to field.
			GDINL FieldInformationReadable(FieldPtrType const FieldPtr) 
				: FieldPtr(FieldPtr) 
			{
			}

			/// @brief Returns true if this field is static.
			/// @returns True if this field is static.
			GDINL virtual bool IsStatic() const override final 
			{ 
				return true; 
			}

			/// @brief Copies value of the member to output.
			/// @param Reflectable Reflectable object we are operating on.
			/// @param OutputValuePtr Here would be copied value of the member.
			GDINL virtual void GetValue(IReflectable const* const Reflectable, handle const OutputValuePtr) const override final
			{
				GD_NOT_USED(Reflectable);
				typedef typename BaseType::FieldTypeClassNonConst FieldTypeClassNonConst;
				FieldTypeClassNonConst& OutputValueReference = *(reinterpret_cast<FieldTypeClassNonConst*>(OutputValuePtr));
				OutputValueReference = Forward<FieldTypeClassNonConst>(const_cast<FieldTypeClassNonConst&>(*this->FieldPtr));
			}
		};	// class FieldInformationReadable<FieldValueType*>

		/// @brief MEMBER POINTER Specialization
		template<typename FieldOwnerClass, typename FieldValueType>
		class FieldInformationReadable<FieldValueType FieldOwnerClass::*> : public BasicFieldInformation<FieldOwnerClass, FieldValueType>
		{
		protected:
            typedef BasicFieldInformation<FieldOwnerClass, FieldValueType> BaseType;
			typedef FieldValueType FieldOwnerClass::* FieldPtrType;
			FieldPtrType const FieldPtr = nullptr;

		public:
			/// @brief Initializes a field information.
			/// @param FieldPtr Pointer to field.
			GDINL FieldInformationReadable(FieldPtrType const FieldPtr)
				: FieldPtr(FieldPtr) 
			{
			}

			/// @brief Returns true if this field is static.
			/// @returns True if this field is static.
			GDINL virtual bool IsStatic() const override final 
			{ 
				return false; 
			}

			/// @brief Copies value of the member to output.
			/// @param Reflectable Reflectable object we are operating on.
			/// @param OutputValuePtr Here would be copied value of the member.
			GDINL virtual void GetValue(IReflectable const* const Reflectable, handle const OutputValuePtr) const override final
			{
                typedef typename BaseType::FieldTypeClassNonConst FieldTypeClassNonConst;
				FieldTypeClassNonConst& OutputValueReference = *(reinterpret_cast<FieldTypeClassNonConst*>(OutputValuePtr));
				OutputValueReference = Forward<FieldTypeClassNonConst>(const_cast<FieldTypeClassNonConst&>(object_cast<FieldOwnerClass const*>(Reflectable)->*this->FieldPtr));
			}
		};	// class FieldInformationReadable<FieldValueType*>

		template<typename FieldSignatureType> 
		class FieldInformationWritable;

		/// @brief STATIC FIELD READ/WRITE Specialization
		template<typename FieldValueType>
		class FieldInformationWritable<FieldValueType*> : public FieldInformationReadable<FieldValueType*>
		{
		public:
			/// @brief Initializes a field information.
			/// @param FieldPtr Pointer to field.
			template<typename FieldSignatureType>
			GDINL FieldInformationWritable(FieldSignatureType const FieldPtr) 
				: FieldInformationReadable<FieldValueType*>(FieldPtr) 
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
            GDINL virtual void SetValue(IReflectable* const Reflectable, handle const InputValuePtr) const override final
			{
				GD_NOT_USED(Reflectable);
				FieldValueType& FieldReference = (*this->FieldPtr);
				FieldReference = Forward<FieldValueType>(*(reinterpret_cast<FieldValueType*>(InputValuePtr)));
			}
		};	// class FieldInformationWritable<FieldValueType*>

		/// STATIC FIELD READ/ONLY Specialization
		template<typename FieldValueType>
		class FieldInformationWritable<FieldValueType const*> : public FieldInformationReadable<FieldValueType const*>
		{
		public:
			/// @brief Initializes a field information.
			/// @param FieldPtr Pointer to field.
			template<typename FieldSignatureType>
			GDINL FieldInformationWritable(FieldSignatureType const FieldPtr) 
				: FieldInformationReadable<FieldValueType const*>(FieldPtr)
			{
			}

			/// @brief Returns true if this field is read-only (it`s definition contains const qualifier).
			/// @returns True if this field is read-only (it`s definition contains const qualifier).
			GDINL virtual bool IsReadOnly() const override final
			{
				return true;
			}

			/// @brief Copies value of input into member.
			/// @param Reflectable Reflectable object we are operating on.
			/// @param InputValuePtr New value of the member would be copied out of here.
            GDINL virtual void SetValue(IReflectable* const Reflectable, handle const InputValuePtr) const override final 
			{ 
				GD_NOT_USED(Reflectable);
				GD_NOT_USED(InputValuePtr);
				GD_NOT_SUPPORTED();
			}

		};	// class FieldInformationWritable<FieldValueType const*>

		/// MEMBER FIELD READ/WRITE Specialization
		template<typename FieldOwnerClass, typename FieldValueType>
		class FieldInformationWritable<FieldValueType FieldOwnerClass::*> : public FieldInformationReadable<FieldValueType FieldOwnerClass::*>
		{
		public:
			/// @brief Initializes a field information.
			/// @param FieldPtr Pointer to field.
			template<typename FieldSignatureType>
			GDINL FieldInformationWritable(FieldSignatureType const FieldPtr) 
				: FieldInformationReadable<FieldValueType FieldOwnerClass::*>(FieldPtr) 
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
            GDINL virtual void SetValue(IReflectable* const Reflectable, handle const InputValuePtr) const override final
			{
				FieldValueType& FieldReference = (object_cast<FieldOwnerClass*>(Reflectable)->*this->FieldPtr);
				FieldReference = Forward<FieldValueType>(*(reinterpret_cast<FieldValueType*>(InputValuePtr)));
			}
		};	// class FieldInformationWritable<FieldValueType FieldValueType::*>

		/// MEMBER FIELD READ/ONLY Specialization
		template<typename FieldOwnerClass, typename FieldValueType>
		class FieldInformationWritable<FieldValueType const FieldOwnerClass::*> : public FieldInformationReadable<FieldValueType const FieldOwnerClass::*>
		{
		public:
			/// @brief Initializes a field information.
			/// @param FieldPtr Pointer to field.
			template<typename FieldSignatureType>
			GDINL FieldInformationWritable(FieldSignatureType const FieldPtr) 
				: FieldInformationReadable<FieldValueType const FieldOwnerClass::*>(FieldPtr) 
			{
			}

			/// @brief Returns true if this field is read-only (it`s definition contains const qualifier).
			/// @returns True if this field is read-only (it`s definition contains const qualifier).
			GDINL virtual bool IsReadOnly() const override final
			{
				return true;
			}

			/// @brief Copies value of input into member.
			/// @param Reflectable Reflectable object we are operating on.
			/// @param InputValuePtr New value of the member would be copied out of here.
            GDINL virtual void SetValue(IReflectable* const Reflectable, handle const InputValuePtr) const override final 
			{
				GD_NOT_USED(Reflectable);
				GD_NOT_USED(InputValuePtr);
				GD_NOT_SUPPORTED();
			}

		};	// class FieldInformationWritable<FieldValueType const FieldValueType::*>
	}	// namespace FieldInformationDetails

	/// @brief Provides information about field. Some of the interface functions are implemented.
	/// @code
	/// 	GDINT virtual ITypeInformation const* GetFieldType() const override final;
	///		GDINT virtual ITypeInformation const* GetOwnerType() const override final;
	///	 	GDINT virtual bool IsStatic() const override final;
	///	 	GDINT virtual bool IsReadOnly() const override final;
	///	 	GDINT virtual void GetValue(IReflectable const* const Reflectable, handle const OutputValuePtr) const override final;
	///	 	GDINT virtual void SetValue(IReflectable* const Reflectable, handle const InputValuePtr) const override final;
	/// @endcode
	/// @tparam FieldSignatureType Type of the field signature.
	/// @tparam FieldPointer Pointer to the field.
	template<typename FieldSignatureType, FieldSignatureType const FieldPointer>
	class FieldInformation : public FieldInformationDetails::FieldInformationWritable<FieldSignatureType>
	{
	public:
		/// @brief Initializes a field information.
		GDINL FieldInformation() 
			: FieldInformationDetails::FieldInformationWritable<FieldSignatureType>(FieldPointer) 
		{
		}
	};	// class FieldInformation

GD_NAMESPACE_END

#endif
