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
			TypeInformation::lastRegisteredType = self;
			TypeInformationRegistrationCs.Leave();
		}

		GD_ASSERT((self->Name.GetSize() > 0),
			"'TypeInformation::TypeInformation' error: invalid name specified: name is empty");

		GD_ASSERT(((self->BaseType != nullptr) || (self == Object::GetClassTypeInformation())),
			"'TypeInformation::TypeInformation' error: nullptr base type specified");

		for (TypeInformation const*
			typeInformation = self->PreviousType; typeInformation != nullptr;
			typeInformation = typeInformation->PreviousType)
		{
			if (typeInformation->Name == self->Name)
			{
				GD_ASSERT(/*(memcmp((&self->Size), (&typeInformation->Size), (sizeof(*self) - sizeof(self->Name))) == 0)*/false,
					"'TypeInformation::TypeInformation' error: this type was alredy registered in RTTI system "
					"with different parameters than this one. May be your project contains two classes with same "
					"names. Attempt giving classes unique names.");

				GD_ASSERT(false,
					"'TypeInformation::TypeInformation' error: this type was alredy registered in RTTI system");
			}
		}
	}

	/// ==========================================================================================
	TypeInformation const* TypeInformation::SearchForType(const String& typeName)
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
			typeInformationIterator = self; typeInformationIterator != nullptr;
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
