#include <GoddamnEngine/Include.h>
#include <Windows.h>

class IDHui
{
public:
	virtual ~IDHui() {}

	virtual void Masturbate(int const param)
	{
		MessageBoxA(nullptr, "hui", "hui", MB_OK);
	}
};

GDAPI IDHui* CreateHui(int a, int b)
{
	return new IDHui();
}
