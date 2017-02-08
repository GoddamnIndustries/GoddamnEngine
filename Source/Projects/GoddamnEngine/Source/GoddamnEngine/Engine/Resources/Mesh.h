// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Engine/Resources/Mesh.h
 * File contains mesh class for resources subsystem.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Engine/Resources/Resource.h>
#include <GoddamnEngine/Engine/Interface/Graphics/Graphics.h>

#include <GoddamnEngine/Core/Math/Vector.h>
#include <GoddamnEngine/Core/Math/Matrix.h>
#include <GoddamnEngine/Core/Templates/UniquePtr.h>
#include <GoddamnEngine/Core/Containers/Vector.h>

GD_NAMESPACE_BEGIN

	//! @todo
	typedef int AABB;

	/*!
	 * 
	 */
	GD_RESOURCE_KERNEL enum class MeshTopology
	{
		Triangles,
		Quads,
		Lines,
		LineStrip,
		Points,
	};	// enum class MeshTopology

	/*!
	 * 
	 */
	GD_RESOURCE_KERNEL struct MeshBoneWeight
	{
	public:
		UInt32 BoneIndex0;
		UInt32 BoneIndex1;
		UInt32 BoneIndex2;
		UInt32 BoneIndex3;
		Float32 Weight0;
		Float32 Weight1;
		Float32 Weight2;
		Float32 Weight3;
	};	// struct MeshBindWight

	// **------------------------------------------------------------------------------------------**
	//! Declares a generic mesh resource.
	// **------------------------------------------------------------------------------------------**
	GD_RESOURCE_KERNEL class Mesh final : public Resource
	{
		GD_DECLARE_OBJECT_INTRINSIC_ENGINE(Mesh, Resource);

		enum class ChannelType
		{
			VertexCoords,
			Normals,
			Tangents,
			TexCoords,
			TexCoords1,
			TexCoords2,
			TexCoords3,
			TexCoords4,
			Colors,
			Indices,
			Count
		};	// enum class ChannelType

	private:
		SharedPtr<IGraphicsVertexArray> m_VertexArray;
	//	Vector<UniquePtr<class ISubmeshChannel>> m_VertexChannels;

	public:

		/*!
		 * Returns true if this mesh is really editable.
		 */
		GDAPI bool IsEditable() const;

		/*!
		 * Makes mesh non-editable and clears lots of internal buffers.
		 */
		GDAPI void MakeNotEditable() const;

		// ------------------------------------------------------------------------------------------
		// Generic mesh meta operations.
		// ------------------------------------------------------------------------------------------

		GDAPI SizeTp AddSubmesh();

		GDAPI SizeTp GetSubmeshesCount() const;

		GDAPI MeshTopology GetTopology(SizeTp const submeshIndex = 0) const;

		GDAPI void SetTopology(MeshTopology const topology, SizeTp const submeshIndex = 0);

		GDAPI void UploadToGpu() const;

		// ------------------------------------------------------------------------------------------
		// Vertex attributes access and modifications.
		// ------------------------------------------------------------------------------------------

	private:
		/* Generalized operations. */
		GDAPI Handle GetChannel(ChannelType const channelType, SizeTp const submeshIndex, bool const createIfNotPresent, bool const wriceAccess = true);
		GDAPI CHandle GetChannel(ChannelType const channelType, SizeTp const submeshIndex, bool const createIfNotPresent) const;
		
		GDAPI void ClearChannel(ChannelType const channelType, SizeTp const submeshIndex, bool const removeChannel);

	public:

		/*!
		 * @brief Accesses the bind poses of the mesh.
		 */
		//! @{
		GDAPI Vector<Matrix4x4 const> const* GetBindPoses() const;
		GDAPI Vector<Matrix4x4>* GetBindPoses();
		//! @}

		/*!
		 * @brief Accesses the vertex bone weights.
		 * 
		 * If this mesh is editable, returns a pointer to an editable vector of vertex bone weights.
		 * The next time this mesh would be rendered by system, all changes in vertex coordinates would be
		 * automatically uploaded to the GPU.
		 * 
		 * @param submeshIndex Index of a sub-mesh.
		 * @param createIfNotPresent If true and no vertex coordinates channel exists, creates a new one.
		 * 
		 * @returns Pointer to an editable vector of vertex bone weights or nullptr if the channel does not exist.
		 */
		//! @{
		GDAPI Vector<MeshBoneWeight const> const* GetBoneWeights(bool const createIfNotPresent = false) const;
		GDAPI Vector<MeshBoneWeight>* GetBoneWeights(bool const createIfNotPresent = false);
		//! @}

		/*!
		 * @brief Accesses the vertex coordinates.
		 * 
		 * If this mesh is editable, returns a pointer to an editable vector of vertex coordinates.
		 * The next time this mesh would be rendered by system, all changes in vertex coordinates would be
		 * automatically uploaded to the GPU.
		 * 
		 * @param submeshIndex Index of a sub-mesh.
		 * @param createIfNotPresent If true and no vertex coordinates channel exists, creates a new one.
		 * 
		 * @returns Pointer to an editable vector of vertex coordinates or nullptr if the channel does not exist.
		 */
		//! @{
		GDINL Vector<Vector3> const* GetVertexCoords(SizeTp const submeshIndex = 0, bool const createIfNotPresent = false) const
		{
			return static_cast<Vector<Vector3> const*>(GetChannel(ChannelType::VertexCoords, submeshIndex, createIfNotPresent));
		}
		GDINL Vector<Vector3>* GetVertexCoords(SizeTp const submeshIndex = 0, bool const createIfNotPresent = false)
		{
			return static_cast<Vector<Vector3>*>(GetChannel(ChannelType::VertexCoords, submeshIndex, createIfNotPresent));
		}
		//! @}

		/*!
		 * Clears vertex coordinates in the mesh.
		 * 
		 * @param submeshIndex Index of a sub-mesh.
		 * @param removeChannel If true, removes the channel from vertex layout.
		 */
		GDINL void ClearVertexCoords(SizeTp const submeshIndex = 0, bool const removeChannel = false)
		{
			ClearChannel(ChannelType::VertexCoords, submeshIndex, removeChannel);
		}

		/*!
		 * @brief Accesses the vertex normals.
		 * 
		 * If this mesh is editable, returns a pointer to an editable vector of vertex normals.
		 * The next time this mesh would be rendered by system, all changes in vertex normals would be
		 * automatically uploaded to the GPU.
		 * 
		 * @param submeshIndex Index of a sub-mesh.
		 * @param createIfNotPresent If true and no vertex normals channel exists, creates a new one.
		 * 
		 * @returns Pointer to an editable vector of vertex normals or nullptr if the channel does not exist.
		 */
		//! @{
		GDINL Vector<Vector3> const* GetNormals(SizeTp const submeshIndex = 0, bool const createIfNotPresent = false) const
		{
			return static_cast<Vector<Vector3> const*>(GetChannel(ChannelType::Normals, submeshIndex, createIfNotPresent));
		}
		GDINL Vector<Vector3>* GetNormals(SizeTp const submeshIndex = 0, bool const createIfNotPresent = false)
		{
			return static_cast<Vector<Vector3>*>(GetChannel(ChannelType::Normals, submeshIndex, createIfNotPresent));
		}
		//! @}

		/*!
		 * Clears vertex normals in the mesh.
		 * 
		 * @param submeshIndex Index of a sub-mesh.
		 * @param removeChannel If true, removes the channel from vertex layout.
		 */
		GDINL void ClearNormals(SizeTp const submeshIndex = 0, bool const removeChannel = false)
		{
			ClearChannel(ChannelType::Normals, submeshIndex, removeChannel);
		}

		/*!
		 * @brief Accesses the vertex tangents.
		 * 
		 * If this mesh is editable, returns a pointer to an editable vector of vertex tangents.
		 * The next time this mesh would be rendered by system, all changes in vertex tangents would be
		 * automatically uploaded to the GPU.
		 * 
		 * @param submeshIndex Index of a sub-mesh.
		 * @param createIfNotPresent If true and no vertex tangents channel exists, creates a new one.
		 * 
		 * @returns Pointer to an editable vector of vertex tangents or nullptr if the channel does not exist.
		 */
		//! @{
		GDINL Vector<Vector3> const* GetTangents(SizeTp const submeshIndex = 0, bool const createIfNotPresent = false) const
		{
			return static_cast<Vector<Vector3> const*>(GetChannel(ChannelType::Tangents, submeshIndex, createIfNotPresent));
		}
		GDINL Vector<Vector3>* GetTangents(SizeTp const submeshIndex = 0, bool const createIfNotPresent = false)
		{
			return static_cast<Vector<Vector3>*>(GetChannel(ChannelType::Tangents, submeshIndex, createIfNotPresent));
		}
		//! @}

		/*!
		 * Clears vertex tangents in the mesh.
		 * 
		 * @param submeshIndex Index of a sub-mesh.
		 * @param removeChannel If true, removes the channel from vertex layout.
		 */
		GDINL void ClearTangents(SizeTp const submeshIndex = 0, bool const removeChannel = false)
		{
			ClearChannel(ChannelType::Tangents, submeshIndex, removeChannel);
		}

		/*!
		 * @brief Accesses the vertex texture coordinates.
		 * 
		 * If this mesh is editable, returns a pointer to an editable vector of vertex texture coordinates.
		 * The next time this mesh would be rendered by system, all changes in vertex texture coordinates would be
		 * automatically uploaded to the GPU.
		 * 
		 * @param submeshIndex Index of a sub-mesh.
		 * @param createIfNotPresent If true and no vertex texture coordinates channel exists, creates a new one.
		 * 
		 * @tparam TTextureCoordsIndex Index of the texture coordinates channel.
		 * @returns Pointer to an editable vector of vertex texture coordinates or nullptr if the channel does not exist.
		 */
		//! @{
		template<SizeTp TTextureCoordsIndex = 0>
		GDINL Vector<Vector2> const* GetTextureCoords(SizeTp const submeshIndex = 0, bool const createIfNotPresent = false) const
		{
			static_assert(TTextureCoordsIndex <= 4, "'TTextureCoordsIndex' parameter should be between 0 and 4.");
			auto static const channel = static_cast<ChannelType>(static_cast<SizeTp>(ChannelType::TexCoords) + TTextureCoordsIndex);
			return static_cast<Vector<Vector2> const*>(GetChannel(channel, submeshIndex, createIfNotPresent));
		}
		template<SizeTp TTextureCoordsIndex = 0>
		GDINL Vector<Vector2>* GetTextureCoords(SizeTp const submeshIndex = 0, bool const createIfNotPresent = false)
		{
			static_assert(TTextureCoordsIndex <= 4, "'TTextureCoordsIndex' parameter should be between 0 and 4.");
			auto static const channel = static_cast<ChannelType>(static_cast<SizeTp>(ChannelType::TexCoords) + TTextureCoordsIndex);
			return static_cast<Vector<Vector2>*>(GetChannel(channel, submeshIndex, createIfNotPresent));
		}
		//! @}

		/*!
		 * Clears vertex texture coordinates in the mesh.
		 * 
		 * @param submeshIndex Index of a sub-mesh.
		 * @param removeChannel If true, removes the channel from vertex layout.
		 * 
		 * @tparam TTextureCoordsIndex Index of the texture coordinates channel.
		 */
		template<SizeTp TTextureCoordsIndex = 0>
		GDINL void ClearTextureCoords(SizeTp const submeshIndex = 0, bool const removeChannel = false)
		{
			static_assert(TTextureCoordsIndex <= 4, "'TTextureCoordsIndex' parameter should be between 0 and 4.");
			auto static const channel = static_cast<ChannelType>(static_cast<SizeTp>(ChannelType::TexCoords) + TTextureCoordsIndex);
			ClearChannel(channel, submeshIndex, removeChannel);
		}

		/*!
		 * @brief Accesses the vertex colors.
		 * 
		 * If this mesh is editable, returns a pointer to an editable vector of vertex colors.
		 * The next time this mesh would be rendered by system, all changes in vertex colors would be
		 * automatically uploaded to the GPU.
		 * 
		 * @param submeshIndex Index of a sub-mesh.
		 * @param createIfNotPresent If true and no vertex colors channel exists, creates a new one.
		 * 
		 * @returns Pointer to an editable vector of vertex colors or nullptr if the channel does not exist.
		 */
		//! @{
		GDINL Vector<Color> const* GetColors(SizeTp const submeshIndex = 0, bool const createIfNotPresent = false) const
		{
			return static_cast<Vector<Color> const*>(GetChannel(ChannelType::Colors, submeshIndex, createIfNotPresent));
		}
		GDINL Vector<Color>* GetColors(SizeTp const submeshIndex = 0, bool const createIfNotPresent = false)
		{
			return static_cast<Vector<Color>*>(GetChannel(ChannelType::Colors, submeshIndex, createIfNotPresent));
		}
		//! @}

		/*!
		 * Clears vertex texture colors in the mesh.
		 * 
		 * @param submeshIndex Index of a sub-mesh.
		 * @param removeChannel If true, removes the channel from vertex layout.
		 */
		GDINL void ClearColors(SizeTp const submeshIndex = 0, bool const removeChannel = false)
		{
			ClearChannel(ChannelType::Colors, submeshIndex, removeChannel);
		}

		/*!
		 * @brief Accesses the vertex indices.
		 * 
		 * If this mesh is editable, returns a pointer to an editable vector of vertex indices.
		 * The next time this mesh would be rendered by system, all changes in vertex colors would be
		 * automatically uploaded to the GPU.
		 * 
		 * @param submeshIndex Index of a sub-mesh.
		 * @param createIfNotPresent If true and no vertex indices channel exists, creates a new one.
		 * 
		 * @returns Pointer to an editable vector of vertex indices or nullptr if the channel does not exist.
		 */
		//! @{
		GDINL Vector<UInt32> const* GetIndices(SizeTp const submeshIndex = 0, bool const createIfNotPresent = false) const 
		{
			return static_cast<Vector<UInt32> const*>(GetChannel(ChannelType::Indices, submeshIndex, createIfNotPresent));
		}
		GDINL Vector<UInt32>* GetIndices(SizeTp const submeshIndex = 0, bool const createIfNotPresent = false)
		{
			return static_cast<Vector<UInt32>*>(GetChannel(ChannelType::Indices, submeshIndex, createIfNotPresent));
		}
		//! @}

		/*!
		 * Clears vertex indices in the mesh.
		 * 
		 * @param submeshIndex Index of a sub-mesh.
		 * @param removeChannel If true, removes the channel from vertex layout.
		 */
		GDINL void ClearIndices(SizeTp const submeshIndex = 0, bool const removeChannel = false)
		{
			ClearChannel(ChannelType::Indices, submeshIndex, removeChannel);
		}

		// ------------------------------------------------------------------------------------------
		// Automated vertex attributes modifications.
		// ------------------------------------------------------------------------------------------

		/*!
		 * Recalculates smooth normals of the mesh.
		 */
		GDAPI void RecalculateSmoothNormals();

		/*!
		 */
		GDAPI void Optimize();

	};	// class Mesh

GD_NAMESPACE_END
