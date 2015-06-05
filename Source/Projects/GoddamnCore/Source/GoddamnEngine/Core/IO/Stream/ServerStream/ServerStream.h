/// ==========================================================================================
/// ServerStream.h - Server IO stream interface.
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// 
/// History:
///		* 22.06.2014 - Created by James Jhuighuy
/// ==========================================================================================

#pragma once
#ifndef GD_CORE_IO_STREAM_SERVER_STREAM_SERVER_STREAM
#define GD_CORE_IO_STREAM_SERVER_STREAM_SERVER_STREAM

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/IO/Stream/Stream.h>

GD_NAMESPACE_BEGIN

	class ServerInputStream final : public InputStream
	{
	};	// class ServerInputStream

	class ServerOutputStream final : public OutputStream
	{
	};	// class ServerOutputStream

GD_NAMESPACE_END

#endif
