// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/Object/Serialization/ReaderWriter.h
 * File contains base interface to the DOM-based serialization readers and writers.
 */
#pragma once

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Object/Serialization/ReaderWriter.h>
#include <GoddamnEngine/Core/Object/Serialization/DOM.h>

GD_NAMESPACE_BEGIN

	// **------------------------------------------------------------------------------------------**
	//! Interface for the serialization reader - FOR INTERNAL USAGE ONLY!
	// **------------------------------------------------------------------------------------------**
	class DOMReader : public IGenericReader
	{
	private:
		struct DOMValueArrayScope
		{
			DomValueVector Array;
			UInt32 mutable CurrentIndex;
		};	// struct DOMValueArrayScope
		typedef DomObjectPtr DOMObjectPtrScope;

		DomValuePtr m_SelectedValue;
		Stack<DOMObjectPtrScope> m_ObjectsScope;
		Stack<DOMValueArrayScope> m_ArraysScope;

	protected:
		GDINL explicit DOMReader(DomPtr const dom, SharedPtr<InputStream> const readingStream)
			: IGenericReader(readingStream)
		{
			auto result = dom->_TryParseDOM(readingStream);
			m_ObjectsScope.InsertLast(result.RootObject);
		}

	public:
		GDINL gd_s_virtual bool TryReadPropertyName(String const& name) gd_s_override
		{
			if (!IGenericReader::TryReadPropertyName(name))
			{
				// Selecting top object's property by name.
				GD_DEBUG_VERIFY(!m_ObjectsScope.IsEmpty(), "Object scoping error.");
				
				auto const topDomObject = m_ObjectsScope.GetLast();
				m_SelectedValue = topDomObject->_GetProperty(name);
				return m_SelectedValue != nullptr;
			}

			// Selecting next element of the array.
			GD_DEBUG_VERIFY(!m_ArraysScope.IsEmpty(), "Array scoping error.");
			
			auto const& topDomArray = m_ArraysScope.GetLast();
			GD_DEBUG_VERIFY(topDomArray.CurrentIndex < topDomArray.Array.GetLength(), "Reading array element runs out of bounds.");
			m_SelectedValue = topDomArray.Array[topDomArray.CurrentIndex++];
			return true;
		}

		// ------------------------------------------------------------------------------------------
		// Primitive properties readers.
		// ------------------------------------------------------------------------------------------

		template<typename TValue>
		GDINL gd_s_virtual bool TryReadPropertyValue(TValue& value) gd_s_override
		{
			GD_DEBUG_VERIFY(m_SelectedValue != nullptr, "No property was selected with 'TryReadPropertyName' call.");
			return m_SelectedValue->_TryGetValue(value);
		}

		// ------------------------------------------------------------------------------------------
		// Array properties readers.
		// ------------------------------------------------------------------------------------------

		GDINL gd_s_virtual bool TryBeginReadArrayPropertyValue(SizeTp& arraySize) gd_s_override
		{
			if (!IGenericReader::TryBeginReadArrayPropertyValue(arraySize))
			{
				DomValueVector value;
				if (!TryReadPropertyValue(value))
				{
					IGenericReader::EndReadArrayPropertyValue();
					return false;
				}
				arraySize = value.GetLength();
				m_ArraysScope.InsertLast({ value, 0 });
			}
			return true;
		}
			
		GDINL gd_s_virtual void EndReadArrayPropertyValue() gd_s_override
		{
			IGenericReader::EndReadArrayPropertyValue();
			m_ArraysScope.EraseLast();
		}

		// ------------------------------------------------------------------------------------------
		// Structure properties readers.
		// ------------------------------------------------------------------------------------------

		GDINL gd_s_virtual bool TryBeginReadStructPropertyValue() gd_s_override
		{
			if (!IGenericReader::TryBeginReadStructPropertyValue())
			{
				// If we are trying to parse root object.
				if (!m_ObjectsScope.IsEmpty())
				{
					DomObjectPtr value;
					if (!TryReadPropertyValue(value))
					{
						IGenericReader::EndReadArrayPropertyValue();
						return false;
					}
					m_ObjectsScope.InsertLast(value);
				}
			}
			return true;
		}

		GDINL gd_s_virtual void EndReadStructPropertyValue() gd_s_override
		{
			IGenericReader::EndReadArrayPropertyValue();
			m_ObjectsScope.EraseLast();
		}

	};	// class DOMReader

	// Note that we are not providing the DOMSerializationWriter class.

GD_NAMESPACE_END
