#include <GoddamnEngine/Engine/Resource/Impl/StaticMesh/StaticMesh.h>
#include <GoddamnEngine/Engine/Renderer/Renderer.h>

GD_NAMESPACE_BEGIN

	/// ==========================================================================================
	// StaticMesh Class
	/// ==========================================================================================

	GD_TYPEINFORMATION_IMPLEMENTATION(StaticMesh, Resource, GDAPI, GD_RESOURCE_ARGS);

	StaticMesh::StaticMesh(String const& identifier) :
		Resource(identifier)
	{
		this->levelsOfDetails.PushLast(HRInterface::GetInstance().CreateIndexedShape());
		this->bindedLevelOfDetails = 0;
	}

	StaticMesh::~StaticMesh()
	{
		this->SetNumberOfLods(1);
	}

	/// ==========================================================================================
	//  LOD management
	/// ==========================================================================================

	size_t StaticMesh::GetNumberOfLods() const
	{
		return this->levelsOfDetails.GetLength();
	}

	void StaticMesh::SetNumberOfLods(const size_t numberOfLods)
	{
		GD_ASSERT(numberOfLods >= 1, "Number of LODS should be in [1; +infinity)");

		/**/ if (numberOfLods > this->GetNumberOfLods())
		{
			this->levelsOfDetails.Resize(numberOfLods);
			for (size_t cnt = this->GetNumberOfLods(); cnt < numberOfLods; cnt += 1)
			{
				this->levelsOfDetails[cnt] = HRInterface::GetInstance().CreateIndexedShape();
			}
		}
		else if (numberOfLods < this->GetNumberOfLods())
		{
			for (size_t cnt = numberOfLods; cnt < this->GetNumberOfLods(); cnt += 1)
			{
				this->levelsOfDetails[cnt]->RemoveReference();
			}

			this->levelsOfDetails.Resize(numberOfLods);
		}
	}

	void StaticMesh::DestroyLodAt(const size_t lod)
	{
		GD_ASSERT((lod != 0), "Unable to delete LOD in level 0");
		GD_ASSERT((lod < this->GetNumberOfLods()), "Unable to delete LOD in level 0");

		this->levelsOfDetails[lod]->RemoveReference();
		this->levelsOfDetails.RemoveElementAt(lod);
	}

	HRIIndexedShape* StaticMesh::GetLodAt(const size_t lod)
	{
		GD_ASSERT((lod < this->GetNumberOfLods()), "LOD at this level doesn't exists");
		return this->levelsOfDetails[lod];
	}

	const HRIIndexedShape* StaticMesh::GetLodAt(const size_t lod) const
	{
		GD_ASSERT((lod < this->GetNumberOfLods()), "LOD at this level doesn't exists");
		return this->levelsOfDetails[lod];
	}
		
	void StaticMesh::SetLodAt(const size_t lod, const HRIIndexedShape* submesh)
	{
		/*GD_ASSERT((lod < this->GetNumberOfLods()), "LOD at this level doesn't exists");
		
		HRIIndexedShape* existingSubmesh = const_cast<HRIIndexedShape*>(this->GetLodAt(lod));
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
		this->GetLodAt(lod);
		this->bindedLevelOfDetails = lod;
	}

	/// ==========================================================================================
	// LoadShekel_* functions
	/// ==========================================================================================

	void LoadShekel_01(UniquePtr<InputStream> const& InputResourceData, HRIIndexedShape* const staticMesh)
	{
		/// ==========================================================================================
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
		/// ==========================================================================================

		UInt32 verticesNumber = 0;
		InputResourceData->Read(&verticesNumber, 1, sizeof(verticesNumber));

		UInt32 indicesNumber = 0;
		InputResourceData->Read(&indicesNumber, 1, sizeof(indicesNumber));

		Vector<Float32> verticesBuffer;
		verticesBuffer.Resize(static_cast<size_t>(verticesNumber) * sizeof(Vector3) / sizeof(Float32));

		InputResourceData->Read(&verticesBuffer[0], static_cast<size_t>(verticesNumber), sizeof(Vector3));
		staticMesh->SetVertexData(GD_HRI_SEMANTIC_POSITION, &verticesBuffer[0], verticesBuffer.GetLength());

		InputResourceData->Read(&verticesBuffer[0], static_cast<size_t>(verticesNumber), sizeof(Vector3));
		staticMesh->SetVertexData(GD_HRI_SEMANTIC_NORMAL, &verticesBuffer[0], verticesBuffer.GetLength());

		InputResourceData->Read(&verticesBuffer[0], static_cast<size_t>(verticesNumber), sizeof(Vector3));
		staticMesh->SetVertexData(GD_HRI_SEMANTIC_TANGENT, &verticesBuffer[0], verticesBuffer.GetLength());

		InputResourceData->Read(&verticesBuffer[0], static_cast<size_t>(verticesNumber), sizeof(Vector3));
		staticMesh->SetVertexData(GD_HRI_SEMANTIC_BINORMAL, &verticesBuffer[0], verticesBuffer.GetLength());

		verticesBuffer.Resize(static_cast<size_t>(verticesNumber) * sizeof(Vector2) / sizeof(Float32));
		InputResourceData->Read(&verticesBuffer[0], static_cast<size_t>(verticesNumber), sizeof(Vector2));
		staticMesh->SetVertexData(GD_HRI_SEMANTIC_TEXCOORD, &verticesBuffer[0], verticesBuffer.GetLength());

		Vector<UInt16> indicesBuffer;
		indicesBuffer.Resize(static_cast<size_t>(indicesNumber));
		InputResourceData->Read(&indicesBuffer[0], sizeof(UInt16), static_cast<size_t>(indicesNumber));
		staticMesh->SetIndexData(&indicesBuffer[0], indicesBuffer.GetLength(), sizeof(indicesBuffer[0]));
	}

	void LoadShekel_10(UniquePtr<InputStream> const& InputResourceData, HRIIndexedShape* const staticMesh)
	{
		GD_ASSERT(false, "Unimplemented");
	}

	/// ==========================================================================================
	// Messaging functions
	/// ==========================================================================================

	void StaticMesh::OnResourceLoad(UniquePtr<InputStream> const& InputResourceData)
	{
		/// ==========================================================================================
		// Attempting to check if this file is Shekel(R)
		struct ShekeLHeader
		{
			char format[10];
			ShekelVersion version;
		} shekeLHeader;
		
		InputResourceData->Read(&shekeLHeader, 1, sizeof(shekeLHeader));
		GD_ASSERT((strcmp(&shekeLHeader.format[0], "SHEKEL??!") == 0), 
			"Shekel(R) format validation failed.");

		switch (shekeLHeader.version)
		{
		case ShekelVersion::Version01: { LoadShekel_01(InputResourceData, this->GetLodAt(0)); } break;
		case ShekelVersion::Version10: { LoadShekel_10(InputResourceData, this->GetLodAt(0)); } break;
		default: GD_ASSERT(false, "Unsupported Shekel(R) version");
		}
	}
	
	void StaticMesh::OnResourceUnload()
	{
	}

GD_NAMESPACE_END
