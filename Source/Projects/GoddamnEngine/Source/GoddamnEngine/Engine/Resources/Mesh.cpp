// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Engine/Resources/Mesh.cpp
 * File contains mesh class for resources subsystem.
 */
#include <GoddamnEngine/Engine/Resources/Mesh.h>
#include <GoddamnEngine/Engine/Interface/Graphics/Graphics.h>

GD_NAMESPACE_BEGIN

	GD_IMPLEMENT_OBJECT_INTRINSIC(Mesh);

	// ------------------------------------------------------------------------------------------
	// Channels implementations.
	// ------------------------------------------------------------------------------------------

	GD_RESOURCE_KERNEL class ISubmeshChannel : public IVirtuallyDestructible
	{
	public:
	};	// class IChannel

	template<typename TVertexAttributeType>
	GD_RESOURCE_KERNEL class TSubmeshChannel : public ISubmeshChannel
	{
	private:
		Vector<TVertexAttributeType> m_Data;
		SharedPtr<IGraphicsBuffer> m_DataBuffer;

	public:

	};	// class IChannel

	// ------------------------------------------------------------------------------------------
	// Manual vertex attributes modifications.
	// ------------------------------------------------------------------------------------------

	/*{
	return const_cast<Mesh*>(this)->GetChannel(channelType, submeshIndex, createIfNotPresent, false);
	}*/

	/*!
	 * @brief Accesses some of the vertex channels.
	 *
	 * Returns a pointer to an editable vector of vertex attributes of some generic channel.
	 * The next time this mesh would be rendered by system, all changes in vertex coordinates would be
	 * automatically uploaded to the GPU.
	 * 
	 * @param channelType Desired channel.
	 * @param submeshIndex Index of a sub-mesh.
	 * @param createIfNotPresent If true and no vertex coordinates channel exists, creates a new one.
	 *
	 * @returns Pointer to an editable vector of vertex coordinates or nullptr if the channel does not exist.
	 */
	GDAPI Handle Mesh::GetChannel(ChannelType const channelType, SizeTp const submeshIndex, bool const createIfNotPresent, bool const wriceAccess)
	{
		GD_DEBUG_VERIFY(channelType < ChannelType::Count, "Invalid 'channelType' parameter value.");
		GD_DEBUG_VERIFY(submeshIndex == 0, "Parameter 'submeshIndex' should be set to zero for now.");

		return nullptr;
	}

GD_NAMESPACE_END
