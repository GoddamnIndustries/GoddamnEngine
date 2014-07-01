//////////////////////////////////////////////////////////////////////////
/// Enumeration.hh: GoddamnC++ enumeration implementation. 
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 30.06.2014 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#include <GoddamnReflector/Enumeration/Enumeration.hh>
#include <GoddamnEngine/Core/Text/StringBuilder/StringBuilder.hh>
#include <GoddamnEngine/Core/IO/Stream/Stream.hh>

GD_NAMESPACE_BEGIN

	LockFreeList<SharedPtr<CPPEnumeration const>> static CPPEnumerationsListImpl;
	LockFreeList<SharedPtr<CPPEnumeration const>> const& CPPEnumerationsList = CPPEnumerationsListImpl;

	/// Fills the array with default enumeration values names.
	/// @param Enumeration Pointer on enumeration description.
	/// @param OutputNames List of generated output names.
	extern void CPPEnumerationFillNames(SharedPtr<CPPEnumeration const> const& Enumeration, Vector<String>& OutputNames)
	{
		OutputNames.Emptify();
		for (auto const& EnumerationValue : Enumeration->EnumerationValues)
			OutputNames.PushLast(EnumerationValue.Key);
	}

	/// Chops enumeration values common parts E.g. (MY_ENUM_E1RT_A, MY_ENUM_E2RT_A) would be stringified as ("E1RT_A", "E2RT_A").
	/// @param Enumeration Pointer on enumeration description.
	/// @param OutputNames List of generated output names.
	extern void CPPEnumerationChopNames(SharedPtr<CPPEnumeration const> const& Enumeration, Vector<String>& OutputNames)
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
	/// @param Enumeration Pointer on enumeration description.
	/// @param OutputNames List of generated output names.
	extern void CPPEnumerationTranslateCasePolicy(SharedPtr<CPPEnumeration const> const& Enumeration, Vector<String>& OutputNames)
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

	/// Generates stringiciators methods for specified enumeration.
	/// @param Enumeration                 Pointer on enumeration description.
	/// @param OutputGeneratedHeaderStream Stream to which stringiciators interfaces would be written.
	/// @param OutputGeneratedSourceStream Stream to which stringiciators implementations would be written.
	extern void CPPEnumerationWriteStringificators(SharedPtr<CPPEnumeration const> const& Enumeration, OutputStream* const OutputGeneratedHeaderStream, OutputStream* const OutputGeneratedSourceStream)
	{
		GD_DEBUG_ASSERT((OutputGeneratedHeaderStream != nullptr) || (Enumeration->EnumerationStringification.ExportPolicy == GD_CPP_ENUMERATION_STRINGIFICATION_EXPORT_POLICY_INTERNAL), "No header output specified.");
		GD_DEBUG_ASSERT((OutputGeneratedHeaderStream != nullptr), "No source output specified.");

		String const EnumerationToStrInterface   = String::Format("extern Str %sToStr(%s const Value)",   Enumeration->EnumerationName.CStr(), Enumeration->EnumerationName.CStr());
		String const EnumerationFromStrInterface = String::Format("extern %s %sFromStr(Str const Value)", Enumeration->EnumerationName.CStr(), Enumeration->EnumerationName.CStr());

		if (Enumeration->EnumerationStringification.ExportPolicy == GD_CPP_ENUMERATION_STRINGIFICATION_EXPORT_POLICY_PUBLIC)
		{	// We need to write prototypes into header file.
			OutputGeneratedHeaderStream->WriteString(String::Format("\n\n\tenum %s : %s;", Enumeration->EnumerationName.CStr(), Enumeration->EnumerationBaseTypeName.CStr()));
			OutputGeneratedHeaderStream->WriteString(String::Format("\n\n\t/// Converts %s to string.\n\tGDAPI %s;", Enumeration->EnumerationName.CStr(), EnumerationToStrInterface.CStr()));
			OutputGeneratedHeaderStream->WriteString(String::Format("\n\n\t/// Converts string to %s.\n\tGDAPI %s;", Enumeration->EnumerationName.CStr(), EnumerationFromStrInterface.CStr()));
		}

		/// Writing stringificators bodies.
		Vector<String> EnumerationNames;
		CPPEnumerationChopNames(Enumeration, EnumerationNames);
		CPPEnumerationTranslateCasePolicy(Enumeration, EnumerationNames);
		
		// Writing ToString function.
		OutputGeneratedSourceStream->WriteString(String("\n\n\t") + EnumerationToStrInterface);
		OutputGeneratedSourceStream->WriteString("\n\t{\n\t\tswitch(Value)\n\t\t{");
		for (size_t Cnt = 0; Cnt < EnumerationNames.GetSize(); ++Cnt)
		{	// Writing stringified values.
			OutputGeneratedSourceStream->WriteString(String::Format("\n\t\t\tcase %s: return \"%s\";"
				, Enumeration->EnumerationValues.GetElementAt(Cnt).Key.CStr()
				, EnumerationNames.GetElementAt(Cnt).CStr()));
		}
		OutputGeneratedSourceStream->WriteString("\n\t\t}\n\t\treturn nullptr;\n\t}");
		
		// Writing FromString function.
		OutputGeneratedSourceStream->WriteString(String("\n\n\t") + EnumerationFromStrInterface);
		OutputGeneratedSourceStream->WriteString("\n\t{\n\t\tUInt32 const HashCode = String(Value).GetHashSumm();\n\t\tswitch(HashCode) {");
		for (size_t Cnt = 0; Cnt < EnumerationNames.GetSize(); ++Cnt)
		{	// Writing prehashed values.
			OutputGeneratedSourceStream->WriteString(String::Format("\n\t\t\tcase %d: return %s;"
				, static_cast<Int32>(EnumerationNames.GetElementAt(Cnt).GetHashSumm().GetValue())
				, Enumeration->EnumerationValues.GetElementAt(Cnt).Key.CStr()));
		}
		OutputGeneratedSourceStream->WriteString("\n\t\t}\n\t\treturn ???Something???;\n\t}");
	}

GD_NAMESPACE_END
