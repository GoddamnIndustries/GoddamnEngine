#ifndef GD_COMPONENTS_STATIC_SCREEN
#define GD_COMPONENTS_STATIC_SCREEN

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Engine/Component/Static/StaticComponent.h>
#include <GoddamnEngine/Core/Math/Math.h>
#include <GoddamnEngine/Core/Math/Math.h>

#include <stdint.h>

#define GD_REAPPLY_RESOLUTION ((const Resolution&)(*(Resolution*)nullptr))

GD_NAMESPACE_BEGIN

	/// @brief		Class for interaction with renderer context, window and screen in common 
	///				By default engine is initialized in HD resolution (1280*720) in fullscreen without cursor
	class Screen final : public StaticComponent,
						 public Singleton<Screen>
	{
	private:
		GD_SERIALIZABLE_DEFINITION(Screen, StaticComponent, GDAPI);

		GD_SERIALIZATION_BEGIN(Screen, StaticComponent);
			GD_FGDLD(Resolution,	screenResolution);
			GD_FGDLD(bool,			isFullscreen);
			GD_FGDLD(bool,			isVsyncEnabled);
		GD_SERIALIZATION_END();

	protected:
		GDINT Screen();
		GDINT virtual ~Screen();

	public:
		typedef UInt32 RenderBitfield;
		enum : UInt32
		{
			ClearScreenDepth	= 1 << 1,	///< Clears depth component	
			ClearScreenColor	= 1 << 2,	///< Clears color component
			All					= UINT32_MAX
		};

		/// @name Resolution, position and bit-count management
		/// @{

		/// @brief				Returns current screen resolution.
		///						By default resolution is HD (as 1280x720) on PC and native on mobile devices and consoles
		/// @param resolution	Output for current screen resolution
		GDINL virtual void GetResolution(Resolution& resolution) const;

		/// @brief				Returns current screen`s width.
		/// @returns			Current screen`s width.
		GDINL static Dimension GetResolutionWidth();

		/// @brief				Returns current screen`s height.
		/// @returns			Current screen`s height.
		GDINL static Dimension GetResolutionHeight();

		/// @brief				Sets screen resolution and applies it, if is able to.
		/// @param resolution	New screen resolution or 'GD_REAPPLY_RESOLUTION' to reapply current one 
		/// @returns			Applied resolution
		/// @note		This function does nothing on mobile devices and consoles
		/// @warning	In DirectX 11 implementation not all resolutions can be compatible with Vertical Synchronization.
		GDAPI virtual const Resolution& SetResolution(const Resolution& resolution);

		/// @}

		/// @name Window position management
		/// @{

		/// @brief				Returns current window position.
		///						By default window position is screen center 
		///						if windowed mode specified, { 0, 0 } Otherwise 
		/// @param position		Output for current window position
		/// @note This functions returns value of { 0, 0 } on mobile devices and consoles
		GDAPI virtual void GetWindowPosition(Position& position) const;

		/// @brief				Applies window position
		/// @param position		New window position
		/// @note This function does nothing on mobile devices and consoles or if engine is running fullscreen
		GDAPI virtual void SetWindowPosition(const Position& position);

		/// @}

		/// @name Windowed/Fullscreen management
		/// @{

		/// @brief				Returns boolean for if game running fullscreen.
		///						By default fullscreen is turned on in release build and is disabled
		///						in debug one.
		/// @returns			Boolean for if game running fullscreen
		/// @note This functions returns true on mobile devices and consoles
		GDAPI virtual bool GetIsFullscreen() const;

		/// @brief				Sets windowed/fullscreen mode.
		/// @param isFullscreen	Set fullscreen or widowed mode.
		/// @note This function does nothing on mobile devices and consoles
		GDAPI virtual void SetIsFullscreen(const bool isFullscreen);

		/// @}

		/// @name VSync management
		/// @{

		/// @brief				Returns boolean for Vertical Synchronization is enabled.
		///						If is disabled, engine is rendering as frequent as
		///						possible, showing maximum FPS. Over wise is rendering in
		///						60/30 FPS maximum and saving performance for other threads.
		/// @returns			Boolean for Vertical Synchronization is enabled
		GDAPI virtual bool GetVsyncEnabled() const;

		/// @brief				Enables/Disables Vertical Synchronization
		/// @param vsyncEnabled	Enable or disable Vertical Synchronization 
		GDAPI virtual void SetVsyncEnabled(const bool isVsyncEnabled);

		/// @}

		/// @name Title management
		/// @{

		/// @brief				Returns window title
		/// @returns			Output to place window title
		/// @note This function returns "GoddamnEngine Window" on mobile devices and consoles
		GDAPI virtual void GetWindowTitle(String& title) const;

		/// @brief				Changes window title
		/// @param title		New window title
		/// @note This function does nothing on mobile devices and consoles
		GDAPI virtual void SetWindowTitle(const String& title);
		
		/// @}

		/// @name Cursor management
		/// @{

		/// @brief				Return boolean for if system cursor is showing
		/// @returns			Boolean for if system cursor is showing
		/// @note This function returns 'false' on mobile devices and consoles
		GDAPI virtual bool GetCursorShowing() const;

		/// @brief				Shows/Hides native cursor
		/// @param showCursor	Show or hide cursor?
		/// @note This function does nothing on mobile devices and consoles
		GDAPI virtual void SetCursorShowing(const bool showCursor);
		
		/// @brief				Shows cursor
		/// @note This function does nothing on mobile devices and consoles
		GDINL static void ShowCursor();

		/// @brief				Hides cursor
		/// @note This function does nothing on mobile devices and consoles
		GDINL static void HideCursor();

		/// @}

		/// @name Screen and Viewport management
		/// @{

		/// @brief			Sets view port in current context
		/// @param viewport Viewport to apply
		GDAPI virtual void ClearViewport(const Vector4& viewport) const {};

		/// @brief			Clears screen in specified mod with specified color
		/// @param color	Color to clear screen with.
		/// @param mode		Mode to clear screen. See 'Renderer::ClearScreen*' enumarations.
		GDAPI virtual void ClearScreen(const Color& color, const RenderBitfield mode = ClearScreenColor | ClearScreenDepth) const {};

		/// @}
	};

GD_NAMESPACE_END

#include <GoddamnEngine/Engine/Component/Static/Screen/Screen.inl>

#endif