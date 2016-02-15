#include <GoddamnEngine/Include.h>

GD_NAMESPACE_BEGIN

	class PlatformMisc final : public TNonCreatable
	{
	public:

		GDINL static void Sleep(UInt32 const durationMilliseconds)
		{
			::Sleep(durationMilliseconds);
		}

	};	// class PlatformMisc

GD_NAMESPACE_END
