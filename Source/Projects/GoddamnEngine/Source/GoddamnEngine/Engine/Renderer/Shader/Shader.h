/// ==========================================================================================
/// Shader.h - shaders common interface
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// 
/// History:
///		* --.01.2014 - Created by James Jhuighuy
///		* 06.02.2014 - Rewritten from scratch by James Jhuighuy,
///					   Functionality splitted to Shader and ShaderEx
///  * 05.05.2014 - Rewritten to match cross compiler
/// ==========================================================================================

#pragma once
#ifndef GD_ENGINE_RENDERER_ASSET_HRI_SHADER
#define GD_ENGINE_RENDERER_ASSET_HRI_SHADER

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Math/Math.h>
#include <GoddamnEngine/Core/Object/Object.h>
#include <GoddamnEngine/Core/Containers/String.h>
#include <GoddamnEngine/Core/Templates/RefPtr.h>
#include <GoddamnEngine/Core/Templates/UniquePtr.h>

#include <GoddamnEngine/Engine/Renderer/Format/Format.h>
// #include "Shader.Generated.h"

// Define this to get OpenGL-style name aliases.
#if (defined(GD_DOCUMENTATION) || ((!defined(GD_PLATFORM_XBOX_ONE)) || (!defined(GD_PLATFORM_WINPNONE8))))
#	 define  GD_HRI_SHADER_OPENGL_ALISASING	
#	 define  GD_HRI_SHADER_D3D11_EMULATING	
#endif	// if (defined(GD_DOCUMENTATION) || ((!defined(GD_PLATFORM_XBOX_ONE)) || (!defined(GD_PLATFORM_WINPNONE8))))

GD_NAMESPACE_BEGIN

	class HRIConstantBuffer;
	class HRITexture2D;
	class HRITextureCube;

	class HRIShaderParamLocationDesc;
	class HRIShaderParamLocation;
	class HRIShaderParamDesc;
	class HRIShaderParam;
	class HRIShaderInstanceDesc;
	class HRIShaderInstance;
	class HRIShaderProgram;
	class HRIShader;

#if (defined(GD_HRI_SHADER_D3D11_EMULATING))
	typedef void (*HRIShaderParamUploaderProc)(HRIShaderParam const* const Param);
#endif	// if (defined(GD_HRI_SHADER_D3D11_EMULATING))
	enum  : size_t { GD_HRI_SHADER_PARAM_DESC_ID_UNKNOWN = UINT_MAX };
	typedef size_t HRIShaderParamID;

	/// Describes some limits in shader subsystem.
	enum HRIShaderLimits : size_t 
	{
		/// Max size of shader param location in bytes.
		GD_HRI_SHADER_PARAM_MAX_LOCATION_SIZE = 16 * 1024,
		
		/// Max number of variables that can be stored inside location.
		GD_HRI_SHADER_PARAM_MAX_LOCATION_COUNT = 16,

		/// Max length of shader string parameter.
		GD_HRI_SHADER_PARAM_MAX_STRING_LENGTH = 64,

		/// Max constant buffer locations inside single shader.
		/// *	OpenGL case:
		///			Mainly, this limitation comes from OpenGL, where we cannot use same buffer registers in one program.
		///			Mostly GPU drivers max limitation in 84 uniform buffers per shader program. We have 6 shader stage ->
		///			14 uniform buffers per progragm. We leave a small buffering space (13 * 6 = 78) for uncommon drivers.
		GD_HRI_SHADER_MAX_CBUFFERS_LOCATIONS = 13
	};	// enum HRIShaderLimits

	/// Describes types of shaders.
	$GD_ENUMERATION(Type = Enumeration, ExportPolicy = Internal)
	enum HRIShaderType : size_t
	{
		GD_HRI_SHADER_TYPE_VERTEX,								///< Vertex shader type.
		GD_HRI_SHADER_TYPE_HULL,								///< Hull shader type.
		GD_HRI_SHADER_TYPE_DOMAIN,								///< Domain shader type.
		GD_HRI_SHADER_TYPE_PIXEL,								///< Pixel shader type.
		GD_HRI_SHADER_TYPE_COMPUTE,								///< Computing shader type.
		GD_HRI_SHADER_TYPE_GEOMETRY,							///< Geometry shader type.
		GD_HRI_SHADER_TYPE_UNKNOWN,								///< Unknown shader type (internal usage only).
		GD_HRI_SHADER_TYPES_COUNT								= GD_HRI_SHADER_TYPE_UNKNOWN,
#if (defined(GD_HRI_SHADER_OPENGL_ALISASING))
		GD_HRI_SHADER_TYPE_TESSELLATION_CONTROL					= GD_HRI_SHADER_TYPE_HULL,	 ///< Alias to hull shader (for OpenGL).
		GD_HRI_SHADER_TYPE_TESSELLATION_EVALUATION				= GD_HRI_SHADER_TYPE_DOMAIN, ///< Alias to domain shader (for OpenGL).
		GD_HRI_SHADER_TYPE_FRAGMENT								= GD_HRI_SHADER_TYPE_PIXEL,	 ///< Alias to pixel shader (for OpenGL).
#endif	// if (defined(GD_HRI_SHADER_OPENGL_ALISASING))
	};	// enum HRIShaderType

	/// Describes type of shader parameter
	$GD_ENUMERATION(Type = Enumeration, ExportPolicy = Internal)
	enum HRIShaderParamDescType : size_t
	{
		GD_HRI_SHADER_PARAM_DESC_TYPE_FORMATABLE,				///< Type of parameter can be represented as Format.
		GD_HRI_SHADER_PARAM_DESC_TYPE_MATRIX4X4,				///< Float32x4x4 (float32) parameter.
		GD_HRI_SHADER_PARAM_DESC_TYPE_MATRIX3X3,				///< float32x3x3 (float32) parameter.
		GD_HRI_SHADER_PARAM_DESC_TYPE_STRING,					///< String (ANSI Character set, length - 64 symbols) parameter.
		GD_HRI_SHADER_PARAM_DESC_TYPE_TEXTURE2D,				///< 2D texture parameter.
		GD_HRI_SHADER_PARAM_DESC_TYPE_TEXTURECUBE,				///< Cube texture parameter.
		GD_HRI_SHADER_PARAM_DESC_TYPE_UNKNOWN,					///< Unknown parameter type (internal usage only).
		GD_HRI_SHADER_PARAM_DESC_TYPES_COUNT					= GD_HRI_SHADER_PARAM_DESC_TYPE_UNKNOWN,
	};	// enum HRIShaderParamDescType

	/// Describing types of location of shader parameter
	$GD_ENUMERATION(Type = Enumeration, ExportPolicy = Internal)
	enum HRIShaderParamLocationDescType : size_t
	{
		GD_HRI_SHADER_PARAM_LOCATION_DESC_TYPE_RESOURCES,		///< Parameter is resource.
		GD_HRI_SHADER_PARAM_LOCATION_DESC_TYPE_CONSTANTBUFFER,	///< Parameter is located in constant buffer. On platforms without support of constant buffers they are emulated using uniforms.
		GD_HRI_SHADER_PARAM_LOCATION_DESC_TYPE_UNKNOWN,			///< Unknown parameter location (internal usage only).
		GD_HRI_SHADER_PARAM_LOCATION_DESC_TYPES_COUNT			= GD_HRI_SHADER_PARAM_LOCATION_DESC_TYPE_UNKNOWN,
		GD_HRI_SHADER_PARAM_LOCATION_DESC_TYPE_FIRST			= 0
	};	// enum HRIShaderParamLocationDescType

	/// Describes shader public parameter
	$GD_REFLECTABLE()
	class HRIShaderParamDesc final : public HRIObject
	{
		$GD_REFLECTABLE_BODY_GENERATED_CRAP();

	private:
#if (defined(GD_HRI_SHADER_D3D11_EMULATING))
		friend class HRIShaderParamLocationConstantBufferEmulated;
		mutable HRIShaderParamUploaderProc ParamUploader = nullptr;
#endif	// if (defined(GD_HRI_SHADER_D3D11_EMULATING))
		mutable HRIShaderParamID ParamID = GD_HRI_SHADER_PARAM_DESC_ID_UNKNOWN;
		mutable size_t     ParamCachedSize = SIZE_MAX;

	public:
		/// Name of this parameter.
		$GD_PROPERTY(ReadOnly)
		String const ParamName;

		/// Hash of parameter`s name.
	//	$GD_PROPERTY(ReadOnly, DefinedAs = Field)
		HashCode const ParamHash;

		/// Type of this parameter.
		$GD_PROPERTY(ReadOnly)
		HRIShaderParamDescType const ParamType;

		/// Format of this parameter. Unknown if type is not GD_HRI_SHADER_PARAM_DESC_TYPE_FORMATABLE.
		$GD_PROPERTY(ReadOnly)
		Format const ParamFormat;

		/// If this variable was defined as a array, array length.
		$GD_PROPERTY(ReadOnly)
		size_t const ParamArrayLength;

		GDINL virtual ~HRIShaderParamDesc();
		GDINL    HRIShaderParamDesc(
			HRIShaderParamLocationDesc* const  ParamLocationDesc,
			String       const& ParamName,
			HRIShaderParamDescType   const  ParamType,
			Format       const  ParamFormat   = GD_FORMAT_UNKNOWN,
			size_t       const  ParamArrayLength = 1,
			HRIShaderParamID   const  ParamID    = GD_HRI_SHADER_PARAM_DESC_ID_UNKNOWN
		);

		/// Returns instance description this param description uses.
		$GD_PROPERTY(ReadOnly, DefinedAs = Getter)
		GDINL HRIShaderParamLocationDesc const* GetLocationDesc() const;
		GDINL HRIShaderParamLocationDesc   * GetLocationDesc();

#if (defined(GD_HRI_SHADER_D3D11_EMULATING))
		/// Returns pointer to function that uploads this param to GPU while emulating constatant buffers functionality.
		GDINL HRIShaderParamUploaderProc GetParamUploader() const { return this->ParamUploader; }
#endif	// if (defined(GD_HRI_SHADER_D3D11_EMULATING))

		/// Returns unique shader param ID.
		$GD_PROPERTY(ReadOnly, DefinedAs = Getter)
		GDINL HRIShaderParamID GetParamID() const { return this->ParamID; }

		/// Returns size of parameter in bytes
		$GD_PROPERTY(ReadOnly, DefinedAs = Getter)
		GDINL size_t GetParamSize() const;
	};	// class HRIShaderParamDesc

	/// Since GPU workes better with floats we have to do so.
	typedef Float32 HRIShaderChar;	

	/// Instance of single parameter in shader
	$GD_REFLECTABLE(/*Serializable, Deserializable*/)
	class HRIShaderParam final : public HRIObject
	{
		$GD_REFLECTABLE_BODY_GENERATED_CRAP();

	private:
		friend  class HRIShaderParamLocation;
		mutable bool  DidValueChanged = true;
		union {
			handle  ParamPointer   = nullptr;
			HRIObject* ParamValue  /* = nullptr */;
		};	// union

	protected:
		GDAPI virtual ~HRIShaderParam(                       );
		GDAPI explicit HRIShaderParam(HRIShaderParamLocation* const ParamLocation, HRIShaderParamDesc const* const ParamDesc);

	public:
		/// Pointer to description of variable.
		$GD_PROPERTY(ReadOnly, DefinedAs = Field)
		RefPtr<HRIShaderParamDesc const> const ParamDesc; 

		/// Returns Domain in which this param exists.
		GDINL HRIShaderParamLocation const* GetShaderParamLocation() const;
		GDINL HRIShaderParamLocation   * GetShaderParamLocation();
		
		/// Returns next param to this with same properties.
		GDINL HRIShaderParam const* GetNextSameParam() const;
		GDINL HRIShaderParam   * GetNextSameParam();

		/// Returns pointer value that exists in shader.
		$GD_PROPERTY(DefinedAs = Getter, AlsoHas = Setter)
		GDINL handle GetValue() const { return this->ParamValue; }

		/// Applies value to shader param.
		template<typename HRIShaderParamValueType>	
		GDINL void SetValue(HRIShaderParamValueType const& ParamValue);
		GDAPI void SetValue(chandle     const  ParamValue);
	};	// class HRIShaderParam
	
	/// Description of single location type in shader instance desc.
	$GD_REFLECTABLE(/*Serializable, Deserializable*/)
	class HRIShaderParamLocationDesc final : public HRIObject
	{
		$GD_REFLECTABLE_BODY_GENERATED_CRAP();

	private:
		friend class HRIShaderParamDesc;
		size_t LocationInstancesCount = 0;
		size_t LocationInstancesSize  = 0; 

	public:
		/// Type of this location.
		$GD_PROPERTY(ReadOnly, DefinedAs = Field)
		HRIShaderParamLocationDescType const LocationType = GD_HRI_SHADER_PARAM_LOCATION_DESC_TYPE_UNKNOWN; 

		GDINL virtual ~HRIShaderParamLocationDesc(                            ) { }
		GDINL    HRIShaderParamLocationDesc(HRIShaderInstanceDesc* const LocationShaderInstanceDesc, HRIShaderParamLocationDescType const LocationType);

		/// Returns total number of location instances inside
		$GD_PROPERTY(ReadOnly, DefinedAs = Getter)
		GDINL size_t GetLocationInstancesCount() const { return this->LocationInstancesCount; }
		
		/// Returns total size of memory required for this location
		$GD_PROPERTY(ReadOnly, DefinedAs = Getter)
		GDINL size_t GetLocationInstancesSize() const { return this->LocationInstancesSize; }
	};	// class HRIShaderParamLocationDesc

	/// Description of single location in shader instance
	$GD_REFLECTABLE(/*Serializable, Deserializable*/)
	class HRIShaderParamLocation : public HRIObject
	{
		$GD_REFLECTABLE_BODY_GENERATED_CRAP();
	
	private:
		friend class HRIShaderParam;

	protected:
		mutable bool DidAnyParamValueChanged = true;

		/// Resets flag that indicated that specified param has uncommited value.
		/// Returns true if specified param has uncommited value.
		GDINL static bool QueryAndResetDidParamValueChanged(HRIShaderParam const* const Param) 
		{ 
			bool const DidValueChanged = Param->DidValueChanged;
			Param->DidValueChanged = false;
			return DidValueChanged;
		}

		GDINL static void SetParamPointer(HRIShaderParam* const Param, handle const ParamPointer)
		{
			Param->ParamPointer = ParamPointer;
		}

	public:
		/// Pointer to description of this location
		$GD_PROPERTY(ReadOnly)
		RefPtr<HRIShaderParamLocationDesc const> const LocationDesc = nullptr;

		GDAPI    HRIShaderParamLocation(HRIShaderInstance* const LocationInstance, HRIShaderParamLocationDesc const* const LocationDesc);
		GDAPI virtual ~HRIShaderParamLocation(                          );

		/// Returns instance that this location is attached to
		$GD_PROPERTY(ReadOnly, DefinedAs = Getter)
		GDINL HRIShaderInstance const* GetLocationInstance() const;
		GDINL HRIShaderInstance   * GetLocationInstance();

		/// Returns found param with specified name or nullptr
		GDAPI HRIShaderParam const* GetParamByName(String const& ParamName) const;
		GDAPI HRIShaderParam   * GetParamByName(String const& ParamName);

		/// Uploads all pre-cached parameters to GPU
		GDAPI virtual void UploadAllParameters() abstract;
	};	// class HRIShaderParamLocation
	
#if (defined(GD_HRI_SHADER_D3D11_EMULATING) && (!defined(GD_DOCUMENTATION)))
	class HRIShaderParamLocationConstantBufferBase : public HRIShaderParamLocation
	{
	private:
		UniquePtr<UInt8[]> LocationStorage = nullptr;

	protected:
		GDAPI explicit HRIShaderParamLocationConstantBufferBase(HRIShaderInstance* const ShaderInstance, HRIShaderParamLocationDesc const* const LocationDesc);
		GDINL virtual ~HRIShaderParamLocationConstantBufferBase(                        ) { }

		GDINL UInt8 const* GetStorage() const { return this->LocationStorage.GetPointer(); }
		GDINL UInt8   * GetStorage()    { return this->LocationStorage.GetPointer(); }
	};	// class HRIShaderParamLocationConstantBufferBase
#else	// if (defined(GD_HRI_SHADER_D3D11_EMULATING) && (!defined(GD_DOCUMENTATION)))
	typedef HRIShaderParamLocation HRIShaderParamLocationConstantBufferBase;
#endif	// if (defined(GD_HRI_SHADER_D3D11_EMULATING) && (!defined(GD_DOCUMENTATION)))

	/// Default implementation of constant buffer location.
	class HRIShaderParamLocationConstantBufferNative : public HRIShaderParamLocationConstantBufferBase
	{
	private:
		GD_TYPEINFORMATION_DEFINITION(HRIShaderParamLocationConstantBufferNative, HRIShaderParamLocation, GDAPI);
		HRIConstantBuffer* const LocationBuffer = nullptr; // Note, that we cannot use RefPtr here.

	public:
		GDAPI explicit HRIShaderParamLocationConstantBufferNative(HRIShaderInstance* const ShaderInstance, HRIShaderParamLocationDesc const* const LocationDesc);
		GDAPI virtual ~HRIShaderParamLocationConstantBufferNative(                        );
		GDAPI virtual void UploadAllParameters() final;

		/// Returns constant buffer this location uses.
		GDINL HRIConstantBuffer const* GetBuffer() const { return this->LocationBuffer; }
		GDINL HRIConstantBuffer   * GetBuffer()    { return this->LocationBuffer; }
	};	// class HRIShaderParamLocationConstantBufferNative

#if (defined(GD_HRI_SHADER_D3D11_EMULATING))
	/// Default implementation of constant buffer location for platforms that do not support constant buffers (e.g. OpenGL ES 2).
	/// Requires only one function (that uploads single parameter) to be implemented. Note that shader meta information
	/// is requred to be compiled with each paramter identifier.
	class HRIShaderParamLocationConstantBufferEmulated : public HRIShaderParamLocationConstantBufferBase
	{
	private:
		GD_TYPEINFORMATION_DEFINITION(HRIShaderParamLocationConstantBufferEmulated, HRIShaderParamLocation, GDAPI);

	protected:
		GDINL static void CacheParamID   (HRIShaderParam   * const Param, HRIShaderParamID     const ID   ) { Param->ParamDesc->ParamID = ID; }
		GDINL static void CacheParamUploader(HRIShaderParam   * const Param, HRIShaderParamUploaderProc const Uploader) { Param->ParamDesc->ParamUploader = Uploader; }

		GDINL explicit HRIShaderParamLocationConstantBufferEmulated(HRIShaderInstance* const ShaderInstance, HRIShaderParamLocationDesc const* const LocationDesc);
		GDINL virtual ~HRIShaderParamLocationConstantBufferEmulated(                        ) { }
		GDAPI virtual void UploadSingleParameter(HRIShaderParam const* const Param) const abstract;

	public:
		GDINL virtual void UploadAllParameters() final;
	};	// class HRIShaderParamLocationConstantBufferEmulated

	// There is constant buffers emulation, we cannot just use our implementation.
	using HRIShaderParamLocationConstantBuffer = HRIShaderParamLocationConstantBufferBase;
#else	// if (defined(GD_HRI_SHADER_D3D11_EMULATING))
	// There no constant buffers emulation, we can just use our implementation.
	using HRIShaderParamLocationConstantBuffer = HRIShaderParamLocationConstantBuffer;
#endif	// if (defined(GD_HRI_SHADER_D3D11_EMULATING))

	/// Defualt resources location implementation.
	class HRIShaderParamLocationResources : public HRIShaderParamLocation
	{
	private:
		GD_TYPEINFORMATION_DEFINITION(HRIShaderParamLocationResources, HRIShaderParamLocation, GDAPI);

	protected:
		GDINL explicit HRIShaderParamLocationResources(HRIShaderInstance* const ShaderInstance, HRIShaderParamLocationDesc const* const LocationDesc);
		GDINL virtual ~HRIShaderParamLocationResources(                        ) { }
	};	// class HRIShaderParamLocationResources

	/// Description of shader instance
	class HRIShaderInstanceDesc final : public HRIObject
	{
	private:
		GD_TYPEINFORMATION_DEFINITION(HRIShaderInstanceDesc, HRIObject, GDAPI);
	
	public:
		UInt64 const InstanceInputFormat;  ///< Semantic list that describes input list of primitives for shader.
		UInt64 const InstanceOutputFormat; ///< Semantic list that describes output list of primitives of shader.

		GDINL virtual ~HRIShaderInstanceDesc() { }
		GDINL    HRIShaderInstanceDesc(UInt64 const InstanceInputFormat, UInt64 const InstanceOutputFormat);
	
		/// Returns shader that this description is contained with
		GDINL HRIShader const* GetInstanceShader() const;
		GDINL HRIShader   * GetInstanceShader();

		/// Returns first description for shader location in instance desc or nullptr
		GDINL HRIShaderParamLocationDesc const* GetFirstLocationDesc(HRIShaderParamLocationDescType const InstanceLocationDescType) const;
		
		/// Returns next description for shader location or nullptr
		GDINL HRIShaderParamLocationDesc const* GetNextLocationDesc (HRIShaderParamLocationDescType const InstanceLocationDescType) const;
	};	// class HRIShaderInstanceDesc

	/// Container for value of public parameter of shader
	class HRIShaderInstance : public HRIObject
	{
		GD_TYPEINFORMATION_DEFINITION(HRIShaderInstance, HRIObject, GDAPI);
	
	private:
		friend class HRIShaderParamLocation;
		union {
			HRIShaderParamLocation* InstanceFirstLocations[GD_HRI_SHADER_PARAM_LOCATION_DESC_TYPES_COUNT];
			struct {
				HRIShaderParamLocationResources  * InstanceResourcesLocation;
				HRIShaderParamLocationConstantBuffer* InstanceFirstConstantBuffersLocation;
			};	// anonymous struct
		};	// anonymous union

	protected:
		GDINL virtual ~HRIShaderInstance(              ) { }
		GDAPI explicit HRIShaderInstance(HRIShaderInstanceDesc const* const InstanceDesc);

	public:
		RefPtr<HRIShaderInstanceDesc const> const InstanceDesc;

		/// Returns resources location attached to this instance.
		GDINL HRIShaderParamLocation const* GetInstanceResourcesLocation() const { return this->InstanceResourcesLocation; }
		GDINL HRIShaderParamLocation   * GetInstanceResourcesLocation()    { return this->InstanceResourcesLocation; }
		
		/// Returns globals location attached to this instance
		GDINL HRIShaderParamLocationConstantBuffer const* GetInstanceFirstConstantBuffersLocation() const { return this->InstanceFirstConstantBuffersLocation; }
		GDINL HRIShaderParamLocationConstantBuffer   * GetInstanceFirstConstantBuffersLocation()    { return this->InstanceFirstConstantBuffersLocation; }

		/// Returns parameter by name or nullptr if it was not found.
		GDINL HRIShaderParam const* GetParamByName(String const& ParamName) const;
		GDINL HRIShaderParam   * GetParamByName(String const& ParamName);
	};	// class HRIShaderInstance

	/// Contains data required for shader construction
	struct HRIShaderCtorInfo final
	{
		GD_CLASS_UNASSIGNABLE(HRIShaderCtorInfo);
		GD_CLASS_UNSWAPPABLE (HRIShaderCtorInfo);
		GD_CLASS_UNMOVABLE   (HRIShaderCtorInfo);

	public:
		HRIShaderProgram*  const  CreationShaderProgram; ///< Shader to which this shader is attached to.
		chandle      const  CreationData;			///< Pointer to memory that contains shader data.
		size_t    const  CreationDataSize;		///< Length of shader data.
		HRIShaderInstanceDesc const* CreationInstanceDesc;	///< HRIShader instance description.

		GDINL HRIShaderCtorInfo(HRIShaderCtorInfo const& CtorInfo);
		GDINL HRIShaderCtorInfo(
			HRIShaderProgram*  const  Effect,
			chandle      const  CreationData,
			size_t    const  CreationDataSize,
			HRIShaderInstanceDesc const* CreationInstanceDesc
		);
	};	// struct HRIShaderCtorInfo

	/// Basic class for shaders
	class HRIShader : public HRIObject
	{
	private:
		GD_TYPEINFORMATION_DEFINITION(HRIShader, HRIObject, GDAPI);

	protected:
		GDINL virtual ~HRIShader(                 );
		GDINL    HRIShader(HRIShaderCtorInfo const& CtorInfo, HRIShaderType const ShaderType);

	public:
		HRIShaderType    const Type;		///< Type of this shader.
		HRIShaderInstanceDesc const* const ShaderDesc;	///< Pointer to description of this shader.

		/// Returns shader program to which this shader is attached to
		GDINL HRIShaderProgram const* GetShaderProgram() const;
		GDINL HRIShaderProgram   * GetShaderProgram();

		/// Binds shader to be used while next rendering
		GDAPI virtual void   BindShader(HRIShaderInstance const* const ShaderInstance) const abstract;
		/// Unbinds shader.
		GDAPI virtual void UnbindShader(            ) const abstract;
	};	// class HRIShader

#define GD_DEFINE_SHADER_TYPE(HRIShaderTypeName, HRIShaderType) \
	class HRIShaderTypeName : public HRIShader { public: \
		GDINL virtual ~HRIShaderTypeName(         ) { } \
		GDINL    HRIShaderTypeName(HRIShaderCtorInfo const& CtorInfo) : HRIShader(CtorInfo, HRIShaderType) { } \
	};	// class HRIShaderTypeName
	GD_DEFINE_SHADER_TYPE(HRIComputeShader  , GD_HRI_SHADER_TYPE_COMPUTE );
	GD_DEFINE_SHADER_TYPE(HRIGeometryShader , GD_HRI_SHADER_TYPE_GEOMETRY);
	GD_DEFINE_SHADER_TYPE(HRIVertexShader   , GD_HRI_SHADER_TYPE_VERTEX  );
	GD_DEFINE_SHADER_TYPE(HRIHullShader  , GD_HRI_SHADER_TYPE_HULL );
	GD_DEFINE_SHADER_TYPE(HRIDomainShader   , GD_HRI_SHADER_TYPE_DOMAIN  );
	GD_DEFINE_SHADER_TYPE(HRIPixelShader , GD_HRI_SHADER_TYPE_PIXEL   );
#undef GD_DEFINE_SHADER_TYPE

#if (defined(GD_HRI_SHADER_OPENGL_ALISASING))
	typedef HRIPixelShader  HRIFragmentShader;
	typedef HRIHullShader	HRITessellationControlShader;
	typedef HRIDomainShader HRITessellationEvaluationShader;
#endif	// if (defined(GD_HRI_SHADER_OPENGL_ALISASING))

	/// HRIShader Program contains several shader required for rendering pipeline.
	class HRIShaderProgram : public HRIObject
	{
	private:
		GD_TYPEINFORMATION_DEFINITION(HRIShaderProgram, HRIObject, GDAPI);
		friend class HRIShader;
		union {
			HRIShader* ProgramShaders[GD_HRI_SHADER_TYPES_COUNT];
			struct {
				HRIVertexShader  * ProgramVertexShader;
				HRIHullShader * ProgramHullShader;
				HRIDomainShader  * ProgramDomainShader;
				HRIPixelShader   * ProgramPixelShader;
				HRIComputeShader * ProgramComputeShader;
				HRIGeometryShader* ProgramGeometryShader;
			};	// anonymous struct
		};	// anonymous union

	protected:
		GDINL virtual ~HRIShaderProgram() { }
		GDINL explicit HRIShaderProgram();

	public:
		/// Returns shader attached to this program by type
		GDINL HRIShader const* GetProgramShader(HRIShaderType const HRIShaderType) const;
		GDINL HRIShader   * GetProgramShader(HRIShaderType const HRIShaderType);

		/// Returns compute shader attached to this program
		GDINL HRIComputeShader  const* GetProgramComputeShader () const { return this->ProgramComputeShader; }
		GDINL HRIComputeShader    * GetProgramComputeShader ()    { return this->ProgramComputeShader; }

		/// Returns geometry shader attached to this program
		GDINL HRIGeometryShader const* GetProgramGeometryShader() const { return this->ProgramGeometryShader; }
		GDINL HRIGeometryShader   * GetProgramGeometryShader()    { return this->ProgramGeometryShader; }

		/// Returns vertex shader attached to this program
		GDINL HRIVertexShader   const* GetProgramVertexShader  () const { return this->ProgramVertexShader; }
		GDINL HRIVertexShader  * GetProgramVertexShader  ()    { return this->ProgramVertexShader; }
		
		/// Returns Hull shader attached to this program
		GDINL HRIHullShader  const* GetProgramHullShader () const { return this->ProgramHullShader; }
		GDINL HRIHullShader    * GetProgramHullShader ()    { return this->ProgramHullShader; }
#if (defined(GD_HRI_SHADER_OPENGL_ALISASING))
#	if (!defined(GetProgramTessellationControlShader))
#	  define  GetProgramTessellationControlShader GetProgramHullShader
#	else	// if (!defined(GetProgramTessellationControlShader))
#		 error "'GetProgramTessellationControlShader' Identifier was already defined, please remove if or disable OpenGL naming aliasing."
#	endif	// if (!defined(GetProgramTessellationControlShader))
#endif	// if (defined(GD_HRI_SHADER_OPENGL_ALISASING))

		/// Returns vertex shader attached to this program
		GDINL HRIDomainShader const* GetProgramDomainShader () const { return this->ProgramDomainShader; }
		GDINL HRIDomainShader   * GetProgramDomainShader ()    { return this->ProgramDomainShader; }
#if (defined(GD_HRI_SHADER_OPENGL_ALISASING))
#	if (!defined(GetProgramTessellationEvaluationShader))
#	  define  GetProgramTessellationEvaluationShader GetProgramDomainShader
#	else	// if (!defined(GetProgramTessellationEvaluationShader))
#		 error '"GetProgramTessellationEvaluationShader" Identifier was already defined, please remove if or disable OpenGL naming aliasing.'
#	endif	// if (!defined(GetProgramTessellationEvaluationShader))
#endif	// if (defined(GD_HRI_SHADER_OPENGL_ALISASING))

		/// Returns vertex shader attached to this program
		GDINL HRIPixelShader const* GetProgramPixelShader   () const { return this->ProgramPixelShader; }
		GDINL HRIPixelShader   * GetProgramPixelShader   ()    { return this->ProgramPixelShader; }
#if (defined(GD_HRI_SHADER_OPENGL_ALISASING))
#	if (!defined(GetProgramFragmentShader))
#	  define  GetProgramFragmentShader GetProgramPixelShader
#	else	// if (!defined(GetProgramFragmentShader))
#		 error "'GetProgramFragmentShader' Identifier was already defined, please remove if or disable OpenGL naming aliasing."
#	endif	// if (!defined(GetProgramFragmentShader))
#endif	// if (defined(GD_HRI_SHADER_OPENGL_ALISASING))
	};	// class HRIShaderProgram

GD_NAMESPACE_END

#include <GoddamnEngine/Engine/Renderer/Shader/Shader.inl>
#endif
