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
#include <GoddamnEngine/Core/Object/Serialization/Serialization.h>
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

    class TestAddress final : public SerializableObject
    {
        GD_DECLARE_OBJECT(TestAddress, SerializableObject)
        GD_PROPERTY(public, WideString, StreetAddress);
        GD_PROPERTY(public, WideString, City);
        GD_PROPERTY(public, Int16, PostalCode);
    };  // class TestAddress
    GD_IMPLEMENT_OBJECT(TestAddress)

    class TestPerson final : public SerializableObject
    {
        GD_DECLARE_OBJECT(TestPerson, SerializableObject)
        GD_PROPERTY(public, WideString, FirstName, PFNotSerializable);
        GD_PROPERTY(public, WideString, LastName);
        GD_PROPERTY(public, RefPtr<TestAddress>, Address, PFChildObject);
        GD_PROPERTY(public, Vector<TestPhoneNumber>, PhoneNumbers);
    };  // class TestPerson
    GD_IMPLEMENT_OBJECT(TestPerson)

    gd_testing_unit_test(SerializableObjectSerialize)
    {
        auto const address = TestAddress::CreateObject();
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
        person->SerializeSync(outputStream);
        
        auto const str = (char*)outputStream.GetData().GetData();
        printf(str);
        
        auto const r = R"(
        {
            "m_IsDynamic" : false,
            "LastName" : "Smith",
            "Address" : {
                "ObjectGuid" : "8329EE39AAA76193-6E7B829729583F75",
                "ObjectClass" : "TestAddress"
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
        }
        )";
        
        MemoryInputStream inputStream(r, CString::Strlen(r));
        person->DeserializeSync(inputStream);
    };

GD_NAMESPACE_END

#endif  // if GD_TESTING_ENABLED
