//#include <GoddamnEngine/Core/Reflection/TypeInformation.h>
//#include <GoddamnEngine/Core/Threading/CriticalSection/CriticalSection.h>
//#include <GoddamnEngine/Core/Diagnostics/Assertion/Assertion.h>
//#include <GoddamnEngine/Core/Object/Object.h>
//
//GD_NAMESPACE_BEGIN
//
//	/// ==========================================================================================
//	ITypeInformation const* ITypeInformation::lastRegisteredType = nullptr;
//	ITypeInformation const* ITypeInformation::GetLastRegistered()
//	{
//		return ITypeInformation::lastRegisteredType;
//	}
//
//	/// ==========================================================================================
//	ITypeInformation::ITypeInformation(TypeInformationClassInfo const& typeInformationClassInfo):
//		TypeInformationClassInfo(typeInformationClassInfo),
//		PreviousType(ITypeInformation::GetLastRegistered())
//	{
//		{	
//			static CriticalSection TypeInformationRegistrationCs;
//			TypeInformationRegistrationCs.Enter();
//			ITypeInformation::lastRegisteredType = this;
//			TypeInformationRegistrationCs.Leave();
//		}
//
//		GD_ASSERT((this->Name.GetLength() > 0),
//			"'ITypeInformation::ITypeInformation' error: invalid name specified: name is empty");
//
//		GD_ASSERT(((this->BaseType != nullptr) || (this == Object::GetClassTypeInformation())),
//			"'ITypeInformation::ITypeInformation' error: nullptr base type specified");
//
//		for (ITypeInformation const*
//			typeInformation = this->PreviousType; typeInformation != nullptr;
//			typeInformation = typeInformation->PreviousType)
//		{
//			if (typeInformation->Name == this->Name)
//			{
//				GD_ASSERT(/*(memcmp((&this->Size), (&typeInformation->Size), (sizeof(*this) - sizeof(this->Name))) == 0)*/false,
//					"'ITypeInformation::ITypeInformation' error: this type was alredy registered in RTTI system "
//					"with different parameters than this one. May be your project contains two classes with same "
//					"names. Attempt giving classes unique names.");
//
//				GD_ASSERT(false,
//					"'ITypeInformation::ITypeInformation' error: this type was alredy registered in RTTI system");
//			}
//		}
//	}
//
//	/// ==========================================================================================
//	ITypeInformation const* ITypeInformation::FindType(const String& typeName)
//	{
//		for (ITypeInformation const* 
//			typeInformationIterator = ITypeInformation::GetLastRegistered(); typeInformationIterator != nullptr;
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
//	bool ITypeInformation::IsDerivedFrom(ITypeInformation const* const typeInformation) const
//	{
//		for (ITypeInformation const* 
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
