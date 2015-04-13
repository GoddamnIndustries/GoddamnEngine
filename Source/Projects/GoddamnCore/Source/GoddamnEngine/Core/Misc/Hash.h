//! ==========================================================================================
//! Hash.h - Text hashing algorithms.
//! Copyright (C) Goddamn Industries 2015. All Rights Reserved.
//! ==========================================================================================

#pragma once
#ifndef GD_CORE_TEXT_HASHCODE
#define GD_CORE_TEXT_HASHCODE

#include <GoddamnEngine/Include.h>

GD_NAMESPACE_BEGIN

	typedef UInt32 HashValueType;

	//! Represents hash summ that can not be implcilty casted to integer type. 
	struct HashCode final
	{
	private:
		HashValueType Value = 0;

	public:
		//! Initializes hash summ with precomputed integer value.
		GDINL explicit HashCode(HashValueType const HashValue = 0) : Value(HashValue) { }

		//! Returns integer representation of this hash summ.
		GDINL HashValueType GetValue() const { return this->Value; }

		//! Compares to hash summes.
		GDINL bool operator== (HashCode const& HashCode) const { return (this->Value == HashCode.Value); }
		GDINL bool operator!= (HashCode const& HashCode) const { return (this->Value != HashCode.Value); }
		GDINL bool operator>  (HashCode const& HashCode) const { return (this->Value >  HashCode.Value); }
		GDINL bool operator<  (HashCode const& HashCode) const { return (this->Value <  HashCode.Value); }
	};	// struct HashCode

GD_NAMESPACE_END

#endif	// ifndef GD_CORE_TEXT_HASHCODE
