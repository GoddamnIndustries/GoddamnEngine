/// ==========================================================================================
/// OGLLinkagePoint.cpp: Linkage point OGL implementation.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 21.05.2014  - Created by James Jhuighuy
/// ==========================================================================================

#include <GoddamnEngine/Engine/Renderer/Impl/OpenGL/OGLLinkagePoint.h>
#include <GoddamnEngine/Engine/Renderer/Impl/OpenGL/OGLBuffer.h>

GD_NAMESPACE_BEGIN
	GD_TYPEINFORMATION_IMPLEMENTATION(HRIOGLLinkagePoint, HRILinkagePoint, GDINT);

	HRIOGLVertexBuffer* vb = nullptr;
	HRIOGLIndexBuffer* ib = nullptr;
	bool static Init = false;

	void HRIOGLLinkagePoint::RenderSelf() const
	{
		auto const& GL = HROGLInterface::GetInstance().Driver;
		if (self->IsRelinkingRequired) {
			self->IsRelinkingRequired = false;
			if (self->LinkingCache.VertexArrayObject != 0) {	// Linked previous time, vertex array exists.
				GL.DeleteVertexArrays(1, &self->LinkingCache.VertexArrayObject);
				self->LinkingCache.VertexArrayObject = 0;
			}

			GL.GenVertexArrays(1, &self->LinkingCache.VertexArrayObject);
			GD_HRI_OGL_CHECK_ERRORS("Failed to generate Vertex Array Object");
			GL.BindVertexArray(self->LinkingCache.VertexArrayObject);
			GD_HRI_OGL_CHECK_ERRORS("Failed to bind Vertex Array Object");

			// Vertex attributes.
			GLuint       ShapeVertexAttribute = 0;
			UInt64 const ShapeVertexFormat = self->GetShaderProgram()->GetProgramVertexShader()->ShaderDesc->InstanceInputFormat;
			for (size_t SemanticIter = GD_HRI_SEMANTIC_FIRST; SemanticIter != GD_HRI_SEMANTIC_UNKNOWN; SemanticIter += 1) {	// Setting up all upcoming semantic from shader.
				if ((ShapeVertexFormat & GD_BIT(SemanticIter + 1)) == 0) {	// This semantic is not used, we not need to mention it in layout.
					continue;
				}

				HRISemantic               const  Semantic = static_cast<HRISemantic>(SemanticIter);
				HRISemanticDesc           const& SemanticDesc = HRISemanticGetDesc(Semantic);
				HRIOGLVertexBuffer const* const  VertexBuffer = object_cast<HRIOGLVertexBuffer const*>(self->IndexedShape->GetVertexBuffer(Semantic));
				if (VertexBuffer == nullptr) {
					throw HRIOGLException("No vertex buffer for required semantic exists.");
				}

				GLsizei const VerticesCount = static_cast<GLsizei>(VertexBuffer->GetSize()) / GD_FORMAT_COUNT_EXTRACT(SemanticDesc.SlotFormat);
				if (self->LinkingCache.VerticesCount == -1) {
					self->LinkingCache.VerticesCount = VerticesCount;
				} else {
					if (self->LinkingCache.VerticesCount != VerticesCount) {
						throw HRIOGLException("Invalid vertices count.");
					}
				}

				GL.EnableVertexAttribArray(ShapeVertexAttribute);
				GD_HRI_OGL_CHECK_ERRORS("Failed to enable Vertex attribute array ('EnableVertexAttribArray' failed)");
				VertexBuffer->BindBuffer();
				GL.VertexAttribPointer(ShapeVertexAttribute, static_cast<GLint>(GD_FORMAT_COUNT_EXTRACT(SemanticDesc.SlotFormat)), GL_FLOAT, GL_FALSE, 0, nullptr);
				GD_HRI_OGL_CHECK_ERRORS("Failed to link Vertex buffer with Vertex Attrib Array ('VertexAttribPointer' failed)");
			
				++ShapeVertexAttribute;
			}
			
			// Index buffer.
			HRIOGLIndexBuffer const* const IndexBuffer = object_cast<HRIOGLIndexBuffer const*>(self->IndexedShape->GetIndexBuffer());
			if (IndexBuffer != nullptr) {
				IndexBuffer->BindBuffer();
				size_t const Stride = IndexBuffer->GetStride();
				if (Stride == 2) {
					self->LinkingCache.IndexType = GL_UNSIGNED_SHORT;
				} else if (Stride == 4) {
					self->LinkingCache.IndexType = GL_UNSIGNED_INT;
				} else {
					throw HRIOGLException("Unimplemented Index buffer Stride");
				}
			}
		}

		self->ShaderProgram->BindShaderProgram();
		GL.BindVertexArray(self->LinkingCache.VertexArrayObject);
		GD_HRI_OGL_CHECK_ERRORS("Failed to bind Vertex Array Object");
		
		HRIOGLIndexBuffer const* const IndexBuffer = object_cast<HRIOGLIndexBuffer const*>(self->IndexedShape->GetIndexBuffer());
		if (IndexBuffer != nullptr) {
			GL.DrawElements(GL_TRIANGLES, static_cast<GLsizei>(IndexBuffer->GetSize()), self->LinkingCache.IndexType, nullptr);
			GD_HRI_OGL_CHECK_ERRORS("Failed to Render indexed elements ('DrawElements' failed)");
		} else {
			GL.DrawArrays(GL_TRIANGLES, 0, self->LinkingCache.VerticesCount);
			GD_HRI_OGL_CHECK_ERRORS("Failed to Render vertex array ('DrawArrays' failed)");
		}

		GL.BindVertexArray(0);
		self->ShaderProgram->UnbindShaderProgram();

#if 0	// Debug code. Leave it be here for a while.
		Float32 const static Vertices[] = {
			0.0f, 0.0f, 0.5f,
			1.0f, 0.0f, 0.5f,
			0.0f, 1.0f, 0.5f,
		};
		UInt16 const static Indices[] = {
			0, 1, 2
		};
		if (!Init) {
			Init = true;


			vb = new HRIOGLVertexBuffer(Vertices, GD_ARRAY_SIZE(Vertices));
			ib = new HRIOGLIndexBuffer(Indices, GD_ARRAY_SIZE(Indices), sizeof(Indices[0]));

			GLuint vao;
			GL.GenVertexArrays(1, &vao);
			GL.BindVertexArray(vao);
		}
		self->ShaderProgram->BindShaderProgram();
		GL.EnableVertexAttribArray(0);
		GD_HRI_OGL_CHECK_ERRORS("EnableVertexAttribArray");
		vb->BindBuffer();
		GL.VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		GD_HRI_OGL_CHECK_ERRORS("VertexAttribPointer");
		ib->BindBuffer();
		GL.DrawElements(GL_TRIANGLES, GD_ARRAY_SIZE(Indices), GL_UNSIGNED_SHORT, nullptr);
		GD_HRI_OGL_CHECK_ERRORS("DrawArrays");
		self->ShaderProgram->UnbindShaderProgram();
#endif	// if 0	// Debug code.
	}

	HRILinkagePoint* HROGLInterface::CreateLinkagePoint()
	{
		return new HRIOGLLinkagePoint();
	}

GD_NAMESPACE_END
