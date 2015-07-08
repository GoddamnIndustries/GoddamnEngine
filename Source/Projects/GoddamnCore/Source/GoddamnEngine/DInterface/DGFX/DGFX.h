// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
* @file GoddamnEngine/DInterface/DGFX.h
* File contains export interface for the engine graphics subsystem.
*/
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/DInterface/DInterface.h>
#include <GoddamnEngine/Core/Containers/Vector.h>
#include <GoddamnEngine/Core/Geometry.h>
#include <GoddamnEngine/Core/Containers/String.h>
#include <GoddamnEngine/Core/UniquePtr.h>

GD_NAMESPACE_BEGIN

	enum DGFXDriver : Int32
	{
		DGFX_DRIVER_UNKNOWN,
		DGFX_DRIVER_MANTLE,
		DGFX_DRIVER_METAL,
		DGFX_DRIVER_DIRECTX12,
		DGFX_DRIVER_VULKAN,
		DGFX_DRIVER_DIRECTX11,
		DGFX_DRIVER_OPENGL,
		DGFX_DRIVER_OPENGLES,
		DGFX_DRIVER_EMPTY,
	};	// enum DGFXDriver

	enum DGFXOutputMode : Int32
	{
		DGFX_OUTPUT_MODE_UNKNOWN,
		DGFX_OUTPUT_MODE_WINDOWED,
		DGFX_OUTPUT_MODE_PSEUDO_FULLSCREEN,
		DGFX_OUTPUT_MODE_FULLSCREEN,
	};	// enum DGFXOutputMode

	enum DGFXOutputVsyncMode : Int32
	{
		DGFX_OUTPUT_VSYNC_UNKNOWN,
		DGFX_OUTPUT_VSYNC_DISABLED,
		DGFX_OUTPUT_VSYNC_30FPS,
		DGFX_OUTPUT_VSYNC_60FPS,
	};	// enum DGFXOutputVsyncMode 

	enum DFGXBufferType : Int32
	{
		DGFX_BUFFER_TYPE_UNKNOWN,
		DGFX_BUFFER_TYPE_VERTEX,
		DGFX_BUFFER_TYPE_INDEX,
		DGFX_BUFFER_TYPE_CONSTANT,
	};	// enum DFGXBufferType

	enum DGFXTopologyType : Int32
	{
		DGFX_TOPOLOGY_TYPE_UNKNOWN,
		DGFX_TOPOLOGY_TYPE_TRAINGLELIST,
	};	// enum DGFXTopologyType

	enum DGFXSemanticType : UInt8
	{
		DGFX_SEMANTIC_UNKNOWN,
		DGFX_SEMANTIC_POSITION,
		DGFX_SEMANTIC_NORMAL,
		DGFX_SEMANTIC_TANGENT,
		DGFX_SEMANTIC_BINORMAL,
		DGFX_SEMANTIC_TEXCOORD,
		DGFX_SEMANTIC_BLENDINDICES,
		DGFX_SEMANTIC_BLENDWEIGHT,
		DGFX_SEMANTIC_COLOR,
		DGFX_SEMANTICS_COUNT = DGFX_SEMANTIC_COLOR + 1
	};	// enum HRISemantic

	enum DGFXShaderType : Int32
	{
		DGFX_SHADER_TYPE_UNKNOWN,
		DGFX_SHADER_TYPE_VERTEX,
		DGFX_SHADER_TYPE_PIXEL,
	};	// enum DGFXShaderType

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Type to handle all objects, that were allocated by a Graphics device.
	//! Invalid handles are the one, that equal null pointer value.
	GD_DALIAS("struct _DGFXHandleType { byte Unused; } alias DGFXHandle = DGFXHandleType_;")
	typedef struct _DGFXHandleType { Byte Unused; }* DGFXHandle;

	// ------------------------------------------------------------------------------------------
	//! Provides a dynamic cross-platform API for the renderer's interface.
	//! Also provides default implementations for methods - can be treated as a empty GFX device
	//! interface.
	GD_DINTERFACE("extern(C++): public interface DGFX : DRuntimeInterface")
	struct DGFX : public DRuntimeInterface
	{
		struct _DGFXHandle : public IVirtuallyDestructible { };

	protected:
		DGFXOutputMode                 _GfxOutputMode          = DGFX_OUTPUT_MODE_UNKNOWN;
		DGFXOutputVsyncMode	           _GfxVsyncMode           = DGFX_OUTPUT_VSYNC_UNKNOWN;
		GeomResolution const*          _GfxResolutionSelected  = nullptr;
		Vector<GeomResolution>         _GfxResolutionsList;
		Vector<UniquePtr<_DGFXHandle>> _GfxHandles;

	protected:

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Functionality to handle graphics objects.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		template<typename _DGFXHandleType>
		GDINL typename EnableIf<TypeTraits::IsBase<_DGFXHandle, _DGFXHandleType>::Value, DGFXHandle>::Type
			_Handle_InitializeObject(_DGFXHandle* const _Handle)
		{
			union _IndexToHandle { SizeTp ConvIndex; DGFXHandle ConvHandle; };	// union _IndexToHandle
			_IndexToHandle const _HdvConv = { _GfxHandles.InsertLast(_Handle) + 1 };
			return _HdvConv.ConvHandle;
		}

		template<typename _DGFXHandleType>
		GDINL typename EnableIf<TypeTraits::IsBase<_DGFXHandle, _DGFXHandleType>::Value, _DGFXHandleType*>::Type
			_Handle_DereferenceObject(DGFXHandle const _Handle)
		{
			union _HandleToIndex { DGFXHandle ConvHandle; SizeTp ConvIndex; };	// union _IndexToHandle
			_HandleToIndex const _HdvConv = { _Handle };
			return static_cast<_DGFXHandleType*>(_GfxHandles[_HdvConv.ConvIndex - 1].GetPointer());
		}

	public:
		
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Output functionality.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// We can support dynamic resolution changing only on Desktop platforms.
		GD_DCODE("version(Desktop) {");
#if GD_PLATFORM_DESKTOP

		// ... Output device mode ...

		// ------------------------------------------------------------------------------------------
		//! Retrieves the mode in which graphics output device is running. 
		//! @returns The mode in which graphics output device is running.
		GD_DFUNCTION("public DGFXOutputMode Output_GetMode() @nogc nothrow")
		GDAPI virtual DGFXOutputMode Output_GetMode() const
		{
			return DGFX_OUTPUT_MODE_UNKNOWN;
		}

		// ------------------------------------------------------------------------------------------
		//! Changes the mode in which the graphics output device is running. 
		//! @param GfxOutputMode The new output mode that would be set.
		//! @param GfxForcelyReapply Do forcedly reapply mode, even if device is running is the same mode.
		//! @returns Non-negative value if the operation succeeded.
		GD_DFUNCTION("public DIResult Output_SetMode(DGFXOutputMode GfxOutputMode"
			", bool GfxForcelyReapply = false) @nogc nothrow")
		GDAPI virtual DIResult Output_SetMode(DGFXOutputMode const GfxOutputMode
			, bool const GfxForcelyReapply = false)
		{
			GD_NOT_USED_L(GfxOutputMode, GfxForcelyReapply);
			return DIResult::NotImplemented;
		}

		// ... Output device vertical synchronization ...

		// ------------------------------------------------------------------------------------------
		//! Returns mode, in which graphics device is performing vertical sync.
		//! @returns Mode, in which graphics device is performing vertical sync.
		GD_DFUNCTION("public DGFXOutputMode Output_GetVsyncMode() @nogc nothrow")
		GDAPI virtual DGFXOutputVsyncMode Output_GetVsyncMode() const
		{
			return DGFX_OUTPUT_VSYNC_UNKNOWN;
		}

		// ------------------------------------------------------------------------------------------
		//! Changes mode, in which graphics device is performing vertical sync.
		//! @param GfxVsyncMode The mode in which graphics device is performing vertical sync.
		//! @param GfxForcelyReapply Do forcedly reapply mode, even if vsync is running is the same mode.
		//! @returns Non-negative value if the operation succeeded.
		GD_DFUNCTION("public DIResult Output_SetVsyncEnabled(DGFXOutputVsyncMode GfxVsyncMode"
			", bool GfxForcelyReapply = false) @nogc nothrow")
		GDAPI virtual DIResult Output_SetVsyncEnabled(DGFXOutputVsyncMode const GfxVsyncMode
			, bool const GfxForcelyReapply = false)
		{
			GD_NOT_USED_L(GfxVsyncMode, GfxForcelyReapply);
			return DIResult::NotImplemented;
		}

		// ... Output device resolutions ...

		// ------------------------------------------------------------------------------------------
		//! Returns a vector reference with all supported resolution descriptors.
		//! @returns A vector reference with all supported resolution descriptors.
		GD_DFUNCTION("public DVectorRef!GeomResolution Output_GetResolution() @nogc nothrow")
		GDAPI virtual DVectorRef<GeomResolution> Output_GetSupportedResolutions() const
		{
			//! @todo Provide dummy implementation.
			GD_NOT_IMPLEMENTED();
		}
	
		// ------------------------------------------------------------------------------------------
		//! Returns pointer to the resolution descriptor that is currently set.
		//! @returns Pointer to the resolution descriptor that is currently set or null on failure.
		GD_DFUNCTION("public const(GeomResolution)* Output_GetResolution() @nogc nothrow")
		GDAPI virtual GeomResolution const* Output_GetResolution() const
		{
			return nullptr;
		}

		// ------------------------------------------------------------------------------------------
		//! Changes resolution in which graphics output device is rendering and resize several frame
		//! buffers (those, that have a corresponding flag specified).
		//! @param GfxResolution Pointer to the resolution descriptor that would be set.
		//! @param GfxForcelyReapply Do forcedly resolution mode, even if device is running is the same resolution.
		//! @returns Non-negative value if the operation succeeded.
		GD_DFUNCTION("public DIResult Output_SetResolution(const(GeomResolution)* GfxResolution"
			", bool GfxForcelyReapply = false) @nogc nothrow")
		GDAPI virtual DIResult Output_SetResolution(GeomResolution const* const GfxResolution
			, bool const GfxForcelyReapply = false)
		{
			GD_NOT_USED_L(GfxResolution, GfxForcelyReapply);
			return DIResult::NotImplemented;
		}

#endif	// if GD_PLATFORM_DESKTOP
		GD_DCODE("}\t//version(Desktop)");

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Common functionality.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Graphics device <-> CPU buffers functions.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Creates a GPU buffer with specified parameters.
		//! @param GfxBufferPtr Pointer for output.
		//! @param GfxBufferType Type of the buffer: vertex, index, constant or etc buffer.
		//! @param GfxBufferSize Size of the buffer in bytes.
		//! @param GfxBufferInitialData Initial data of the buffer. Can be null pointer for en empty buffer.
		//! @returns Non-negative value if the operation succeeded.
		GD_DFUNCTION("public DIResult Gfx_BufferCreate(DGFXHandle* GfxBufferPtr, DFGXBufferType GfxBufferType"
			", size_t GfxBufferSize, const(void)* GfxBufferInitialData) @nogc nothrow")
		GDAPI virtual DIResult Gfx_BufferCreate(DGFXHandle* const GfxBufferPtr, DFGXBufferType const GfxBufferType
			, SizeTp const GfxBufferSize, CHandle const GfxBufferInitialData)
		{
			GD_NOT_USED_L(GfxBufferPtr, GfxBufferType, GfxBufferSize, GfxBufferInitialData);
			return DIResult::NotImplemented;
		}

		// ------------------------------------------------------------------------------------------
		//! Maps a GPU buffer onto CPU memory.
		//! @param GfxBuffer Buffer to map.
		//! @returns Pointer to mapped memory or null pointer, if failed.
		GD_DFUNCTION("public void* Gfx_BufferMap(DGFXHandle GfxBuffer) @nogc nothrow")
		GDAPI virtual Handle Gfx_BufferMap(DGFXHandle const GfxBuffer)
		{
			GD_NOT_USED_L(GfxBuffer);
			return nullptr;
		}

		// ------------------------------------------------------------------------------------------
		//! Unmaps a GPU buffer and uploads data back to GPU memory.
		//! @param GfxBuffer Buffer to unmap.
		GD_DFUNCTION("public void Gfx_BufferUnmap(DGFXHandle GfxBuffer) @nogc nothrow")
		GDAPI virtual void Gfx_BufferUnmap(DGFXHandle const GfxBuffer)
		{
			GD_NOT_USED_L(GfxBuffer);
		}

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Shape objects.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Created a new shape that contains indexed vertices.
		//! @param GfxShapePtr Pointer to the output handle.
		//! @param GfxTopologyType Type of topology that shape represents.
		//! @returns Non-negative value if the operation succeeded.
		GD_DFUNCTION("public DIResult Gfx_ShapeCreate(DGFXHandle* GfxBufferPtr, DGFXTopologyType GfxTopologyType) @nogc nothrow")
		GDAPI virtual DIResult Gfx_ShapeCreate(DGFXHandle* const GfxShapePtr, DGFXTopologyType const GfxTopologyType)
		{
			GD_NOT_USED_L(GfxShapePtr, GfxTopologyType);
			return DIResult::NotImplemented;
		}

		// ------------------------------------------------------------------------------------------
		//! Attaches a new vertex buffer into indexed shape object. The order of the attached buffers
		//! should match the corresponding order in the vertex shader input layout.
		//! @param GfxShape Current shape handle.
		//! @param GfxVertexBuffer Vertex buffer to be attached.
		//! @param GfxSemanticType Type of the semantic this buffer represents.
		//! @param GfxSemanticIndex Index of the specified semantic.
		//! @returns Non-negative value if the operation succeeded.
		GD_DFUNCTION("public DIResult Gfx_ShapeAttachVertexBuffer(DGFXHandle GfxShape, DGFXHandle GfxVertexBuffer) @nogc nothrow")
		GDAPI virtual DIResult Gfx_ShapeAttachVertexBuffer(DGFXHandle const GfxShape
			, DGFXHandle const GfxVertexBuffer, DGFXSemanticType const GfxSemanticType, Int32 const GfxSemanticIndex)
		{
			GD_NOT_USED_L(GfxShape, GfxVertexBuffer, GfxSemanticType, GfxSemanticIndex);
			return DIResult::NotImplemented;
		}

		// ------------------------------------------------------------------------------------------
		//! Attaches an index buffer to this indexed shape. If the index buffer was already attached
		//! to shape, changes it.
		//! @param GfxShape Current shape handle.
		//! @param GfxIndexBuffer Index buffer to be attached.
		//! @returns Non-negative value if the operation succeeded.
		GD_DFUNCTION("public DIResult Gfx_ShapeAttachVertexBuffer(DGFXHandle GfxShape, DGFXHandle GfxIndexBuffer) @nogc nothrow")
		GDAPI virtual DIResult Gfx_ShapeAttachIndexBuffer(DGFXHandle const GfxShape, DGFXHandle const GfxIndexBuffer)
		{
			GD_NOT_USED_L(GfxShape, GfxIndexBuffer);
			return DIResult::NotImplemented;
		}

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Shader objects.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ... shaders and shader programs ...

		// ------------------------------------------------------------------------------------------
		//! Creates a GPU shader with specified parameters.
		//! @param GfxShaderPtr Pointer for output.
		//! @param GfxShaderType Shader type.
		//! @param GfxShaderByteCode Pointer to the data of the shader byte code.
		//! @param GfxShaderByteCodeLength Length of the shader byte code data in bytes.
		//! @returns Non-negative value if the operation succeeded.
		GD_DFUNCTION("public DIResult Gfx_ShaderCreate(DGFXHandle* GfxShaderPtr, DGFXShaderType GfxShaderType"
			", const(void)* GfxShaderByteCode, size_t GfxShaderByteCodeLength) @nogc nothrow")
		GDAPI virtual DIResult Gfx_ShaderCreate(DGFXHandle* const GfxShaderPtr
			, DGFXShaderType const GfxShaderType, CHandle const GfxShaderByteCode, SizeTp const GfxShaderByteCodeLength)
		{
			GD_NOT_USED_L(GfxShaderPtr, GfxShaderType, GfxShaderByteCode, GfxShaderByteCodeLength);
			return DIResult::NotImplemented;
		}

		// ------------------------------------------------------------------------------------------
		//! Creates a GPU shader program that can link together several shaders.
		//! @param GfxShaderProgramPtr Pointer for output.
		//! @returns Non-negative value if the operation succeeded.
		GD_DFUNCTION("public DIResult Gfx_ShaderProgramCreate(DGFXHandle* GfxShaderProgramPtr) @nogc nothrow")
		GDAPI virtual DIResult Gfx_ShaderProgramCreate(DGFXHandle* const GfxShaderProgramPtr)
		{
			GD_NOT_USED(GfxShaderProgramPtr);
			return DIResult::NotImplemented;
		}

		// ------------------------------------------------------------------------------------------
		//! Attaches existing shader into the shader program. Any attached shaders should be deleted
		//! manually after shader program.
		//! @param GfxShaderProgram Current shader program.
		//! @param GfxShader Shader that would be attached.
		//! @param GfxShaderType Shader type.
		GD_DFUNCTION("public void Gfx_ShaderProgramAttach(DGFXHandle GfxShaderProgram, DGFXHandle GfxShader"
			", DGFXShaderType GfxShaderType) @nogc nothrow")
		GDAPI virtual void Gfx_ShaderProgramAttach(DGFXHandle const GfxShaderProgram, DGFXHandle const GfxShader
			, DGFXShaderType const GfxShaderType)
		{
			GD_NOT_USED_L(GfxShaderProgram, GfxShader, GfxShaderType);
		}

		// ------------------------------------------------------------------------------------------
		//! Links all attached shader into the complete shader program.
		//! @param GfxShaderProgram Current shader program.
		GD_DFUNCTION("public void Gfx_ShaderProgramAttach(DGFXHandle GfxShaderProgram) @nogc nothrow")
		GDAPI virtual void Gfx_ShaderProgramAttach(DGFXHandle const GfxShaderProgram)
		{
			GD_NOT_USED(GfxShaderProgram);
		}

		// ... shader instances & uniforms management ...

		// ------------------------------------------------------------------------------------------
		//! Creates an object, that represents the separate set of uniform buffers and uniform 
		//! variables of the shader program.
		//! @param GfxShaderInstancePtr Pointer for output.
		//! @param GfxShaderProgram Current shader handle.
		//! @returns Non-negative value if the operation succeeded.
		GD_DFUNCTION("public DIResult Gfx_ShaderInstanceCreate(DGFXHandle* GfxShaderInstancePtr"
			", DGFXHandle GfxShader) @nogc nothrow")
		GDAPI virtual DIResult Gfx_ShaderInstanceCreate(DGFXHandle* const GfxShaderInstancePtr	
			, DGFXHandle const GfxShaderProgram)
		{
			GD_NOT_USED_L(GfxShaderInstancePtr, GfxShaderProgram);
			return DIResult::NotImplemented;
		}

		// ------------------------------------------------------------------------------------------
		//! Finds a named uniform buffer inside shader instance.
		//! @param GfxShaderInstance Current shader instance handle.
		//! @param GfxUniformBufferName Name of the buffer to query.
		//! @returns Handle of the found uniform buffer or invalid handle.
		GD_DFUNCTION("public DGFXHandle Gfx_ShaderInstanceQueryUniformBuffer(DGFXHandle GfxShaderInstance"
			", const(char)* GfxUniformBufferName) @nogc nothrow")
		GDAPI virtual DGFXHandle Gfx_ShaderInstanceQueryUniformBuffer(DGFXHandle const GfxShaderInstance	
			, CStr const GfxUniformBufferName)
		{
			GD_NOT_USED_L(GfxShaderInstance, GfxUniformBufferName);
			return nullptr;
		}

#if 0	// Need a cool API with concrete parameter types - scalar\vector\matrix and textures.
		// ------------------------------------------------------------------------------------------
		//! Finds a named uniform param inside shader instance.
		//! @param GfxShaderInstance Current shader instance handle.
		//! @param GfxUniformParamName Name of the param to query.
		//! @returns Pointer to the memory, to which data could be written, or null pointer.
		GD_DFUNCTION("public void* Gfx_ShaderInstanceQueryUniformParam(DGFXHandle GfxShaderInstance"
			", const(char)* GfxUniformParamName) @nogc nothrow")
		GDAPI virtual Handle Gfx_ShaderInstanceQueryUniformParam(DGFXHandle const GfxShaderInstance,		
			CStr const GfxUniformParamName)
		{
			GD_NOT_USED_L(GfxShaderInstance, GfxUniformParamName);
			return nullptr;
		}
#endif	// if 0

		// ... bindings ...

		// ------------------------------------------------------------------------------------------
		//! Makes specified shader program current active shader program.
		//! @param GfxShader Current shader handle. May be null pointer to unbind current shader program.
		//! @param GfxShaderInstance Current shader instance handle. May be null pointer.
		GD_DFUNCTION("public void Gfx_ShaderProgramMakeActive(DGFXHandle GfxShader"
			", DGFXHandle GfxShaderInstance) @nogc nothrow")
		GDAPI virtual void Gfx_ShaderProgramMakeActive(DGFXHandle const GfxShader, DGFXHandle const GfxShaderInstance)
		{
			GD_NOT_USED_L(GfxShader, GfxShaderInstance);
		}

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Linkage point.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// ------------------------------------------------------------------------------------------
		//! Creates a linkage point - internal object that provides linking shapes and shaders together.
		//! @param GfxLinkagePointPtr Pointer for output.
		//! @returns Non-negative value if the operation succeeded.
		GD_DFUNCTION("public DIResult Gfx_LinkagePointCreate(DGFXHandle* GfxLinkagePointPtr) @nogc nothrow")
		GDAPI virtual DIResult Gfx_LinkagePointCreate(DGFXHandle* const GfxLinkagePointPtr)
		{
			GD_NOT_USED(GfxLinkagePointPtr);
			return DIResult::NotImplemented;
		}

		// ------------------------------------------------------------------------------------------
		//! Changes a shape, that was attached to the linkage point.
		//! @param GfxLinkagePoint Current linkage point.
		//! @param GfxShape Mesh to be set.
		GD_DFUNCTION("public void Gfx_LinkagePointSetShape(DGFXHandle GfxLinkagePoint"
			", DGFXHandle GfxShape) @nogc nothrow")
		GDAPI virtual void Gfx_LinkagePointSetShape(DGFXHandle const GfxLinkagePoint, DGFXHandle const GfxShape)
		{
			GD_NOT_USED_L(GfxLinkagePoint, GfxShape);
		}

	};	// struct DGFX

GD_NAMESPACE_END
