#include <GoddamnEngine/Engine/HUD/HUD.h>
#include <GoddamnEngine/Engine/Renderer/Shader/CrossCompiler/CrossCompiler.h>
#include <GoddamnEngine/Engine/Resource/Impl/Effect/Effect.h>

// Cool precision, yeah?
#define GD_RADIANS(Degrees) (0.01745329251994329576923690768489f * Degrees) 

GD_NAMESPACE_BEGIN

	namespace HUDElementPanelShaderValidation
	{
		static size_t const  RequiredParamsMaxCount = 20;
		static String const* RequiredVertexShaderParams[RequiredParamsMaxCount] = { nullptr };
		static String const* RequiredPixelShaderParams[RequiredParamsMaxCount] = { nullptr };

		struct RequiredPixelParam 
		{ 
			RequiredPixelParam(String const& Name) 
			{ 
				static size_t ParamIndex = SIZE_MAX; 
				RequiredVertexShaderParams[++ParamIndex] = &Name; 
			} 
		};	// struct RequiredPixelParam
	}	// namespace HUDElementPanelShaderValidation

	/// ==========================================================================================
	/// ==========================================================================================
	GD_TYPEINFORMATION_IMPLEMENTATION_C(HUDElementPanel, HRIObject, GDAPI, nullptr);
	
	/// ==========================================================================================
	HUDElementPanel::HUDElementPanel(
		_In_opt_ HUDElementPanel*		   const ElementBasePanel,
		_In_opt_ HUDTextureAtlas	const* const ElementTextureAtlas,
		_In_opt_ HUDCharactersAtlas const* const ElementCharactersAtlas
	) : HRIObject(HRIObject::TreeLockingFlagsAll, ((ElementBasePanel != nullptr)	// Object-specific
			? static_cast<HRIObject*>(ElementBasePanel) 
			: static_cast<HRIObject*>(&HUDResources::GetInstance()))),
		OnRectangleParamChangedEvent(&HUDElementPanel::OnRectangleParamChanged),	// Events
		OnRotationParamChangedEvent(&HUDElementPanel::OnRotationParamChanged),
		OnDepthParamChangedEvent(&HUDElementPanel::OnDepthParamChanged),
		ElementBasePanel(ElementBasePanel),											// Const params
		ElementTextureAtlas(ElementTextureAtlas), 
		ElementCharactersAtlas(ElementCharactersAtlas)
	{
		if (ElementBasePanel != nullptr)
		{	// Connecting to base panel to detect it repositioning
			ElementBasePanel->OnRectangleParamChangedEvent += self;
			ElementBasePanel->OnRotationParamChangedEvent  += self;
			ElementBasePanel->OnDepthParamChangedEvent     += self;
		}

		if ((self->ElementTextureAtlas.GetPointer() != nullptr) || (self->ElementCharactersAtlas.GetPointer() != nullptr))
		{	// Creating shader instances for this "mesh" instance
			// If we have nothing to render (e.g. for sizer HUD elements) there is no reason to create shader instances
			static HRIShaderProgram* const HUDShader = HUDResources::GetInstance().HUDMountingPoint->GetShaderProgram();
			self->ElementVertexShaderInstance = HRInterface::GetInstance().CreateShaderInstance(HUDShader->GetProgramVertexShader()->ShaderDesc);
			self->ElementPixelShaderInstance = HRInterface::GetInstance().CreateShaderInstance(HUDShader->GetProgramPixelShader()->ShaderDesc);

			// Reapplying default parameters to pass them to GPU:
			// Vertex-specific positioning parameters
			self->SetElementRectangle(self->GetElementRectangle());
			self->SetElementRotationDegrees(self->GetElementRotationDegrees());
			self->SetElementDepth(self->GetElementDepth());
			if (self->ElementTextureAtlas.GetPointer() != nullptr)
			{	// Texture-specific parameters
				self->SetElementTextureIndex(self->GetElementTextureIndex());
				self->SetElementTextureAtlas();
			}
			if (self->ElementCharactersAtlas.GetPointer() != nullptr)
			{	// Font-specific parameters
				self->SetElementCharactersString(self->GetElementCharactersString());
				self->SetElementCharactersFontSize(self->GetElementCharactersFontSize());
				self->SetElementCharactersColor(self->GetElementCharactersColor());
				self->SetElementCharactersPadding(self->GetElementCharactersPadding());
				self->SetElementCharactersAtlas();
			}

			self->ElementPixelShaderInstance->GetInstanceResourcesLocation()->UploadAllParameters();
		}
	}

	/// ==========================================================================================
	HUDElementPanel::~HUDElementPanel()
	{
		auto ElementBasePanel  = const_cast<HUDElementPanel*>(self->ElementBasePanel.GetPointer());
		if  (ElementBasePanel != nullptr)
		{	// Disconnecting to base panel
			ElementBasePanel->OnRectangleParamChangedEvent -= self;
			ElementBasePanel->OnRotationParamChangedEvent  -= self;
			ElementBasePanel->OnDepthParamChangedEvent	   -= self;
		}
	}

	/// ==========================================================================================
	static String const ElementRectangleName("HUDElementRectangle");
	void HUDElementPanel::SetElementRectangle(Rectangle const& ElementRectangle)
	{
		Rectangle ElementRectangleAbsolute = (self->ElementRectangle = ElementRectangle);
		for (HUDElementPanel const*
			 Panel =  self->ElementBasePanel.GetPointer(); Panel != nullptr;
			 Panel = Panel->ElementBasePanel.GetPointer())
		{	// Recomputing panel rectangle position as XY-translation and size as scaling to parent 
			ElementRectangleAbsolute = Rectangle(
				Vector2(ElementRectangleAbsolute.Left + Panel->ElementRectangle.Left,
						ElementRectangleAbsolute.Top  + Panel->ElementRectangle.Top),
				Vector2(ElementRectangleAbsolute.Width  * Panel->ElementRectangle.Width,
						ElementRectangleAbsolute.Height * Panel->ElementRectangle.Height)
			);
		}

		self->ElementVertexShaderInstance->GetParamByName(ElementRectangleName)->SetValue<Rectangle>(ElementRectangleAbsolute);
		self->OnRectangleParamChangedEvent.TriggerAndLaunchEvent();
	}
	
	/// ==========================================================================================
	static String const ElementRotationRadiansName("HUDElementRotationRadians");
	GDAPI void HUDElementPanel::SetElementRotationDegrees(Float32 const ElementRotationDegrees)
	{
		Float32 ElementRotationDegreesAbsolute = (self->ElementRotationDegrees = ElementRotationDegrees);
		for (HUDElementPanel const*
			 Panel = self->ElementBasePanel.GetPointer(); Panel != nullptr;
			 Panel = Panel->ElementBasePanel.GetPointer())
		{	// Recomputing panel rectangle position as rotation to parent 
			ElementRotationDegreesAbsolute += Panel->ElementRotationDegrees;
		}

		self->ElementVertexShaderInstance->GetParamByName(ElementRotationRadiansName)->SetValue<Float32>(GD_RADIANS(ElementRotationDegreesAbsolute));
		self->OnRotationParamChangedEvent.TriggerAndLaunchEvent();
	}
	
	/// ==========================================================================================
	static String const ElementDepthName("HUDElementDepth");
	GDAPI void HUDElementPanel::SetElementDepth(Int32 const ElementDepth)
	{
		Int32 ElementDepthAbsolute = (self->ElementDepth = ElementDepth);
		for (HUDElementPanel const* 
			 Panel =  self->ElementBasePanel.GetPointer(); Panel != nullptr; 
			 Panel = Panel->ElementBasePanel.GetPointer())
		{	// Recomputing panel rectangle position as Z-translation
			ElementDepthAbsolute += Panel->ElementDepth;
		}

		self->ElementVertexShaderInstance->GetParamByName(ElementDepthName)->SetValue<Float32>(static_cast<Float32>(ElementDepthAbsolute));
		self->OnDepthParamChangedEvent.TriggerAndLaunchEvent();
	}

	/// ==========================================================================================
	static String const ElementTextureAtlasName("HUDElementTextureAtlas");
	static String const ElementTextureAtlasDimensionsName("HUDElementTextureAtlasDimensions");
	static String const ElementTextureAtlasFixedZoneName("HUDElementTextureAtlasFixedZone");
	void HUDElementPanel::SetElementTextureAtlas()
	{
		GD_DEBUG_ASSERT((self->ElementTextureAtlas.GetPointer() != nullptr), "No texture atlas specified");

		self->ElementPixelShaderInstance->GetParamByName(ElementTextureAtlasName)->SetValue<HRITexture2D const*>(self->ElementTextureAtlas->AtlasTexture.GetPointer());
		self->ElementPixelShaderInstance->GetParamByName(ElementTextureAtlasDimensionsName)->SetValue<UInt32x2>(UInt32x2(self->ElementTextureAtlas->AtlasDimensions));
		self->ElementPixelShaderInstance->GetParamByName(ElementTextureAtlasFixedZoneName)->SetValue<Vector2>(self->ElementTextureAtlas->AtlasFixedZone);
	}

	/// ==========================================================================================
	static String const ElementTextureIndexName("HUDElementTextureIndexName");
	void HUDElementPanel::SetElementTextureIndex(_In_ UInt32 const ElementTextureIndex)
	{
		GD_DEBUG_ASSERT((self->ElementTextureAtlas.GetPointer() != nullptr), "No texture atlas specified");
		
		self->ElementTextureIndex = ElementTextureIndex;
		self->ElementPixelShaderInstance->GetParamByName(ElementTextureIndexName)->SetValue<UInt32>(ElementTextureIndex);
	}

	/// ==========================================================================================
	static String const ElementCharactersAtlasName("HUDElementCharactersAtlass");
	static String const ElementCharactersAtlasDimensionsName("HUDElementCharactersAtlasDimensions");
	void HUDElementPanel::SetElementCharactersAtlas()
	{
		GD_DEBUG_ASSERT((self->ElementCharactersAtlas.GetPointer() != nullptr), "No font atlas specified");

		self->ElementPixelShaderInstance->GetParamByName(ElementCharactersAtlasName)->SetValue<HRITexture2D const*>(self->ElementCharactersAtlas->AtlasTexture.GetPointer());
		self->ElementPixelShaderInstance->GetParamByName(ElementCharactersAtlasDimensionsName)->SetValue<UInt32x2>(UInt32x2(self->ElementCharactersAtlas->AtlasDimensions));
	}

	/// ==========================================================================================
	static String const ElementCharactersStringName("HUDElementCharactersString");
	static String const ElementCharactersStringLengthName("HUDElementCharactersStringLength");
	void HUDElementPanel::SetElementCharactersString(_In_ String const& ElementCharactersString)
	{
		GD_DEBUG_ASSERT((self->ElementCharactersAtlas.GetPointer() != nullptr), "No font atlas specified");
		
		self->ElementCharactersString = ElementCharactersString;
		self->ElementPixelShaderInstance->GetParamByName(ElementCharactersStringName)->SetValue(&ElementCharactersString[0]);
		self->ElementPixelShaderInstance->GetParamByName(ElementCharactersStringLengthName)->SetValue<UInt32>(static_cast<UInt32>(ElementCharactersString.GetSize()));
	}

	/// ==========================================================================================
	static String const HUDElementCharactersSizeName("HUDElementCharactersSize");
	void HUDElementPanel::SetElementCharactersFontSize(_In_ Float32 const ElementCharactersFontSize)
	{
		GD_DEBUG_ASSERT((self->ElementCharactersAtlas.GetPointer() != nullptr), "No font atlas specified");
		static Vector2 const ElementCharactersDefaultSize(0.0055f, 0.0105f);
			   Vector2 const ElementCharactersSize = ElementCharactersDefaultSize * ElementCharactersFontSize /*/ ElementAbsolutSize*/;
			   /// @todo add some math here

		self->ElementCharactersFontSize = ElementCharactersFontSize;
		self->ElementPixelShaderInstance->GetParamByName(HUDElementCharactersSizeName)->SetValue<Vector2>(ElementCharactersSize);
	}

	/// ==========================================================================================
	static String const HUDElementCharactersColorName("HUDElementCharactersColor");
	void HUDElementPanel::SetElementCharactersColor(_In_ Color const& ElementCharactersColor)
	{
		GD_DEBUG_ASSERT((self->ElementCharactersAtlas.GetPointer() != nullptr), "No font atlas specified");
		
		self->ElementCharactersColor = ElementCharactersColor;
		self->ElementPixelShaderInstance->GetParamByName(HUDElementCharactersColorName)->SetValue<Color>(ElementCharactersColor);
	}

	/// ==========================================================================================
	static String const ElementCharactersPaddingName("HUDElementCharactersPadding");
	void HUDElementPanel::SetElementCharactersPadding(_In_ Vector2 const& ElementCharactersPadding)
	{
		GD_DEBUG_ASSERT((self->ElementCharactersAtlas.GetPointer() != nullptr), "No font atlas specified");

		self->ElementCharactersPadding = ElementCharactersPadding;
		self->ElementPixelShaderInstance->GetParamByName(ElementCharactersPaddingName)->SetValue<Vector2>(ElementCharactersPadding);
	}

	/// ==========================================================================================
	/// ==========================================================================================
	/// ==========================================================================================

	/// ==========================================================================================
	/// ==========================================================================================
	GD_TYPEINFORMATION_IMPLEMENTATION(HUDResources, Object, GDAPI);
	GD_SINGLETON_IMPLEMENTATION(HUDResources);

	/// ==========================================================================================
	/// ==========================================================================================
	HUDResources::HUDResources()
	{
		self->HUDMountingPoint = HRInterface::GetInstance().CreateLinkagePoint();
		self->LoadHUDElementMesh();
		self->LoadHUDElementShader();
	}

	/// ==========================================================================================
	/// ==========================================================================================
	HUDResources::~HUDResources()
	{
		///!!! self->HUDElementMesh->RemoveReference();
		self->HUDMountingPoint->RemoveReference();
	}

	/// ==========================================================================================
	/// ==========================================================================================
	void HUDResources::LoadHUDElementMesh()
	{
		static Float32 const HUDElementMeshVertices[] = {
		   	-1.0f, -1.0f,  0.0f,		 1.0f,  1.0f,  0.0f,		 1.0f, -1.0f,  0.0f,
			 1.0f,  1.0f,  0.0f,		-1.0f, -1.0f,  0.0f,		-1.0f,  1.0f,  0.0f,
		};
		static UInt16 const HUDElementMeshIndices[] = {
			0, 1, 2,  
			3, 4, 5 
		};

		HRIIndexedShape* Shape = HRInterface::GetInstance().CreateIndexedShape();
		Shape->SetVertexData(GD_HRI_SEMANTIC_POSITION, HUDElementMeshVertices, GD_ARRAY_SIZE(HUDElementMeshVertices)                                  );
		Shape->SetIndexData(                           HUDElementMeshIndices,  GD_ARRAY_SIZE(HUDElementMeshIndices ), sizeof(HUDElementMeshIndices[0]));
		self->HUDMountingPoint->SetIndexedShape(Shape);
	}

	/// ==========================================================================================
	/// ==========================================================================================
	void HUDResources::LoadHUDElementShader()
	{
		auto const HUDShader = RSStreamer::GetInstance().LoadImmediately<RSEffect>("file://CommonResources/Shaders/HUDShader.hlsl.xml")->GetEffectShaderProgram();
		
		/*auto const VertexShaderInstance = HRInterface::GetInstance().CreateShaderInstance(ShaderInstanceCtorInfo(HUDShader->GetProgramVertexShader()->ShaderDesc));
		GD_ASSERT((VertexShaderInstance->GetParamByName("HUDElementRectangle") != nullptr), "Vertex shader param was not found");
		GD_ASSERT((VertexShaderInstance->GetParamByName("HUDElementRotationDegrees") != nullptr), "Vertex shader param was not found");
		GD_ASSERT((VertexShaderInstance->GetParamByName("HUDElementDepth") != nullptr), "Vertex shader param was not found");
		VertexShaderInstance->RemoveReference();

		auto const PixelShaderInstance = HRInterface::GetInstance().CreateShaderInstance(ShaderInstanceCtorInfo(HUDShader->GetProgramPixelShader()->ShaderDesc));
		PixelShaderInstance->RemoveReference();*/

		self->HUDMountingPoint->SetShaderProgram(HUDShader.GetPointer());
	}

	/// ==========================================================================================
	/// ==========================================================================================
	void HUDResources::RenderPanel(HUDElementPanel* const ElementPanel)
	{
		ElementPanel->ElementVertexShaderInstance->GetInstanceFirstConstantBuffersLocation()->UploadAllParameters();
		ElementPanel->ElementPixelShaderInstance->GetInstanceFirstConstantBuffersLocation()->UploadAllParameters();
		self->HUDMountingPoint->GetShaderProgram()->GetProgramVertexShader()->BindShader(ElementPanel->ElementVertexShaderInstance.GetPointer());
		self->HUDMountingPoint->GetShaderProgram()->GetProgramPixelShader()->BindShader(ElementPanel->ElementPixelShaderInstance.GetPointer());
		self->HUDMountingPoint->RenderSelf();
	}

GD_NAMESPACE_END
