//////////////////////////////////////////////////////////////////////////
/// HUD.h: In-game UI elements 
/// Copyright (C) $(GD_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 24.03.2014 - Created by James Jhuighuy
//////////////////////////////////////////////////////////////////////////

#ifndef GD_ENGINE_RENDERER_HUD_HUD
#define GD_ENGINE_RENDERER_HUD_HUD

#include <GoddamnEngine/Include.hh>
#include <GoddamnEngine/Core/Events/Event.hh>
#include <GoddamnEngine/Core/Object/RefPtr/RefPtr.hh>
#include <GoddamnEngine/Engine/Renderer/Renderer.hh>
#include <GoddamnEngine/Engine/Renderer/Shader/Shader.hh>

GD_NAMESPACE_BEGIN

	class HUDElementPanel;
	class HUDElementButton;
	class HUDElementLabel;

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	/// Class represents format for texture atlas used in HUD system
	class HUDTextureAtlas final : public HRIObject
	{
		//GD_TYPEINFORMATION_DEFINITION(HUDTextureAtlas, HRIObject, GDAPI);

	public:
		RefPtr<HRITexture2D const> const AtlasTexture;		///< Atlas texture pointer
		UInt32x2				const AtlasDimensions;	///< Number rows and columns of images in atlas 
		Vector2					const AtlasFixedZone;	///< Dimension of one image in atlas that would not be scaled

		//////////////////////////////////////////////////////////////////////////
		GDINL HUDTextureAtlas(
			_In_ HRITexture2D const* const AtlasTexture,
			_In_ UInt32x2  const&		AtlasDimensions,
			_In_ Vector2   const&		AtlasFixedZone
		) : HRIObject(),
			AtlasTexture(AtlasTexture), AtlasDimensions(AtlasDimensions), AtlasFixedZone(AtlasFixedZone)
		{ }

		//////////////////////////////////////////////////////////////////////////
		GDINL virtual ~HUDTextureAtlas(
		) { }
	};

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	/// Class represents format for font atlas used in HUD system
	class HUDCharactersAtlas final : public HRIObject 
	{
		//GD_TYPEINFORMATION_DEFINITION(HUDCharactersAtlas, HRIObject, GDAPI);

	public:
		RefPtr<HRITexture2D const> const AtlasTexture;			///< Atlas texture pointer
		UInt32x2				const AtlasDimensions;		///< Number rows and columns of images in atlas 
		float					const AtlasCharacterAspect;	///< Dimension of one image in atlas that would not be scaled

		//////////////////////////////////////////////////////////////////////////
		GDINL HUDCharactersAtlas(
			_In_ HRITexture2D const* const AtlasTexture,
			_In_ UInt32x2  const&		AtlasDimensions,
			_In_ float	   const		AtlasCharacterAspect
		) : HRIObject(),
			AtlasTexture(AtlasTexture), AtlasDimensions(AtlasDimensions), AtlasCharacterAspect(AtlasCharacterAspect)
		{ }

		//////////////////////////////////////////////////////////////////////////
		GDINL virtual ~HUDCharactersAtlas(
		) { }
	};

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	/// Namespace contains some HUD-events listeners classes
	namespace HUDElementPanelEvents
	{	
		/// Event being launched when mouse enters the zone of panel. 
		struct IOnMouseEnteredListener { GDINT virtual void OnMouseEntered(_In_opt_ chandle const Unused) abstract; };
		
		/// Event being launched when mouse leaves the zone of panel. 
		struct IOnMouseLeftListener	   { GDINT virtual void OnMouseLeft(_In_opt_ chandle const Unused) abstract; };
		
		/// Event being launched when user begins dragging this panel using left mouse button 
		struct IOnMouseDraggedListener { GDINT virtual void OnMouseDraged(_In_opt_ chandle const Unused) abstract; };

		/// Event being launched when user clicked on this panel with some mouse button 
		struct IOnMouseClickedListener { GDINT virtual void OnMouseClicked(_In_opt_ MouseButton const Button) abstract; };
	}	// namespace HUDElementPanelPrivate

#define protected public // For testing purposes only
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	/// Base class for all HUD elements. 
	class HUDElementPanel : public  HRIObject
	{
		GD_TYPEINFORMATION_DEFINITION(HUDElementPanel, HRIObject, GDAPI);
		friend class HUDResources;

	private:
		EventQueue<HUDElementPanel, chandle> OnRectangleParamChangedEvent;
		EventQueue<HUDElementPanel, chandle> OnRotationParamChangedEvent;
		EventQueue<HUDElementPanel, chandle> OnDepthParamChangedEvent;

		RefPtr<HRIShaderInstance> ElementPixelShaderInstance  = nullptr;
		RefPtr<HRIShaderInstance> ElementVertexShaderInstance = nullptr;

		// Vertex shader parameters
		Rectangle ElementRectangle			= Rectangle(0.0f, 0.0f, 1.0f, 1.0f);
		Float32	  ElementRotationDegrees	= Float32(0.0f);
		Int32	  ElementDepth				= 0;

		// Pixel shader parameters
		//	Texture parameters
		UInt32    ElementTextureIndex		= 0;
		/// @todo Add more texture customization here
		//	Character parameters
		String    ElementCharactersString	= String("Dummy Text");
		Float32	  ElementCharactersFontSize = Float32(1.0f);
		Color     ElementCharactersColor	= Color(0.96f, 0.96f, 1.0f, 1.0f);
		Vector2	  ElementCharactersPadding	= Vector2(0.0f, 0.15f);

	public:
		RefPtr<HUDElementPanel const>	 const ElementBasePanel;		///< Panel to which this one is attached to
		RefPtr<HUDTextureAtlas const>	 const ElementTextureAtlas;		///< Texture atlas this panel uses
		RefPtr<HUDCharactersAtlas const> const ElementCharactersAtlas;	///< Pre-rendered font atlas this panel uses

	protected:
		GDAPI explicit HUDElementPanel(
			_In_opt_ HUDElementPanel*		   const ElementBasePanel,
			_In_opt_ HUDTextureAtlas	const* const ElementTextureAtlas,
			_In_opt_ HUDCharactersAtlas const* const ElementCharactersAtlas
		);

		GDAPI virtual ~HUDElementPanel();

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		/// @name Panel Positioning
		/// @note All panel positioning is related to parent panel
		/// @{

		/// Position and scale of panel.
		/// Coordinate system is: down left corner is origin, Y - axis up, X - left.
		GDINL Rectangle const& GetElementRectangle() const { return self->ElementRectangle; }
		GDAPI void			   SetElementRectangle(_In_ Rectangle const& ElementRectangle);

		/// Rotation in Z axis of panel in degrees.
		/// Positive value is clockwise rotation, negative - counterclockwise.
		GDINL Float32		   GetElementRotationDegrees() const { return self->ElementRotationDegrees; }
		GDAPI void			   SetElementRotationDegrees(_In_ Float32 const ElementRotationDegrees);

		/// Z coordinate of this panel. Higher value - deeper panel.
		/// On GPU this values if converted in format (depth + 500.0f) / 1000.0f
		GDINL Int32			   GetElementDepth() const { return self->ElementDepth; }
		GDAPI void			   SetElementDepth(_In_ Int32 const ElementDepth);

		/// @}
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		/// @name Panel content management
		/// @{

	private:
		GDINT void			   SetElementTextureAtlas();

	protected:
		/// Id of the texture inside atlas
		GDINL UInt32		   GetElementTextureIndex() const { return self->ElementTextureIndex; }
		GDAPI void			   SetElementTextureIndex(_In_ UInt32 const ElementTextureIndex);

	private:
		GDINT void			   SetElementCharactersAtlas();

	protected:
		/// String containment of panel text. Note, that max length of this string is 64 characters
		GDINL String		   GetElementCharactersString() const { return self->ElementCharactersString; }
		GDAPI void			   SetElementCharactersString(_In_ String const& ElementCharactersString);

		/// Size multiplier of each character in text.
		GDINL Float32		   GetElementCharactersFontSize() const { return self->ElementCharactersFontSize; }
		GDAPI void			   SetElementCharactersFontSize(_In_ Float32 const ElementCharactersFontSize);

		/// The color of text in the panel
		GDINL Color const&	   GetElementCharactersColor() const { return self->ElementCharactersColor; }
		GDAPI void			   SetElementCharactersColor(_In_ Color const& ElementCharactersColor);

		/// Text starting position inside panel
		GDINL Vector2 const&   GetElementCharactersPadding() const { return self->ElementCharactersPadding; }
		GDAPI void			   SetElementCharactersPadding(_In_ Vector2 const& ElementCharactersPadding);

		/// @}
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////

	private: // Dummy methods for event system 
		GDINL void OnDepthParamChanged(chandle const Unused)     { self->SetElementDepth(self->GetElementDepth()); }
		GDINL void OnRotationParamChanged(chandle const Unused)  { self->SetElementRotationDegrees(self->GetElementRotationDegrees()); }
		GDINL void OnRectangleParamChanged(chandle const Unused) { self->SetElementRectangle(self->GetElementRectangle()); // Recomputing char size because it depends on rectangle
																   self->SetElementCharactersFontSize(self->GetElementCharactersFontSize()); }
	};	// class HUDElementPanel
#undef protected

	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	///// Class represents format for pre-rendered font used in HUD system
	//class HUDCharactersAtlas final : public HRIObject
	//{
	//	GD_TYPEINFORMATION_DEFINITION(HUDCharactersAtlas, HRIObject, GDAPI);
	//};

	class HUDResources final : public Object,
							   public Singleton<HUDResources>
	{
		GD_TYPEINFORMATION_DEFINITION(HUDResources, Object, GDAPI);

	public:
		HRILinkagePoint* HUDMountingPoint;

		GDAPI explicit HUDResources();
		GDAPI virtual ~HUDResources();

		GDAPI void RenderPanel(HUDElementPanel* const ElementPanel);

	private:
		GDINT void LoadHUDElementMesh();
		GDINT void LoadHUDElementShader();
	};

GD_NAMESPACE_END

#endif
