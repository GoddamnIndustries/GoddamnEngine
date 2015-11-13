#define GD_DLL_IMPORT 1
#include <GoddamnEngine/Core/JSON/Json.h>
#include <GoddamnEngine/Core/IO/Stream/FileStream/FileStream.h>
using namespace GD_NAMESPACE;

#include <vector>

int main(int argc, const char** argv)
{
	{
		Map<String, String> a;
		RedBlackTree<Pair<String, String>>& b = a;

		a.Insert("1", "1");
		a.Insert("2", "2");
		a.Insert("3", "4");

	//	a.Erase("2");

		typedef char* t;
		t* u = nullptr;
		u->~t();

		int j = 0;
		for (auto i = a.Begin(); i != a.End(); ++i)
		{
			j += 1;
			printf_s("\n%s %s", i->Key.CStr(), i->Value.CStr());
		}
	}

	int i = sizeof(String);
	
	FileInputStream stream("D:\\GoddamnEngine\\Source\\Projects\\GoddamnCore\\Source\\GoddamnEngine\\Core\\JSON\\test.json");
	auto const json = JsonSerializer::Parse(&stream);

	JsonSerializer::Write(json.Result, nullptr);
 }