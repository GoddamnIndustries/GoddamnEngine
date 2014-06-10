#include <GoddamnEngine/Engine/Resource/StaticMesh/StaticMesh.h>
#include <GoddamnEngine/Engine/Renderer/Renderer.h>

GD_NAMESPACE_BEGIN

	//////////////////////////////////////////////////////////////////////////
	// StaticMesh Class
	//////////////////////////////////////////////////////////////////////////

	GD_TYPEINFORMATION_IMPLEMENTATION(StaticMesh, Resource, GDAPI, GD_RESOURCE_ARGS);

	StaticMesh::StaticMesh(String const& identifier) :
		Resource(identifier)
	{
		self->levelsOfDetails.PushLast(HRInterface::GetInstance().CreateIndexedShape());
		self->bindedLevelOfDetails = 0;
	}

	StaticMesh::~StaticMesh()
	{
		self->SetNumberOfLods(1);
	}

	//////////////////////////////////////////////////////////////////////////
	//  LOD management
	//////////////////////////////////////////////////////////////////////////

	size_t StaticMesh::GetNumberOfLods() const
	{
		return self->levelsOfDetails.GetSize();
	}

	void StaticMesh::SetNumberOfLods(const size_t numberOfLods)
	{
		GD_ASSERT(numberOfLods >= 1, "Number of LODS should be in [1; +infinity)");

		/**/ if (numberOfLods > self->GetNumberOfLods())
		{
			self->levelsOfDetails.Resize(numberOfLods);
			for (size_t cnt = self->GetNumberOfLods(); cnt < numberOfLods; cnt += 1)
			{
				self->levelsOfDetails[cnt] = HRInterface::GetInstance().CreateIndexedShape();
			}
		}
		else if (numberOfLods < self->GetNumberOfLods())
		{
			for (size_t cnt = numberOfLods; cnt < self->GetNumberOfLods(); cnt += 1)
			{
				self->levelsOfDetails[cnt]->RemoveReference();
			}

			self->levelsOfDetails.Resize(numberOfLods);
		}
	}

	void StaticMesh::DestroyLodAt(const size_t lod)
	{
		GD_ASSERT((lod != 0), "Unable to delete LOD in level 0");
		GD_ASSERT((lod < self->GetNumberOfLods()), "Unable to delete LOD in level 0");

		self->levelsOfDetails[lod]->RemoveReference();
		self->levelsOfDetails.RemoveElementAt(lod);
	}

	HRIIndexedShape* StaticMesh::GetLodAt(const size_t lod)
	{
		GD_ASSERT((lod < self->GetNumberOfLods()), "LOD at this level doesn't exists");
		return self->levelsOfDetails[lod];
	}

	const HRIIndexedShape* StaticMesh::GetLodAt(const size_t lod) const
	{
		GD_ASSERT((lod < self->GetNumberOfLods()), "LOD at this level doesn't exists");
		return self->levelsOfDetails[lod];
	}
		
	void StaticMesh::SetLodAt(const size_t lod, const HRIIndexedShape* submesh)
	{
		/*GD_ASSERT((lod < self->GetNumberOfLods()), "LOD at this level doesn't exists");
		
		HRIIndexedShape* existingSubmesh = const_cast<HRIIndexedShape*>(self->GetLodAt(lod));
		if (submesh == nullptr)
		{
			existingSubmesh->SetIndexData (                         nullptr, 0, 0);
			existingSubmesh->SetVertexData(GD_HRI_SEMANTIC_UNKNOWN, nullptr, 0   );
			return;
		}

		for (size_t cnt = GD_HRI_SEMANTIC_FIRST; cnt < GD_HRI_SEMANTICS_COUNT; cnt += 1)
		{
			const Vector<float>& vertexData = submesh->GetVerticesDataBySemantic(cnt);
			existingSubmesh->SetVerticesDataBySemantic(cnt, vertexData);
		}

		existingSubmesh->SetIndexData(submesh->GetIndicesData());*/
		GD_NOT_IMPLEMENTED();
	}

	void StaticMesh::SwitchToLod(const size_t lod)
	{
		self->GetLodAt(lod);
		self->bindedLevelOfDetails = lod;
	}

	//////////////////////////////////////////////////////////////////////////
	// LoadShekel_* functions
	//////////////////////////////////////////////////////////////////////////

	void LoadShekel_01(ResourceLoader* const resourceLoader, HRIIndexedShape* const staticMesh)
	{
		//////////////////////////////////////////////////////////////////////////
		// Shekels(R) in version 0.1 are simple: 
		// * Header (12 bytes)
		// * Number of vertices (4 bytes)
		// * Number of indices (4 bytes)
		// * Vertices array (Number of vertices * 12), Vector3 array
		// * Normals array (Number of vertices * 12), Vector3 array
		// * Tangents array (Number of vertices * 12), Vector3 array
		// * Bitangents array (Number of vertices * 12), Vector3 array
		// * UV-Coordinates array (Number of vertices * 8), Vector2 array
		// * Indices array (Number of indices * 2), ushort array
		//////////////////////////////////////////////////////////////////////////

		UInt32 verticesNumber = 0;
		resourceLoader->Read(&verticesNumber, sizeof(verticesNumber));

		UInt32 indicesNumber = 0;
		resourceLoader->Read(&indicesNumber, sizeof(indicesNumber));

		Vector<Float32> verticesBuffer;
		verticesBuffer.Resize(static_cast<size_t>(verticesNumber) * sizeof(Vector3) / sizeof(Float32));

		resourceLoader->Read(&verticesBuffer[0], sizeof(Vector3) * static_cast<size_t>(verticesNumber));
		staticMesh->SetVertexData(GD_HRI_SEMANTIC_POSITION, &verticesBuffer[0], verticesBuffer.GetSize());

		resourceLoader->Read(&verticesBuffer[0], sizeof(Vector3) * static_cast<size_t>(verticesNumber));
		staticMesh->SetVertexData(GD_HRI_SEMANTIC_NORMAL, &verticesBuffer[0], verticesBuffer.GetSize());

		resourceLoader->Read(&verticesBuffer[0], sizeof(Vector3) * static_cast<size_t>(verticesNumber));
		staticMesh->SetVertexData(GD_HRI_SEMANTIC_TANGENT, &verticesBuffer[0], verticesBuffer.GetSize());

		resourceLoader->Read(&verticesBuffer[0], sizeof(Vector3) * static_cast<size_t>(verticesNumber));
		staticMesh->SetVertexData(GD_HRI_SEMANTIC_BINORMAL, &verticesBuffer[0], verticesBuffer.GetSize());

		verticesBuffer.Resize(static_cast<size_t>(verticesNumber) * sizeof(Vector2) / sizeof(Float32));
		resourceLoader->Read(&verticesBuffer[0], sizeof(Vector2) * static_cast<size_t>(verticesNumber));
		staticMesh->SetVertexData(GD_HRI_SEMANTIC_TEXCOORD, &verticesBuffer[0], verticesBuffer.GetSize());

		Vector<UInt16> indicesBuffer;
		indicesBuffer.Resize(static_cast<size_t>(indicesNumber));
		resourceLoader->Read(&indicesBuffer[0], static_cast<size_t>(indicesNumber) * sizeof(UInt16));
		staticMesh->SetIndexData(&indicesBuffer[0], indicesBuffer.GetSize(), sizeof(indicesBuffer[0]));
	}

	void LoadShekel_10(ResourceLoader* const resourceLoader, HRIIndexedShape* const staticMesh)
	{
		GD_ASSERT(false, "Unimplemented");
	}

	//////////////////////////////////////////////////////////////////////////
	// Messaging functions
	//////////////////////////////////////////////////////////////////////////

	void StaticMesh::OnResourceLoad(ResourceLoader* const resourceLoader)
	{
		//////////////////////////////////////////////////////////////////////////
		// Attempting to check if this file is Shekel(R)
		struct ShekelHeader
		{
			char format[10];
			ShekelVersion version;
		} shekelHeader;
		
		resourceLoader->Read(&shekelHeader, sizeof(shekelHeader));
		GD_ASSERT((strcmp(&shekelHeader.format[0], "SHEKEL??!") == 0), 
			"Shekel(R) format validation failed.");

		switch (shekelHeader.version)
		{
		case ShekelVersion::Version01: { LoadShekel_01(resourceLoader, self->GetLodAt(0)); } break;
		case ShekelVersion::Version10: { LoadShekel_10(resourceLoader, self->GetLodAt(0)); } break;
		default: GD_ASSERT(false, "Unsupported Shekel(R) version");
		}
	}
	
	void StaticMesh::OnResourceUnload()
	{
	}

GD_NAMESPACE_END
