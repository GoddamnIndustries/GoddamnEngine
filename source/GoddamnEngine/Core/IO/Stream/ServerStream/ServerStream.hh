/// ==========================================================================================
/// ServerStream.hh - Server IO stream interface.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 22.06.2014 - Created by James Jhuighuy
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_IO_STREAM_SERVER_STREAM_SERVER_STREAM
#define GD_CORE_IO_STREAM_SERVER_STREAM_SERVER_STREAM

#include <GoddamnEngine/Include.hh>
#include <GoddamnEngine/Core/IO/Stream/Stream.hh>

GD_NAMESPACE_BEGIN

	class ServerInputStream final : public InputStream
	{
	};	// class ServerInputStream

	class ServerOutputStream final : public OutputStream
	{
	};	// class ServerOutputStream

GD_NAMESPACE_END

#endif
