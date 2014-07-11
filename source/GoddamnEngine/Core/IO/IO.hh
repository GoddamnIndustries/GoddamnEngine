//////////////////////////////////////////////////////////////////////////
/// IO.hh: Input/Output base interface.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 04.07.2014  - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef GD_CORE_IO
#define GD_CORE_IO

#include <GoddamnEngine/Include.hh>
#include <GoddamnEngine/Core/Diagnostics/Exception/Exception.hh>

GD_NAMESPACE_BEGIN

	/// Represents Input/Output runtime error.
	class IOException : public Exception
	{
	public /*Public API*/:
		GDINL explicit IOException(String const Message) : Exception(Message.CStr()) { }
		GDINL virtual ~IOException() { }
	};	// class IOException

GD_NAMESPACE_END

#endif
