// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
//
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

#pragma once

#include <GoddamnEngine/Core/Containers/Map.h>
#include <GoddamnEngine/Core/Containers/String.h>
#include <GoddamnEngine/Core/Containers/StringBuilder.h>
#include <GoddamnEngine/Core/Containers/Vector.h>
#include <GoddamnEngine/Core/Templates/SharedPtr.h>
#include <GoddamnEngine/Include.h>
GD_USING_NAMESPACE;

#pragma warning(disable : 4244)
#define GD_STUBBED_UTF16

#include "PlatformDependent/Base.h"
#undef CreateWindow

namespace Engine
{
	typedef WideString ImmutableString;
	template <typename T>
	using Array = Vector<T>;
	template <class V>
	using SafePointer = SharedPtr<V>;
	template <class V>
	using ObjectArray = Vector<SharedPtr<V>>;

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//                    AddRef/Release Policy
	// Function must call AddRef() on object pointer if:
	//  - this object is an input argument and function wants
	//    to store it's reference,
	//  - object reference is the result value of this function,
	//    function assumes object's creation, but object was
	//    taken from another place (for example, from cache)
	//  Function mush call Release() on object pointer if is is
	//  not required anymore and object's reference was taken as
	//  a result value of "Create" function or was retained
	//  explicitly.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	class Object : public ReferenceTarget
	{
#if 0
	private:
		UInt32 _refcount;

	public:
		Object();
		Object(const Object& src) = gd_delete;
		Object& operator=(const Object& src) = gd_delete;
		virtual UInt32 AddRef();
		virtual UInt32 Release();
		virtual ~Object();
#endif
	public:
		UInt32 GetReferenceCount() const
		{
			return 1;
		}
		virtual ImmutableString ToString() const
		{
			return L"AA";
		}
	};

	class Exception : public Object
	{
	public:
		Exception();
		Exception(const Exception& e);
		Exception(Exception&& e);
		Exception& operator=(const Exception& e);
		ImmutableString ToString() const override;
	};
	class OutOfMemoryException : public Exception
	{
	public:
		ImmutableString ToString() const override;
	};
	class InvalidArgumentException : public Exception
	{
	public:
		ImmutableString ToString() const override;
	};
	class InvalidFormatException : public Exception
	{
	public:
		ImmutableString ToString() const override;
	};

#if 0
	class ImmutableString final : public Object
	{
	private:
		widechar * text;
		Int32 GeneralizedFindFirst(Int32 from, const widechar * seq, Int32 length) const;
		ImmutableString GeneralizedReplace(const widechar * seq, Int32 length, const widechar * with, Int32 withlen) const;
		UInt64 GeneralizedToUInt64(Int32 dfrom, Int32 dto) const;
		UInt64 GeneralizedToUInt64(Int32 dfrom, Int32 dto, const ImmutableString & digits, bool case_sensitive) const;
	public:
		ImmutableString();
		ImmutableString(const ImmutableString & src);
		ImmutableString(ImmutableString && src);
		ImmutableString(const widechar * src);
		ImmutableString(const widechar * sequence, Int32 length);
		ImmutableString(Int32 src);
		ImmutableString(UInt32 src);
		ImmutableString(Int64 src);
		ImmutableString(UInt64 src);
		ImmutableString(UInt32 value, const ImmutableString & digits, Int32 minimal_length = 0);
		ImmutableString(UInt64 value, const ImmutableString & digits, Int32 minimal_length = 0);
		ImmutableString(const void * Sequence, Int32 Length, Encoding SequenceEncoding);
		ImmutableString(Float32 src, widechar separator = L'.');
		ImmutableString(Float64 src, widechar separator = L'.');
		ImmutableString(bool src);
		ImmutableString(widechar src);
		ImmutableString(widechar src, Int32 repeats);
		explicit ImmutableString(const Object * object);
		explicit ImmutableString(const void * src);
		~ImmutableString() override;

		ImmutableString & operator = (const ImmutableString & src);
		ImmutableString & operator = (const widechar * src);
		operator const widechar * () const;
		Int32 Length() const;

		bool friend operator == (const ImmutableString & a, const ImmutableString & b);
		bool friend operator == (const widechar * a, const ImmutableString & b);
		bool friend operator == (const ImmutableString & a, const widechar * b);
		bool friend operator != (const ImmutableString & a, const ImmutableString & b);
		bool friend operator != (const widechar * a, const ImmutableString & b);
		bool friend operator != (const ImmutableString & a, const widechar * b);

		bool friend operator <= (const ImmutableString & a, const ImmutableString & b);
		bool friend operator >= (const ImmutableString & a, const ImmutableString & b);
		bool friend operator < (const ImmutableString & a, const ImmutableString & b);
		bool friend operator > (const ImmutableString & a, const ImmutableString & b);

		static Int32 Compare(const ImmutableString & a, const ImmutableString & b);
		static Int32 CompareIgnoreCase(const ImmutableString & a, const ImmutableString & b);

		widechar operator [] (Int32 index) const;
		widechar CharAt(Int32 index) const;

		ImmutableString ToString() const override;

		virtual void Concatenate(const ImmutableString & str);
		
		ImmutableString friend operator + (const ImmutableString & a, const ImmutableString & b);
		ImmutableString friend operator + (const widechar * a, const ImmutableString & b);
		ImmutableString friend operator + (const ImmutableString & a, const widechar * b);
		ImmutableString & operator += (const ImmutableString & str);

		Int32 FindFirst(widechar letter) const;
		Int32 FindFirst(const ImmutableString & str) const;
		Int32 FindLast(widechar letter) const;
		Int32 FindLast(const ImmutableString & str) const;

		ImmutableString Fragment(Int32 PosStart, Int32 CharLength) const;
		ImmutableString Replace(const ImmutableString & Substring, const ImmutableString & with) const;
		ImmutableString Replace(widechar Substring, const ImmutableString & with) const;
		ImmutableString Replace(const ImmutableString & Substring, widechar with) const;
		ImmutableString Replace(widechar Substring, widechar with) const;

		ImmutableString LowerCase() const;
		ImmutableString UpperCase() const;

		Int32 GetEncodedLength(Encoding encoding) const;
		void Encode(void * buffer, Encoding encoding, bool include_terminator) const;
		Array<UInt8> * EncodeSequence(Encoding encoding, bool include_terminator) const;
		Array<ImmutableString> Split(widechar divider) const;

		UInt64 ToUInt64() const;
		UInt64 ToUInt64(const ImmutableString & digits, bool case_sensitive = false) const;
		Int64 ToInt64() const;
		Int64 ToInt64(const ImmutableString & digits, bool case_sensitive = false) const;
		UInt32 ToUInt32() const;
		UInt32 ToUInt32(const ImmutableString & digits, bool case_sensitive = false) const;
		Int32 ToInt32() const;
		Int32 ToInt32(const ImmutableString & digits, bool case_sensitive = false) const;
		Float32 ToFloat() const;
		Float32 ToFloat(const ImmutableString & separators) const;
		Float64 ToDouble() const;
		Float64 ToDouble(const ImmutableString & separators) const;
		bool ToBoolean() const;
	};
#endif

	typedef ImmutableString string;

	template <class V>
	void swap(V& a, V& b)
	{
		if (&a == &b)
			return;
		UInt8 buffer[sizeof(V)];
		MemoryCopy(buffer, &a, sizeof(V));
		MemoryCopy(&a, &b, sizeof(V));
		MemoryCopy(&b, buffer, sizeof(V));
	}
	template <class V>
	void safe_swap(V& a, V& b)
	{
		V e = a;
		a = b;
		b = e;
	}

	// template <class V> V & min(V & a, V & b) { return (a < b) ? a : b; }
	// template <class V> V min(V a, V b) { return (a < b) ? a : b; }
	// template <class V> V & max(V & a, V & b) { return (a < b) ? b : a; }
	// template <class V> V max(V a, V b) { return (a < b) ? b : a; }

	Float64 sgn(Float64 x);
	Float32 sgn(Float32 x);

#if 0
	template <class V> class Array : public Object
	{
		V * data;
		Int32 count;
		Int32 allocated;
		Int32 block;
		Int32 block_align(Int32 element_count) { return ((Int64(element_count) + block - 1) / block) * block; }
		void require(Int32 elements, bool nothrow = false)
		{
			Int32 new_size = block_align(elements);
			if (new_size != allocated) {
				if (new_size > allocated) {
					V * new_data = reinterpret_cast<V*>(realloc(data, sizeof(V) * new_size));
					if (new_data || new_size == 0) {
						data = new_data;
						allocated = new_size;
					} else if (!nothrow) throw OutOfMemoryException();
				} else {
					V * new_data = reinterpret_cast<V*>(realloc(data, sizeof(V) * new_size));
					if (new_data || new_size == 0) {
						data = new_data;
						allocated = new_size;
					}
				}
			}
		}
		void append(const V & v) { new (data + count) V(v); count++; }
	public:
		Array() : count(0), allocated(0), data(0), block(0x400) {}
		Array(const Array & src) : count(src.count), allocated(0), data(0), block(src.block)
		{
			require(count); Int32 i = 0;
			try { for (i = 0; i < count; i++) new (data + i) V(src.data[i]); }
			catch (...) {
				for (Int32 j = i - 1; j >= 0; j--) data[i].V::~V();
				free(data); throw;
			}
		}
		Array(Array && src) : count(src.count), allocated(src.allocated), data(src.data), block(src.block) { src.data = 0; src.allocated = 0; src.count = 0; }
		explicit Array(Int32 BlockSize) : count(0), allocated(0), data(0), block(BlockSize) {}
		~Array() override { for (Int32 i = 0; i < count; i++) data[i].V::~V(); free(data); }

		Array & operator = (const Array & src)
		{
			if (this == &src) return *this;
			Array Copy(src.block);
			Copy.require(src.count);
			for (Int32 i = 0; i < src.count; i++) Copy.append(src.data[i]);
			for (Int32 i = 0; i < count; i++) data[i].V::~V();
			free(data);
			data = Copy.data; count = Copy.count; allocated = Copy.allocated; block = Copy.block;
			Copy.data = 0; Copy.count = 0; Copy.allocated = 0;
			return *this;
		}

		virtual void Append(const V & v) { require(count + 1); append(v); }
		virtual void Append(const Array & src) { if (&src == this) throw InvalidArgumentException(); require(count + src.count); for (Int32 i = 0; i < src.count; i++) append(src.data[i]); }
		virtual void Append(const V * v, Int32 Count) { if (data == v) throw InvalidArgumentException(); require(count + Count); for (Int32 i = 0; i < Count; i++) append(v[i]); }
		virtual void SwapAt(Int32 i, Int32 j) { swap(data[i], data[j]); }
		virtual void Insert(const V & v, Int32 IndexAt)
		{
			require(count + 1);
			for (Int32 i = count - 1; i >= IndexAt; i--) swap(data[i], data[i + 1]);
			try { new (data + IndexAt) V(v); count++; }
			catch (...) { for (Int32 i = IndexAt; i < count; i++) swap(data[i], data[i + 1]); throw; }
		}
		virtual V & ElementAt(Int32 index) { return data[index]; }
		virtual const V & ElementAt(Int32 index) const { return data[index]; }
		virtual V & FirstElement() { return data[0]; }
		virtual const V & FirstElement() const { return data[0]; }
		virtual V & LastElement() { return data[count - 1]; }
		virtual const V & LastElement() const { return data[count - 1]; }
		virtual void Remove(Int32 index) { data[index].V::~V(); for (Int32 i = index; i < count - 1; i++) swap(data[i], data[i + 1]); count--; require(count, true); }
		virtual void RemoveFirst() { Remove(0); }
		virtual void RemoveLast() { Remove(count - 1); }
		virtual void Clear() { for (Int32 i = 0; i < count; i++) data[i].V::~V(); free(data); data = 0; count = 0; allocated = 0; }
		virtual void SetLength(Int32 length)
		{
			if (length > count) {
				require(length); Int32 i;
				try { for (i = count; i < length; i++) new (data + i) V(); }
				catch (...) { for (Int32 j = i - 1; j >= count; j--) data[i].V::~V(); throw; }
				count = length;
			} else if (length < count) {
				if (length < 0) throw InvalidArgumentException();
				for (Int32 i = length; i < count; i++) data[i].V::~V(); count = length;
				require(count, true);
			}
		}
		Int32 Length() const { return count; }
		Int32 Length() const { return count; }
		V * GetBuffer() { return data; }
		const V * GetBuffer() const { return data; }
		
		string ToString() const override { return L"Array[" + string::FromInt64(count) + L"]"; }

		operator V * () { return data; }
		operator const V * () { return data; }
		Array & operator << (const V & v) { Append(v); return *this; }
		Array & operator << (const Array & src) { Append(src); return *this; }
		V & operator [] (Int32 index) { return data[index]; }
		const V & operator [] (Int32 index) const { return data[index]; }
		bool friend operator == (const Array & a, const Array & b)
		{
			if (a.count != b.count) return false;
			for (Int32 i = 0; i < a.count; i++) if (a[i] != b[i]) return false;
			return true;
		}
		bool friend operator != (const Array & a, const Array & b)
		{
			if (a.count != b.count) return true;
			for (Int32 i = 0; i < a.count; i++) if (a[i] != b[i]) return true;
			return false;
		}
	};
#endif
	template <typename T, typename U>
	Array<T>& operator<<(Array<T>& a, U const& u)
	{
		a.InsertLast(u);
		return a;
	}

#if 0
	template <class V>
	class SafeArray : public Object
	{
		V** data;
		Int32 count;
		Int32 allocated;
		Int32 block;
		Int32 block_align(Int32 element_count)
		{
			return ((Int64(element_count) + block - 1) / block) * block;
		}
		void require(Int32 elements, bool nothrow = false)
		{
			Int32 new_size = block_align(elements);
			if (new_size != allocated)
			{
				if (new_size > allocated)
				{
					V** new_data = reinterpret_cast<V**>(realloc(data, sizeof(V*) * new_size));
					if (new_data || new_size == 0)
					{
						data = new_data;
						allocated = new_size;
					}
					else if (!nothrow)
						throw OutOfMemoryException();
				}
				else
				{
					V** new_data = reinterpret_cast<V**>(realloc(data, sizeof(V*) * new_size));
					if (new_data || new_size == 0)
					{
						data = new_data;
						allocated = new_size;
					}
				}
			}
		}
		void append(const V& v)
		{
			data[count] = gd_new V(v);
			count++;
		}

	public:
		SafeArray()
		    : count(0)
		    , allocated(0)
		    , data(0)
		    , block(0x400)
		{
		}
		SafeArray(const SafeArray& src)
		    : count(src.count)
		    , allocated(0)
		    , data(0)
		    , block(src.block)
		{
			require(count);
			Int32 i = 0;
			try
			{
				for (i = 0; i < count; i++)
					data[i] = gd_new V(src.data[i]);
			}
			catch (...)
			{
				for (Int32 j = i - 1; j >= 0; j--)
					gd_delete data[i];
				free(data);
				throw;
			}
		}
		SafeArray(SafeArray&& src)
		    : count(src.count)
		    , allocated(src.allocated)
		    , data(src.data)
		    , block(src.block)
		{
			src.data = 0;
			src.allocated = 0;
			src.count = 0;
		}
		explicit SafeArray(Int32 BlockSize)
		    : count(0)
		    , allocated(0)
		    , data(0)
		    , block(BlockSize)
		{
		}
		~SafeArray() override
		{
			for (Int32 i = 0; i < count; i++)
				gd_delete data[i];
			free(data);
		}

		SafeArray& operator=(const SafeArray& src)
		{
			if (this == &src)
				return *this;
			SafeArray Copy(src.block);
			Copy.require(src.count);
			for (Int32 i = 0; i < src.count; i++)
				Copy.append(src.data[i]);
			for (Int32 i = 0; i < count; i++)
				gd_delete data[i];
			free(data);
			data = Copy.data;
			count = Copy.count;
			allocated = Copy.allocated;
			block = Copy.block;
			Copy.data = 0;
			Copy.count = 0;
			Copy.allocated = 0;
			return *this;
		}

		virtual void Append(const V& v)
		{
			require(count + 1);
			append(v);
		}
		virtual void Append(const SafeArray& src)
		{
			if (&src == this)
				throw InvalidArgumentException();
			require(count + src.count);
			for (Int32 i = 0; i < src.count; i++)
				append(*src.data[i]);
		}
		virtual void Append(const V* v, Int32 Count)
		{
			require(count + Count);
			for (Int32 i = 0; i < Count; i++)
				append(v[i]);
		}
		virtual void SwapAt(Int32 i, Int32 j)
		{
			safe_swap(data[i], data[j]);
		}
		virtual void Insert(const V& v, Int32 IndexAt)
		{
			require(count + 1);
			for (Int32 i = count - 1; i >= IndexAt; i--)
				safe_swap(data[i], data[i + 1]);
			try
			{
				data[IndexAt] = gd_new V(v);
				count++;
			}
			catch (...)
			{
				for (Int32 i = IndexAt; i < count; i++)
					safe_swap(data[i], data[i + 1]);
				throw;
			}
		}
		virtual V& ElementAt(Int32 index)
		{
			return *data[index];
		}
		virtual const V& ElementAt(Int32 index) const
		{
			return *data[index];
		}
		virtual V& FirstElement()
		{
			return *data[0];
		}
		virtual const V& FirstElement() const
		{
			return *data[0];
		}
		virtual V& LastElement()
		{
			return *data[count - 1];
		}
		virtual const V& LastElement() const
		{
			return *data[count - 1];
		}
		virtual void Remove(Int32 index)
		{
			gd_delete data[index];
			for (Int32 i = index; i < count - 1; i++)
				safe_swap(data[i], data[i + 1]);
			count--;
			require(count, true);
		}
		virtual void RemoveFirst()
		{
			Remove(0);
		}
		virtual void RemoveLast()
		{
			Remove(count - 1);
		}
		virtual void Clear()
		{
			for (Int32 i = 0; i < count; i++)
				gd_delete data[i];
			free(data);
			data = 0;
			count = 0;
			allocated = 0;
		}
		virtual void SetLength(Int32 length)
		{
			if (length > count)
			{
				require(length);
				Int32 i;
				try
				{
					for (i = count; i < length; i++)
						data[i] = gd_new V();
				}
				catch (...)
				{
					for (Int32 j = i - 1; j >= count; j--)
						gd_delete data[i];
					throw;
				}
				count = length;
			}
			else if (length < count)
			{
				if (length < 0)
					throw InvalidArgumentException();
				for (Int32 i = length; i < count; i++)
					gd_delete data[i];
				count = length;
				require(count, true);
			}
		}
		Int32 Length() const
		{
			return count;
		}

		string ToString() const override
		{
			return L"SafeArray[" + string(count) + L"]";
		}

		SafeArray& operator<<(const V& v)
		{
			Append(v);
			return *this;
		}
		SafeArray& operator<<(const SafeArray& src)
		{
			Append(src);
			return *this;
		}
		V& operator[](Int32 index)
		{
			return *data[index];
		}
		const V& operator[](Int32 index) const
		{
			return *data[index];
		}
		bool friend operator==(const SafeArray& a, const SafeArray& b)
		{
			if (a.count != b.count)
				return false;
			for (Int32 i = 0; i < a.count; i++)
				if (a[i] != b[i])
					return false;
			return true;
		}
		bool friend operator!=(const SafeArray& a, const SafeArray& b)
		{
			if (a.count != b.count)
				return true;
			for (Int32 i = 0; i < a.count; i++)
				if (a[i] != b[i])
					return true;
			return false;
		}
	};
#endif

#if 0
	template <class V>
	class ObjectArray : public Object
	{
		V** data;
		Int32 count;
		Int32 allocated;
		Int32 block;
		Int32 block_align(Int32 element_count)
		{
			return ((Int64(element_count) + block - 1) / block) * block;
		}
		void require(Int32 elements, bool nothrow = false)
		{
			Int32 new_size = block_align(elements);
			if (new_size != allocated)
			{
				if (new_size > allocated)
				{
					V** new_data = reinterpret_cast<V**>(realloc(data, sizeof(V*) * new_size));
					if (new_data || new_size == 0)
					{
						data = new_data;
						allocated = new_size;
					}
					else if (!nothrow)
						throw OutOfMemoryException();
				}
				else
				{
					V** new_data = reinterpret_cast<V**>(realloc(data, sizeof(V*) * new_size));
					if (new_data || new_size == 0)
					{
						data = new_data;
						allocated = new_size;
					}
				}
			}
		}
		void append(V* v)
		{
			data[count] = v;
			v->AddRef();
			count++;
		}

	public:
		ObjectArray()
		    : count(0)
		    , allocated(0)
		    , data(0)
		    , block(0x400)
		{
		}
		ObjectArray(const ObjectArray& src)
		    : count(src.count)
		    , allocated(0)
		    , data(0)
		    , block(src.block)
		{
			require(count);
			Int32 i = 0;
			try
			{
				for (i = 0; i < count; i++)
				{
					data[i] = src.data[i];
					src.data[i]->AddRef();
				}
			}
			catch (...)
			{
				for (Int32 j = i - 1; j >= 0; j--)
					data[i]->Release();
				free(data);
				throw;
			}
		}
		ObjectArray(ObjectArray&& src)
		    : count(src.count)
		    , allocated(src.allocated)
		    , data(src.data)
		    , block(src.block)
		{
			src.data = 0;
			src.allocated = 0;
			src.count = 0;
		}
		explicit ObjectArray(Int32 BlockSize)
		    : count(0)
		    , allocated(0)
		    , data(0)
		    , block(BlockSize)
		{
		}
		~ObjectArray() override
		{
			for (Int32 i = 0; i < count; i++)
				data[i]->Release();
			free(data);
		}

		ObjectArray& operator=(const ObjectArray& src)
		{
			if (this == &src)
				return *this;
			ObjectArray Copy(src.block);
			Copy.require(src.count);
			for (Int32 i = 0; i < src.count; i++)
				Copy.append(src.data[i]);
			for (Int32 i = 0; i < count; i++)
				data[i]->Release();
			free(data);
			data = Copy.data;
			count = Copy.count;
			allocated = Copy.allocated;
			block = Copy.block;
			Copy.data = 0;
			Copy.count = 0;
			Copy.allocated = 0;
			return *this;
		}

		virtual void InsertLast(V* v)
		{
			require(count + 1);
			append(v);
		}
		virtual void InsertLast(const ObjectArray& src)
		{
			if (&src == this)
				throw InvalidArgumentException();
			require(count + src.count);
			for (Int32 i = 0; i < src.count; i++)
				append(src.data[i]);
		}
		virtual void SwapAt(Int32 i, Int32 j)
		{
			safe_swap(data[i], data[j]);
		}
		virtual void InsertAt(V* v, Int32 IndexAt)
		{
			require(count + 1);
			for (Int32 i = count - 1; i >= IndexAt; i--)
				safe_swap(data[i], data[i + 1]);
			try
			{
				data[IndexAt] = v;
				v->AddRef();
				count++;
			}
			catch (...)
			{
				for (Int32 i = IndexAt; i < count; i++)
					safe_swap(data[i], data[i + 1]);
				throw;
			}
		}
		virtual V* ElementAt(Int32 index) const
		{
			return data[index];
		}
		virtual V* FirstElement() const
		{
			return data[0];
		}
		virtual V* LastElement() const
		{
			return data[count - 1];
		}
		virtual void EraseAt(Int32 index)
		{
			data[index]->Release();
			for (Int32 i = index; i < count - 1; i++)
				safe_swap(data[i], data[i + 1]);
			count--;
			require(count, true);
		}
		virtual void RemoveFirst()
		{
			EraseAt(0);
		}
		virtual void RemoveLast()
		{
			EraseAt(count - 1);
		}
		virtual void Clear()
		{
			for (Int32 i = 0; i < count; i++)
				data[i]->Release();
			free(data);
			data = 0;
			count = 0;
			allocated = 0;
		}
		Int32 GetLength() const
		{
			return count;
		}

		string ToString() const override
		{
			string result = L"ObjectArray[";
			if (count)
				for (Int32 i = 0; i < count; i++)
				{
					result += data[i]->ToString() + ((i == count - 1) ? L"]" : L", ");
				}
			else
				result += L"]";
			return result;
		}

		ObjectArray& operator<<(V* v)
		{
			InsertLast(v);
			return *this;
		}
		ObjectArray& operator<<(const ObjectArray& src)
		{
			InsertLast(src);
			return *this;
		}
		V& operator[](Int32 index) const
		{
			return *data[index];
		}
		bool friend operator==(const ObjectArray& a, const ObjectArray& b)
		{
			if (a.count != b.count)
				return false;
			for (Int32 i = 0; i < a.count; i++)
				if (a[i] != b[i])
					return false;
			return true;
		}
		bool friend operator!=(const ObjectArray& a, const ObjectArray& b)
		{
			if (a.count != b.count)
				return true;
			for (Int32 i = 0; i < a.count; i++)
				if (a[i] != b[i])
					return true;
			return false;
		}
	};
#endif

	template <class A>
	void SortArray(A& volume, bool ascending = true)
	{
		Int32 len = volume.GetLength();
		for (Int32 i = 0; i < len - 1; i++)
		{
			for (Int32 j = i + 1; j < len; j++)
			{
				bool swap = false;
				if (ascending)
				{
					swap = volume[i] > volume[j];
				}
				else
				{
					swap = volume[i] < volume[j];
				}
				if (swap)
				{
					volume.SwapAt(i, j);
				}
			}
		}
	}
	template <class V>
	Int32 BinarySearchLE(const Array<V>& volume, const V& value)
	{
		if (!volume.GetLength())
			throw InvalidArgumentException();
		Int32 bl = 0, bh = volume.GetLength();
		while (bh - bl > 1)
		{
			Int32 mid = (bl + bh) / 2;
			if (volume[mid] > value)
			{
				bh = mid;
			}
			else
			{
				bl = mid;
			}
		}
		if (bl == 0)
		{
			if (value < volume[0])
				bl = -1;
		}
		return bl;
	}

#if 0
	template <class O>
	class SafePointer final : Object
	{
	private:
		O* reference = 0;

	public:
		SafePointer()
		    : reference(0)
		{
		}
		SafePointer(O* ref)
		    : reference(ref)
		{
		}
		SafePointer(const SafePointer& src)
		{
			reference = src.reference;
			if (reference)
				reference->AddRef();
		}
		SafePointer(SafePointer&& src)
		    : reference(src.reference)
		{
			src.reference = 0;
		}
		~SafePointer()
		{
			if (reference)
				reference->Release();
			reference = 0;
		}
		SafePointer& operator=(const SafePointer& src)
		{
			if (this == &src)
				return *this;
			if (reference)
				reference->Release();
			reference = src.reference;
			if (reference)
				reference->AddRef();
			return *this;
		}

		O& operator*() const
		{
			return *reference;
		}
		O* operator->() const
		{
			return reference;
		}
		operator O*() const
		{
			return reference;
		}
		operator bool() const
		{
			return reference != 0;
		}
		O* Inner() const
		{
			return reference;
		}
		O** InnerRef()
		{
			return &reference;
		}

		void SetReference(O* ref)
		{
			if (reference)
				reference->Release();
			reference = ref;
		}
		void SetRetain(O* ref)
		{
			if (reference)
				reference->Release();
			reference = ref;
			if (reference)
				reference->AddRef();
		}

		/*bool friend operator==(const SafePointer& a, const O* b)
		{
			return a.reference == b;
		}*/
		bool friend operator==(const SafePointer& a, const SafePointer& b)
		{
			return a.reference == b.reference;
		}
		//bool friend operator!=(const SafePointer& a, const O* b)
		//{
		//	return a.reference != b;
		//}
		bool friend operator!=(const SafePointer& a, const SafePointer& b)
		{
			return a.reference != b.reference;
		}
	};
#endif
}
