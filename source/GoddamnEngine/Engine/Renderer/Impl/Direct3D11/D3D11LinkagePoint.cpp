/// ==========================================================================================
/// D3D11LinkagePoint.cpp: Linkage point D3D11 implementation.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 21.05.2014  - Created by James Jhuighuy
/// ==========================================================================================

#include <GoddamnEngine/Engine/Renderer/Impl/Direct3D11/D3D11LinkagePoint.h>
#include <GoddamnEngine/Engine/Renderer/Impl/Direct3D11/D3D11Buffer.h>

GD_NAMESPACE_BEGIN
	GD_TYPEINFORMATION_IMPLEMENTATION(HRID3D11LinkagePoint, HRILinkagePoint, GDINT);
	
	void HRID3D11LinkagePoint::RenderSelf() const
	{
		if (this->IsRelinkingRequired) {	// We need to relink vertex shader input layout with shape.
			this->IsRelinkingRequired = false;
			this->LinkingCache.Buffers.Emptify();
			this->LinkingCache.Strides.Emptify();
			this->LinkingCache.Offsets.Emptify();
			UInt64 const ShapeVertexFormat = this->GetShaderProgram()->GetProgramVertexShader()->ShaderDesc->InstanceInputFormat;
			for (size_t SemanticIter = GD_HRI_SEMANTIC_FIRST; SemanticIter != GD_HRI_SEMANTIC_UNKNOWN; SemanticIter += 1) {	// Setting up all upcoming semantic from shader.
				if ((ShapeVertexFormat & GD_BIT(SemanticIter + 1)) == 0) {	// This semantic is not used, we not need to mention it in layout.
					continue;
				}

				HRISemantic     const  Semantic = static_cast<HRISemantic>(SemanticIter);
				HRISemanticDesc const& SemanticDesc = HRISemanticGetDesc(Semantic);
				HRIVertexBuffer const* const VertexBuffer = this->IndexedShape->GetVertexBuffer(Semantic);
				if (VertexBuffer == nullptr) {
					throw HRID3D11Exception("No vertex buffer for required semantic exists.");
				}

				UINT const VerticesCount = static_cast<UINT>(VertexBuffer->GetSize()) / GD_FORMAT_COUNT_EXTRACT(SemanticDesc.SlotFormat);
				if (this->LinkingCache.VerticesCount == UINT_MAX) {
					this->LinkingCache.VerticesCount = VerticesCount;
				} else {
					if (this->LinkingCache.VerticesCount != VerticesCount) {
						throw HRID3D11Exception("Invalid vertices number.");
					}
				}

				this->LinkingCache.Buffers.PushLast(reinterpret_cast<ID3D11Buffer*>(VertexBuffer->GetNativePointer()));
				this->LinkingCache.Strides.PushLast(     static_cast<UINT         >(GD_FORMAT_SIZEOF(SemanticDesc.SlotFormat)));
				this->LinkingCache.Offsets.PushLast(0);
				this->LinkingCache.SlotsTotalCount += 1;
			}
		}

		ID3D11DeviceContext      * const Context = HRD3D11Interface::GetInstance().Context.Get();
		HRID3D11IndexBuffer const* const Buffer  = object_cast<HRID3D11IndexBuffer const*>(this->IndexedShape->GetIndexBuffer());
		Context->IASetVertexBuffers(0, this->LinkingCache.SlotsTotalCount, &this->LinkingCache.Buffers[0], &this->LinkingCache.Strides[0], &this->LinkingCache.Offsets[0]);
		if (Buffer != nullptr) {	// We have indices.
			Context->IASetIndexBuffer(Buffer->GetBufferPointer(), Buffer->GetIndexFormat(), 0);
			Context->DrawIndexed(static_cast<UINT>(Buffer->GetSize()), 0, 0);
		} else {	// No indices specified.
			Context->Draw(this->LinkingCache.VerticesCount, 0);
		}
	}

	HRILinkagePoint* HRD3D11Interface::CreateLinkagePoint()
	{
		return new HRID3D11LinkagePoint();
	}

GD_NAMESPACE_END
