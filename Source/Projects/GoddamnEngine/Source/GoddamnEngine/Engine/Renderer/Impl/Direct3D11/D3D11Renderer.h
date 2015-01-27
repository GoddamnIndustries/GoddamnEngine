/// ==========================================================================================
/// D3D11Renderer.h - Direct3D 11 Hardware renderer implementation.
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// 
/// History:
///		* 05.06.2014 - Created by James Jhuighuy
/// ==========================================================================================

#pragma once
#ifndef GD_ENGINE_RENDERER_IMPL_D3D11_RENDERER
#define GD_ENGINE_RENDERER_IMPL_D3D11_RENDERER

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Engine/Renderer/Renderer.h>

#if (defined (GD_D3DWIN7))
#	pragma warning(push)
#	pragma warning(disable: 4005)
#endif	// if (defined (GD_D3DWIN7))
#include <D3D11.h>
#include <D3D11.h>
#include <D3DCompiler.h>
#if (defined (GD_D3DWIN7))
#	pragma warning(pop)
#endif	// if (defined (GD_D3DWIN7))

GD_NAMESPACE_BEGIN

	/// Indicates about runtime error caused by renderer interface
	class HRID3D11Exception : public HRIException
	{
	public /*Public API*/:
		GDINL explicit HRID3D11Exception(String const& Message) : HRIException(Message.CStr()) { }
		GDINL virtual ~HRID3D11Exception() { }
	};	// class HRID3D11Exception

	/// Provides automatic releasing of Direct3D 11 objects.
	template<typename D3D11PointerType>
	class D3D11RefPtr final
	{
	private:
		GD_CLASS_UNCOPIABLE(D3D11RefPtr);
		D3D11PointerType* Pointer;

	public:
		GDINL  D3D11RefPtr(D3D11PointerType* const Pointer = nullptr) : Pointer(Pointer)    { }
		GDINL  D3D11RefPtr(D3D11RefPtr    && OtherPtr   ) : Pointer(OtherPtr.Pointer) { OtherPtr.Pointer = nullptr; }
		GDINL ~D3D11RefPtr()                   { if (Pointer != nullptr) this->Pointer->Release(); }

		GDINL D3D11PointerType* Get() const { return this->Pointer; }

		GDINL D3D11PointerType* const* GetAddressOf() const { return  (&this->Pointer); }
		GDINL D3D11PointerType*   * GetAddressOf()    { return  (&this->Pointer); }

		GDINL D3D11PointerType* Reset()
		{
			this->~D3D11RefPtr();
			D3D11PointerType* const Pointer = this->Pointer;
			this->Pointer = nullptr;
			return Pointer;
		}

		GDINL D3D11RefPtr& operator= (D3D11RefPtr const& OtherPtr) = delete;
		GDINL D3D11RefPtr& operator= (D3D11RefPtr  && OtherPtr)
		{
			if ((&OtherPtr) != this) {
				this->~D3D11RefPtr();
				this->Pointer = OtherPtr.Pointer;
				OtherPtr.Pointer = nullptr;
			}

			return *this;
		}

		GDINL D3D11PointerType* const* operator&  () const { return  (&this->Pointer); }
		GDINL D3D11PointerType*   * operator&  ()    { return  (&this->Pointer); }
		GDINL D3D11PointerType&  operator*  () const { return  (*this->Get()); }
		GDINL D3D11PointerType*  operator-> () const { return  ( this->Get()); }
		GDINL bool      operator== (D3D11PointerType const* const OtherPointer) const { return  (this->Get() == OtherPointer); }
		GDINL bool      operator!= (D3D11PointerType const* const OtherPointer) const { return !((*this) == OtherPointer); }
	};	// class D3D11RefPtr

	class HRD3D11Interface final : public HRInterface
	{
	private:
		GD_CLASSINFO_DEFINITION(HRD3D11Interface, HRInterface, GDINT);
		GD_SINGLETON_OVERRIDE(HRD3D11Interface, HRInterface);

	public:
		D3D11RefPtr<ID3D11Device     > Device;
		D3D11RefPtr<ID3D11DeviceContext > Context;
		D3D11RefPtr<IDXGISwapChain   > SwapChain;
		D3D11RefPtr<ID3D11RenderTargetView > RenderTargetView;
		D3D11RefPtr<ID3D11Texture2D  > DepthStencilBuffer;
		D3D11RefPtr<ID3D11DepthStencilState> DepthStencilState;
		D3D11RefPtr<ID3D11DepthStencilView > DepthStencilView;
		D3D11RefPtr<ID3D11RasterizerState  > SolidRasterizerState;

		GDINL explicit HRD3D11Interface() { }
		GDINL virtual ~HRD3D11Interface() { }

#define GD_HRI_API  GDINT virtual
#define GD_HRI_SPEC    override final
#include <GoddamnEngine/Engine/Renderer/RendererMethods.h>
	};

	/// ==========================================================================================
	GDINL static DXGI_FORMAT ToDxgiFormat(Format const format)
	{
		switch (format)
		{
			/// ==========================================================================================
			/// ==========================================================================================
		case GD_FORMAT_R8_UINT:				return DXGI_FORMAT_R8_UINT;
		case GD_FORMAT_R16_UINT:			return DXGI_FORMAT_R16_UINT;
		case GD_FORMAT_R32_UINT:			return DXGI_FORMAT_R32_UINT;
		//case GD_FORMAT_R64_UINT:			return DXGI_FORMAT_R64_UINT;

			/// ==========================================================================================
		case GD_FORMAT_R8G8_UINT:			return DXGI_FORMAT_R8G8_UINT;
		case GD_FORMAT_R16G16_UINT:			return DXGI_FORMAT_R16G16_UINT;
		case GD_FORMAT_R32G32_UINT:			return DXGI_FORMAT_R32G32_UINT;
		//case GD_FORMAT_R64G64_UINT:		return DXGI_FORMAT_R64G64_UINT;

			/// ==========================================================================================
		//case GD_FORMAT_R8G8B8_UINT:		return DXGI_FORMAT_R8G8B8_UINT;
		//case GD_FORMAT_R16G16B16_UINT:	return DXGI_FORMAT_R16G16B16_UINT;
		case GD_FORMAT_R32G32B32_UINT:		return DXGI_FORMAT_R32G32B32_UINT;
		//case GD_FORMAT_R64G64B64_UINT:	return DXGI_FORMAT_R64G64B64_UINT;
			
			/// ==========================================================================================
		case GD_FORMAT_R8G8B8A8_UINT:		return DXGI_FORMAT_R8G8B8A8_UINT;
		case GD_FORMAT_R16G16B16A16_UINT:	return DXGI_FORMAT_R16G16B16A16_UINT;
		case GD_FORMAT_R32G32B32A32_UINT:	return DXGI_FORMAT_R32G32B32A32_UINT;
		//case GD_FORMAT_R64G64B64A64_UINT: return DXGI_FORMAT_R64G64B64A64_UINT;

			/// ==========================================================================================
			/// ==========================================================================================
		case GD_FORMAT_R8_SINT:				return DXGI_FORMAT_R8_SINT;
		case GD_FORMAT_R16_SINT:			return DXGI_FORMAT_R16_SINT;
		case GD_FORMAT_R32_SINT:			return DXGI_FORMAT_R32_SINT;
		//case GD_FORMAT_R64_SINT:			return DXGI_FORMAT_R64_SINT;

			/// ==========================================================================================
		case GD_FORMAT_R8G8_SINT:			return DXGI_FORMAT_R8G8_SINT;
		case GD_FORMAT_R16G16_SINT:			return DXGI_FORMAT_R16G16_SINT;
		case GD_FORMAT_R32G32_SINT:			return DXGI_FORMAT_R32G32_SINT;
		//case GD_FORMAT_R64G64_SINT:		return DXGI_FORMAT_R64G64_SINT;

			/// ==========================================================================================
		//case GD_FORMAT_R8G8B8_SINT:		return DXGI_FORMAT_R8G8B8_SINT;
		//case GD_FORMAT_R16G16B16_SINT:	return DXGI_FORMAT_R16G16B16_SINT;
		case GD_FORMAT_R32G32B32_SINT:		return DXGI_FORMAT_R32G32B32_SINT;
		//case GD_FORMAT_R64G64B64_SINT:	return DXGI_FORMAT_R64G64B64_SINT;

			/// ==========================================================================================
		case GD_FORMAT_R8G8B8A8_SINT:		return DXGI_FORMAT_R8G8B8A8_SINT;
		case GD_FORMAT_R16G16B16A16_SINT:	return DXGI_FORMAT_R16G16B16A16_SINT;
		case GD_FORMAT_R32G32B32A32_SINT:	return DXGI_FORMAT_R32G32B32A32_SINT;
		//case GD_FORMAT_R64G64B64A64_SINT: return DXGI_FORMAT_R64G64B64A64_SINT;

			/// ==========================================================================================
			/// ==========================================================================================
		//case GD_FORMAT_R8_FLOAT:			return DXGI_FORMAT_R8_FLOAT;
		case GD_FORMAT_R16_FLOAT:			return DXGI_FORMAT_R16_FLOAT;
		case GD_FORMAT_R32_FLOAT:			return DXGI_FORMAT_R32_FLOAT;
		//case GD_FORMAT_R64_FLOAT:			return DXGI_FORMAT_R64_FLOAT;

			/// ==========================================================================================
		//case GD_FORMAT_R8G8_FLOAT:		return DXGI_FORMAT_R8G8_FLOAT;
		case GD_FORMAT_R16G16_FLOAT:		return DXGI_FORMAT_R16G16_FLOAT;
		case GD_FORMAT_R32G32_FLOAT:		return DXGI_FORMAT_R32G32_FLOAT;
		//case GD_FORMAT_R64G64_FLOAT:		return DXGI_FORMAT_R64G64_FLOAT;

			/// ==========================================================================================
		//case GD_FORMAT_R8G8B8_FLOAT:		return DXGI_FORMAT_R8G8B8_FLOAT;
		//case GD_FORMAT_R16G16B16_FLOAT:	return DXGI_FORMAT_R16G16B16_FLOAT;
		case GD_FORMAT_R32G32B32_FLOAT:		return DXGI_FORMAT_R32G32B32_FLOAT;
		//case GD_FORMAT_R64G64B64_FLOAT:	return DXGI_FORMAT_R64G64B64_FLOAT;

			/// ==========================================================================================
		//case GD_FORMAT_R8G8B8A8_FLOAT:	return DXGI_FORMAT_R8G8B8A8_FLOAT;
		case GD_FORMAT_R16G16B16A16_FLOAT:	return DXGI_FORMAT_R16G16B16A16_FLOAT;
		case GD_FORMAT_R32G32B32A32_FLOAT:	return DXGI_FORMAT_R32G32B32A32_FLOAT;
		//case GD_FORMAT_R64G64B64A64_FLOAT: return DXGI_FORMAT_R64G64B64A64_FLOAT;
		}

		return DXGI_FORMAT_UNKNOWN;
	}

	/// ==========================================================================================
	GDINL static DXGI_FORMAT ToDxgiFormatUnorm(Format const format)
	{
		switch (format)
		{
			/// ==========================================================================================
			/// ==========================================================================================
		case GD_FORMAT_R8_UINT:				return DXGI_FORMAT_R8_UNORM;
		case GD_FORMAT_R16_UINT:			return DXGI_FORMAT_R16_UNORM;

			/// ==========================================================================================
		case GD_FORMAT_R8G8_UINT:			return DXGI_FORMAT_R8G8_UNORM;
		case GD_FORMAT_R16G16_UINT:			return DXGI_FORMAT_R16G16_UNORM;

			/// ==========================================================================================
		case GD_FORMAT_R8G8B8A8_UINT:		return DXGI_FORMAT_R8G8B8A8_UNORM;
		case GD_FORMAT_R16G16B16A16_UINT:	return DXGI_FORMAT_R16G16B16A16_UNORM;

			/// ==========================================================================================
			/// ==========================================================================================
		case GD_FORMAT_R8_SINT:				return DXGI_FORMAT_R8_SNORM;
		case GD_FORMAT_R16_SINT:			return DXGI_FORMAT_R16_SNORM;

			/// ==========================================================================================
		case GD_FORMAT_R8G8_SINT:			return DXGI_FORMAT_R8G8_SNORM;
		case GD_FORMAT_R16G16_SINT:			return DXGI_FORMAT_R16G16_SNORM;

			/// ==========================================================================================
		case GD_FORMAT_R8G8B8A8_SINT:		return DXGI_FORMAT_R8G8B8A8_SNORM;
		case GD_FORMAT_R16G16B16A16_SINT:	return DXGI_FORMAT_R16G16B16A16_SNORM;

		default: return ToDxgiFormat(format);
		}
	}

GD_NAMESPACE_END

#endif
