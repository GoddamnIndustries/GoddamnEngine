//////////////////////////////////////////////////////////////////////////
/// Enumeration.hh: GoddamnC++ enumeration implementation. 
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 30.06.2014 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#include <GoddamnReflector/Enumeration/Enumeration.hh>
#include <GoddamnEngine/Core/Text/StringBuilder/StringBuilder.hh>

GD_NAMESPACE_BEGIN

	LockFreeList<SharedPtr<CPPEnumeration const>> static CPPEnumerationsListImpl;
	LockFreeList<SharedPtr<CPPEnumeration const>> const& CPPEnumerationsList = CPPEnumerationsListImpl;

	/// Fills the array with default enumeration values names.
	static void CPPEnumerationFillNames(SharedPtr<CPPEnumeration const> const& Enumeration, Vector<String>& OutputNames)
	{
		OutputNames.Emptify();
		for (auto const& EnumerationValue : Enumeration->EnumerationValues)
			OutputNames.PushLast(EnumerationValue.Key);
	}

	/// Stubs enumeration values common parts E.g. (MY_ENUM_E1RT_A, MY_ENUM_E2RT_A) would be stringified as ("E1RT_A", "E2RT_A").
	static void CPPEnumerationChopNames(SharedPtr<CPPEnumeration const> const& Enumeration, Vector<String>& OutputNames)
	{
		if (Enumeration->EnumerationValues.GetSize() != OutputNames.GetSize())
			CPPEnumerationFillNames(Enumeration, OutputNames);
		if (Enumeration->EnumerationValues.GetSize() == 0)
			return;

		if (Enumeration->EnumerationStringification.ChoppingPolicy == GD_CPP_ENUMERATION_STRINGIFICATION_CHOPPING_POLICY_CHOPPING)
		{	// We have to chop enum values names here.
			size_t EnumerationValueMaxIndex = SIZE_MAX;
			for (;;)
			{
				String const& InitialStubableString = OutputNames[0];
				if (InitialStubableString.GetSize() == EnumerationValueMaxIndex)
					break;

				Char const CurrentMatchingCharacter = InitialStubableString[EnumerationValueMaxIndex];
				for (auto EnumerationValue = OutputNames.Begin() + 1; EnumerationValue != OutputNames.End(); ++EnumerationValue)
				{
					if ((*EnumerationValue).GetSize() != EnumerationValueMaxIndex)
						if ((*EnumerationValue)[EnumerationValueMaxIndex] == CurrentMatchingCharacter)
						{
							++EnumerationValueMaxIndex;
							continue;
						}

					goto LeaveOuterLoop;
				}
			}
		LeaveOuterLoop:;

			++EnumerationValueMaxIndex;
			for (auto& OutputName : OutputNames)
				OutputName = OutputName.GetSubstring(EnumerationValueMaxIndex);
		}
	}

	/// Translates enumeration values capicalization styles.
	static void CPPEnumerationTranslateCasePolicy(SharedPtr<CPPEnumeration const> const& Enumeration, Vector<String>& OutputNames)
	{
		if (Enumeration->EnumerationValues.GetSize() != OutputNames.GetSize())
			CPPEnumerationFillNames(Enumeration, OutputNames);
		if (Enumeration->EnumerationValues.GetSize() == 0)
			return;

		if (Enumeration->EnumerationStringification.CasePolicy != GD_CPP_ENUMERATION_STRINGIFICATION_CASE_POLICY_AS_IS)
		{	// We have to change policy.
			for (auto& EnumerationValue : OutputNames)
			{
				Vector<Char const*> WordsStubs;
				for (Char const* WordPosition = EnumerationValue.CStr() + 1; (*WordPosition) != Char('\0'); ++WordPosition)
					if (((*WordPosition) == Char('_')) || (islower(*WordPosition) && (isupper(*(WordPosition - 1))))) // We have '_' separator. OR Lowercase character and previous uppercase.
						WordsStubs.PushLast(WordPosition);

				WordsStubs.PushLast(&*EnumerationValue.End());

				StringBuilder EnumerationValueBuilder;
				for (auto const WordStub : WordsStubs)
				{
					if (Enumeration->EnumerationStringification.CasePolicy != GD_CPP_ENUMERATION_STRINGIFICATION_CASE_POLICY_GODDAMN)
					{
						if (EnumerationValueBuilder.GetSize() != 0)
							EnumerationValueBuilder.Append('_');

						if (Enumeration->EnumerationStringification.CasePolicy == GD_CPP_ENUMERATION_STRINGIFICATION_CASE_POLICY_UPPER_CASE)
							EnumerationValueBuilder.Append(String(EnumerationValue.CStr() + (WordStub - EnumerationValue.CStr())).ToUpper());
						else if (Enumeration->EnumerationStringification.CasePolicy == GD_CPP_ENUMERATION_STRINGIFICATION_CASE_POLICY_LOWER_CASE)
							EnumerationValueBuilder.Append(String(EnumerationValue.CStr() + (WordStub - EnumerationValue.CStr())).ToLower());
					}
					else
					{
						String EnumerationValueBuilderStub(EnumerationValue.CStr() + (WordStub - EnumerationValue.CStr()));
						EnumerationValueBuilderStub = Move(EnumerationValueBuilderStub.ToLower());
						EnumerationValueBuilderStub[0] = static_cast<Char>(toupper(EnumerationValueBuilderStub[0]));
						EnumerationValueBuilder.Append(EnumerationValueBuilderStub);
					}
				}

				EnumerationValue = EnumerationValueBuilder.ToString();
			}
		}
	}

GD_NAMESPACE_END
