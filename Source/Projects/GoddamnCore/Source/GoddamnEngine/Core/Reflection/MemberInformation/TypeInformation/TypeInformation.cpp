#include <GoddamnEngine/Core/Reflection/TypeInformation/TypeInformation.h>
#include <GoddamnEngine/Core/Threading/CriticalSection/CriticalSection.h>
#include <GoddamnEngine/Core/Diagnostics/Assertion/Assertion.h>
#include <GoddamnEngine/Core/Object/Object.h>

GD_NAMESPACE_BEGIN

	/// ==========================================================================================
	TypeInformation const* TypeInformation::lastRegisteredType = nullptr;
	TypeInformation const* TypeInformation::GetLastRegistered()
	{
		return TypeInformation::lastRegisteredType;
	}

	/// ==========================================================================================
	TypeInformation::TypeInformation(TypeInformationClassInfo const& typeInformationClassInfo):
		TypeInformationClassInfo(typeInformationClassInfo),
		PreviousType(TypeInformation::GetLastRegistered())
	{
		{	
			static CriticalSection TypeInformationRegistrationCs;
			TypeInformationRegistrationCs.Enter();
			TypeInformation::lastRegisteredType = this;
			TypeInformationRegistrationCs.Leave();
		}

		GD_ASSERT((this->Name.GetLength() > 0),
			"'TypeInformation::TypeInformation' error: invalid name specified: name is empty");

		GD_ASSERT(((this->BaseType != nullptr) || (this == Object::GetClassTypeInformation())),
			"'TypeInformation::TypeInformation' error: nullptr base type specified");

		for (TypeInformation const*
			typeInformation = this->PreviousType; typeInformation != nullptr;
			typeInformation = typeInformation->PreviousType)
		{
			if (typeInformation->Name == this->Name)
			{
				GD_ASSERT(/*(memcmp((&this->Size), (&typeInformation->Size), (sizeof(*this) - sizeof(this->Name))) == 0)*/false,
					"'TypeInformation::TypeInformation' error: this type was alredy registered in RTTI system "
					"with different parameters than this one. May be your project contains two classes with same "
					"names. Attempt giving classes unique names.");

				GD_ASSERT(false,
					"'TypeInformation::TypeInformation' error: this type was alredy registered in RTTI system");
			}
		}
	}

	/// ==========================================================================================
	TypeInformation const* TypeInformation::FindType(const String& typeName)
	{
		for (TypeInformation const* 
			typeInformationIterator = TypeInformation::GetLastRegistered(); typeInformationIterator != nullptr;
			typeInformationIterator = typeInformationIterator->PreviousType)
		{
			if (typeInformationIterator->Name == typeName)
			{
				return typeInformationIterator;
			}
		}

		return nullptr;
	}

	/// ==========================================================================================
	bool TypeInformation::IsDerivedFrom(TypeInformation const* const typeInformation) const
	{
		for (TypeInformation const* 
			typeInformationIterator = this; typeInformationIterator != nullptr;
			typeInformationIterator = typeInformationIterator->BaseType)
		{
			if (typeInformationIterator == typeInformation)
			{
				return true;
			}
		}

		return false;
	}

GD_NAMESPACE_END
