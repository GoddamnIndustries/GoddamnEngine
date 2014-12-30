/// ==========================================================================================
/// FieldInformation.h: Common interface for field information
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 17.04.2014 - Created by James Jhuighuy
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_REFLECTION_FIELDINFORMATION
#define GD_CORE_REFLECTION_FIELDINFORMATION

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Containers/String.h>
#include <GoddamnEngine/Core/Containers/Vector.h>

#include <GoddamnEngine/Core/Reflection/MemberInformation.h>

GD_NAMESPACE_BEGIN

	/// Provides information about field
	class IFieldInformation : public IAccessableMemberInformation
	{
	public:
		/// Returns true if this field is static.
		GDAPI virtual bool IsStatic() const abstract;

		/// Returns true if this field is read-only (it`s definition contains const qualifier).
		GDAPI virtual bool IsReadOnly() const abstract;

		/// Returns type information of field.
		GDAPI virtual ITypeInformation const* GetFieldType() const abstract;

		/// Returns type information of class that owns this field.
		GDAPI virtual ITypeInformation const* GetOwnerType() const abstract;
	};	// class IFieldInformation

#if (!defined(GD_DOCUMENTATION))
	// Contains some basic interface implementations that does not require to be exported
	namespace FieldInformationPrivate
	{
		/// Represents basic information about field types.
		template<typename FieldOwnerClass, typename FieldTypeClass>
		class BasicFieldInformation : public IFieldInformation
		{	
		private:
			GD_CLASS_UNASSIGNABLE(BasicFieldInformation);
			GD_CLASS_UNCOPIABLE(BasicFieldInformation);

		public:
			typedef typename TypeTraits::RemoveConst<FieldTypeClass>::Type ThisFieldTypeClassNonConst;

			GDINL BasicFieldInformation() { }
			GDINL virtual ITypeInformation const* GetOwnerType() const final { return TypeOf<FieldOwnerClass>(); }
			GDINL virtual ITypeInformation const* GetFieldType() const final { return TypeOf<FieldTypeClass>(); }
		};	// class BasicFieldInformation

		template<typename FieldSignatureType> class FieldInformationReadable;

		/// STATIC POINTER Specialization
		template<typename FieldValueType>
		class FieldInformationReadable<FieldValueType*> : public BasicFieldInformation<nullptr_t, FieldValueType>
		{
		private:
			typedef BasicFieldInformation<nullptr_t, FieldValueType> Base;
			typedef FieldValueType* ThisFieldPtrType;
			ThisFieldPtrType const FieldPtr = nullptr;

		public:
			GDINL FieldInformationReadable(ThisFieldPtrType const FieldPtr) : FieldPtr(FieldPtr) { }
			GDINL virtual bool IsStatic() const final { return true; }
   GDINL virtual void GetValue(IReflectable const* const Reflectable, handle const OutputValuePtr) const final
			{
				typedef typename Base::ThisFieldTypeClassNonConst ThisFieldTypeClassNonConst;
				ThisFieldTypeClassNonConst& OutputValueReference = *(reinterpret_cast<ThisFieldTypeClassNonConst*>(OutputValuePtr));
				OutputValueReference = Forward<ThisFieldTypeClassNonConst>(const_cast<ThisFieldTypeClassNonConst&>(*this->FieldPtr));
			}
		};	// class FieldInformationReadable<FieldValueType*>

		/// MEMBER POINTER Specialization
		template<typename FieldOwnerClass, typename FieldValueType>
		class FieldInformationReadable<FieldValueType FieldOwnerClass::*> : public BasicFieldInformation<FieldOwnerClass, FieldValueType>
		{
		protected:
   typedef BasicFieldInformation<FieldOwnerClass, FieldValueType> Base;
			typedef FieldValueType FieldOwnerClass::* ThisFieldPtrType;
			ThisFieldPtrType const FieldPtr = nullptr;

		public:
			GDINL FieldInformationReadable(ThisFieldPtrType const FieldPtr) : FieldPtr(FieldPtr) { }
			GDINL virtual bool IsStatic() const final { return false; }
   GDINL virtual void GetValue(IReflectable const* const Reflectable, handle const OutputValuePtr) const final
			{
    typedef typename Base::ThisFieldTypeClassNonConst ThisFieldTypeClassNonConst;
				ThisFieldTypeClassNonConst& OutputValueReference = *(reinterpret_cast<ThisFieldTypeClassNonConst*>(OutputValuePtr));
				OutputValueReference = Forward<ThisFieldTypeClassNonConst>(const_cast<ThisFieldTypeClassNonConst&>(object_cast<FieldOwnerClass const*>(Reflectable)->*this->FieldPtr));
			}
		};	// class FieldInformationReadable<FieldValueType*>

		template<typename FieldSignatureType> class FieldInformationWriteble;

		/// STATIC FIELD READ/WRITE Specialization
		template<typename FieldValueType>
		class FieldInformationWriteble<FieldValueType*> : public FieldInformationReadable<FieldValueType*>
		{
		public:
			template<typename FieldSignatureType>
			GDINL FieldInformationWriteble(FieldSignatureType const FieldPtr) : FieldInformationReadable<FieldValueType*>(FieldPtr) { }
			GDINL virtual bool IsReadOnly() const final { return false; }
   GDINL virtual void SetValue(IReflectable* const Reflectable, handle const InputValuePtr) const final
			{
				FieldValueType& FieldReference = (*this->FieldPtr);
				FieldReference = Forward<FieldValueType>(*(reinterpret_cast<FieldValueType*>(InputValuePtr)));
			}
		};	// class FieldInformationWriteble<FieldValueType*>

		/// STATIC FIELD READ/ONLY Specialization
		template<typename FieldValueType>
		class FieldInformationWriteble<FieldValueType const*> : public FieldInformationReadable<FieldValueType const*>
		{
		public:
			template<typename FieldSignatureType>
			GDINL FieldInformationWriteble(FieldSignatureType const FieldPtr) : FieldInformationReadable<FieldValueType const*>(FieldPtr) { }
   GDINL virtual void SetValue(IReflectable* const Reflectable, handle const InputValuePtr) const final { GD_DEBUG_ASSERT_FALSE("Attempt to write value to read only field"); }
			GDINL virtual bool IsReadOnly() const final { return true; }
		};	// class FieldInformationWriteble<FieldValueType const*>

		/// MEMBER FIELD READ/WRITE Specialization
		template<typename FieldOwnerClass, typename FieldValueType>
		class FieldInformationWriteble<FieldValueType FieldOwnerClass::*> : public FieldInformationReadable<FieldValueType FieldOwnerClass::*>
		{
		public:
			template<typename FieldSignatureType>
			GDINL FieldInformationWriteble(FieldSignatureType const FieldPtr) : FieldInformationReadable<FieldValueType FieldOwnerClass::*>(FieldPtr) { }
			GDINL virtual bool IsReadOnly() const final { return false; }
   GDINL virtual void SetValue(IReflectable* const Reflectable, handle const InputValuePtr) const final
			{
				FieldValueType& FieldReference = (object_cast<FieldOwnerClass*>(Reflectable)->*this->FieldPtr);
				FieldReference = Forward<FieldValueType>(*(reinterpret_cast<FieldValueType*>(InputValuePtr)));
			}
		};	// class FieldInformationWriteble<FieldValueType FieldValueType::*>

		/// MEMBER FIELD READ/ONLY Specialization
		template<typename FieldOwnerClass, typename FieldValueType>
		class FieldInformationWriteble<FieldValueType const FieldOwnerClass::*> : public FieldInformationReadable<FieldValueType const FieldOwnerClass::*>
		{
		public:
			template<typename FieldSignatureType>
			GDINL FieldInformationWriteble(FieldSignatureType const FieldPtr) : FieldInformationReadable<FieldValueType const FieldOwnerClass::*>(FieldPtr) { }
   GDINL virtual void SetValue(IReflectable* const Reflectable, handle const InputValuePtr) const final { GD_DEBUG_ASSERT_FALSE("Attempt to write value to read only field"); }
			GDINL virtual bool IsReadOnly() const final { return true; }
		};	// class FieldInformationWriteble<FieldValueType const FieldValueType::*>
	}	// namespace FieldInformationPrivate
#endif	// if (!defined(GD_DOCUMENTATION))

#if (!defined(GD_DOCUMENTATION))
	template<typename FieldSignatureType, FieldSignatureType const FieldPointer>
	class FieldInformation : public FieldInformationPrivate::FieldInformationWriteble<FieldSignatureType>
	{
	public:
		GDINL FieldInformation() : FieldInformationPrivate::FieldInformationWriteble<FieldSignatureType>(FieldPointer) { }
	};	// class FieldInformation
#else	// if (!defined(GD_DOCUMENTATION))
	/// Implements some staff about fields that can be generated via templates 
	template<typename FieldSignatureType, FieldSignatureType const FieldPtr>
	class FieldInformation : public IFieldInformation
	{
	public:
		/// Implemented in this class: 
		GDINT virtual ITypeInformation const* GetFieldType() const final;
		GDINT virtual ITypeInformation const* GetOwnerType() const final;
		GDINT virtual bool IsStatic() const final;
		GDINT virtual bool IsReadOnly() const final;
  GDINT virtual void GetValue(IReflectable const* const Reflectable, handle const OutputValuePtr) const final;
  GDINT virtual void SetValue(IReflectable* const Reflectable, handle const InputValuePtr) const final;
	};	// class FieldInformation
#endif	// if (!defined(GD_DOCUMENTATION))

GD_NAMESPACE_END

#endif
