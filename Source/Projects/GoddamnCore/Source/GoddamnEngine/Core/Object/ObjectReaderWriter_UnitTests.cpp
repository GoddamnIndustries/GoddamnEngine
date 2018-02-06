// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
//
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file
 * Serialization system unit tests.
 */
#include <GoddamnEngine/Core/Object/ObjectReaderWriterJson.h>
#if GD_TESTING_ENABLED

#include <GoddamnEngine/Core/IO/MemoryStream.h>

GD_NAMESPACE_BEGIN

    struct TestPhoneNumber final : public Struct
    {
        GD_DECLARE_STRUCT(TestPhoneNumber, Struct)
        GD_PROPERTY(public, UInt32, Code);
        GD_PROPERTY(public, UInt64, Number);
        
    public:
        GDINL TestPhoneNumber(UInt32 const code = 0, UInt64 const number = 0)
            : Code(code), Number(number)
        {
        }
    };  // struct TestPhoneNumber

    class TestAddress final : public Object
    {
        GD_DECLARE_OBJECT(TestAddress, Object)
        GD_PROPERTY(public, WideString, StreetAddress);
        GD_PROPERTY(public, WideString, City);
        GD_PROPERTY(public, Int16, PostalCode);
    };  // class TestAddress
    GD_IMPLEMENT_OBJECT(TestAddress)

    class TestPerson final : public Object
    {
        GD_DECLARE_OBJECT(TestPerson, Object)
        GD_PROPERTY(public, WideString, FirstName, PFNotSerializable);
        GD_PROPERTY(public, WideString, LastName);
        GD_PROPERTY(public, RefPtr<TestAddress>, Address, PFChildren);
        GD_PROPERTY(public, Vector<TestPhoneNumber>, PhoneNumbers);
    };  // class TestPerson
    GD_IMPLEMENT_OBJECT(TestPerson)

    gd_testing_unit_test(SerializableObjectSerialize)
    {
		auto const r = R"(
    		[
				{
						"GODDAMN_OBJECT_CLASS" : "TestPerson",
						"GODDAMN_OBJECT_GUID" : "8157833EDF004545-8DB762891FF11602",
						"LastName" : "Smith",
						"Address" : {
								"GODDAMN_OBJECT_CLASS" : "TestAddress",
								"GODDAMN_OBJECT_GUID" : "F1772840B0434D61-B38E58AB04D593C9"
						},
						"PhoneNumbers" : [
								{
										"Code" : 631.000000,
										"Number" : 5964570.000000
								},
								{
										"Code" : 917.000000,
										"Number" : 7444138.000000
								}
						]
				},
				{
						"GODDAMN_OBJECT_CLASS" : "TestAddress",
						"GODDAMN_OBJECT_GUID" : "F1772840B0434D61-B38E58AB04D593C9",
						"StreetAddress" : "779 Abia Martin Drive",
						"City" : "New York",
						"PostalCode" : 10011.000000
				}
			])";

        /*auto const address = TestAddress::CreateObject();
        address->StreetAddress = L"779 Abia Martin Drive";
        address->City = L"New York";
        address->PostalCode = 10011;
        
        auto const person = TestPerson::CreateObject();
        person->FirstName = L"Jhon";
        person->LastName = L"Smith";
        person->Address = address;
        person->PhoneNumbers.InsertLast(TestPhoneNumber(631, 5964570));
        person->PhoneNumbers.InsertLast(TestPhoneNumber(917, 7444138));
        
        MemoryOutputStream outputStream;
		{
			ObjectWriterJson objectWriter(outputStream);
			objectWriter.WriteObject(person);
		}
		outputStream.Write('\0');
        
        auto const str = (char*)outputStream.GetData().GetData();
        printf(str);
        
		person->LastName = L"Hui";*/

        MemoryInputStream inputStream(r, CString::Strlen(r));
		ObjectReaderJson objectReader(inputStream);

		auto person = objectReader.ReadObject<TestPerson>();
		MemoryOutputStream outputStream;
		{
			ObjectWriterJson objectWriter(outputStream);
			objectWriter.WriteObject(person);
			outputStream.Write('\0');
		}

		auto const str = (char*)outputStream.GetData().GetData();
		printf(str);
		printf(str);
	};

GD_NAMESPACE_END

#endif  // if GD_TESTING_ENABLED
