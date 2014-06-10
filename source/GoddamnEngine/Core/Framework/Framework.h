//////////////////////////////////////////////////////////////////////////
/// Framework.h - Framework interface.
/// Copyright (C) $(GD_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 10.06.2014 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef GD_CORE_FRAMEWORK
#define GD_CORE_FRAMEWORK

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Math/Math.h>

GD_NAMESPACE_BEGIN

	/// Native window / context wrapper class.
	class Window
	{
	private:
		GD_CLASS_UNASSIGNABLE(Window);
		GD_CLASS_UNCOPIABLE  (Window);
		struct WindowImpl;
		struct WindowImpl* const Impl = nullptr;

	public:
		GDAPI static Str       const DefaultWindowTitle;
		GDAPI static Rectangle const DefaultWindowRectangle;

	protected:
		GDAPI explicit Window();
		GDAPI virtual ~Window();

	public:
		GDAPI virtual Rectangle GetRectangle(                             ) const;
		GDAPI virtual void      SetRectangle(Rectangle const& NewRectangle);

		GDAPI virtual bool GetIsFullscreen() const;
		GDAPI virtual void SetIsFullscreen(bool const IsFullscreen);
	};	// class Window

GD_NAMESPACE_END

#endif
