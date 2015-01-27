///// ==========================================================================================
///// Reflection.cpp: Common interface for RTTI/Introspection/Reflection.
///// Copyright (C) Goddamn Industries 2011 - Present. All Rights Reserved.
///// ==========================================================================================
//
//#include <GoddamnEngine/Core/Reflection/Reflection.h>
//
//GD_NAMESPACE_BEGIN
//
//UniquePtr<HashMap<ITypeInfo*>> Reflection::RegisteredTypes;
//	UniquePtr<LinkedList<Assembly*>> Reflection::RegisteredAssembiles;
//
//	GDAPI void Reflection::RegisterTypeInfo(ITypeInfo* const TypeInfo)
//	{
//		if (RegisteredTypes == nullptr) {
//			RegisteredTypes = new HashMap<ITypeInfo*>();
//		}
//
//		RegisteredTypes->InsertKeyValue(TypeInfo->GetName().GetHashCode(), TypeInfo);
//	}
//
//	GDAPI void Reflection::UnregisterTypeInfo(ITypeInfo const* const TypeInfo)
//	{
//		RegisteredTypes->RemoveElementWithKey(TypeInfo->GetName().GetHashCode());
//	}
//
//GD_NAMESPACE_END
//
//#if 0
//
//#include <GoddamnEngine/Core/Reflection/TypeInfo.h>
//#include <GoddamnEngine/Core/Threading/CriticalSection/CriticalSection.h>
//#include <GoddamnEngine/Core/Diagnostics/Assertion/Assertion.h>
//#include <GoddamnEngine/Core/Object/Object.h>
//
//GD_NAMESPACE_BEGIN
//
//	/// ==========================================================================================
//	IClassInfo const* IClassInfo::lastRegisteredType = nullptr;
//	IClassInfo const* IClassInfo::GetLastRegistered()
//	{
//		return IClassInfo::lastRegisteredType;
//	}
//
//	/// ==========================================================================================
//	IClassInfo::IClassInfo(TypeInfoClassInfo const& typeInformationClassInfo):
//		TypeInfoClassInfo(typeInformationClassInfo),
//		PreviousType(IClassInfo::GetLastRegistered())
//	{
//		{	
//			static CriticalSection TypeInfoRegistrationCs;
//			TypeInfoRegistrationCs.Enter();
//			IClassInfo::lastRegisteredType = this;
//			TypeInfoRegistrationCs.Leave();
//		}
//
//		GD_ASSERT((this->Name.GetLength() > 0),
//			"'IClassInfo::IClassInfo' error: invalid name specified: name is empty");
//
//		GD_ASSERT(((this->BaseType != nullptr) || (this == Object::GetTypeClassInfo())),
//			"'IClassInfo::IClassInfo' error: nullptr base type specified");
//
//		for (IClassInfo const*
//			typeInformation = this->PreviousType; typeInformation != nullptr;
//			typeInformation = typeInformation->PreviousType)
//		{
//			if (typeInformation->Name == this->Name)
//			{
//				GD_ASSERT(/*(memcmp((&this->Size), (&typeInformation->Size), (sizeof(*this) - sizeof(this->Name))) == 0)*/false,
//					"'IClassInfo::IClassInfo' error: this type was alredy registered in RTTI system "
//					"with different parameters than this one. May be your project contains two classes with same "
//					"names. Attempt giving classes unique names.");
//
//				GD_ASSERT(false,
//					"'IClassInfo::IClassInfo' error: this type was alredy registered in RTTI system");
//			}
//		}
//	}
//
//	/// ==========================================================================================
//	IClassInfo const* IClassInfo::FindType(const String& typeName)
//	{
//		for (IClassInfo const* 
//			typeInformationIterator = IClassInfo::GetLastRegistered(); typeInformationIterator != nullptr;
//			typeInformationIterator = typeInformationIterator->PreviousType)
//		{
//			if (typeInformationIterator->Name == typeName)
//			{
//				return typeInformationIterator;
//			}
//		}
//
//		return nullptr;
//	}
//
//	/// ==========================================================================================
//	bool IClassInfo::IsDerivedFrom(IClassInfo const* const typeInformation) const
//	{
//		for (IClassInfo const* 
//			typeInformationIterator = this; typeInformationIterator != nullptr;
//			typeInformationIterator = typeInformationIterator->BaseType)
//		{
//			if (typeInformationIterator == typeInformation)
//			{
//				return true;
//			}
//		}
//
//		return false;
//	}
//
//GD_NAMESPACE_END
//
//#endif // if 0