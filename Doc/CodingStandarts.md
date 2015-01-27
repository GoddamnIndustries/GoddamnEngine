Goddamn Production Coding Standarts.
====================================

1. Identifiers.
---------------
```C++

// We use Pascal-Case identifiers for variables, methods, enumerations, types and namespaces.
namespace MyNamespace
{
	UInt32 MyVariableName; 

	void MyFunction(String const& MyParam);

	enum class MyEnum : UInt32
	{
		Zero,
		One,
		Three = 3,
	};
}

// Uppercased identifiers for macros.
#define GD_MY_MACRO ... 

// Function-style macro parameter identifiers should be written in the identifier style, if 
// they are supposed to be part of an identifier or an identifier, or in the macro style if
// they are supposed to be part of a macro or a macro.
#define GD_MY_FUNCTION_MACRO(IdentifierParam, MACRO_PARAM) ... 

```

2. Braces
---------
```C++
// K&R-style braces:
class MyClass
{
	// Param braces come after definition without any spaces.
	String MyFunction(UInt32 const Param) const 
	{
		try {
			if (Param == 1488) {
				return "Hail, Hitler!";
			} else {
				auto HailerDelegete = []() {
					printf("Zig Hail!");
				};

				UInt32 Counter = Param;
				do {
					HailerDelegete();
					--Counter;
				} while (Counter != 228);
			}
		} catch (NaziException const&) {
			return "Fuck off!";
		}
	} 
};
```