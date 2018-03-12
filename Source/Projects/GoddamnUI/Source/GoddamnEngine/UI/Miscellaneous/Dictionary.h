#pragma once

#include "../EngineBase.h"

namespace Engine
{
	namespace Dictionary
	{
		template <class K, class O>
		class DictionaryKeyPair
		{
		public:
			K key;
			SafePointer<O> object;

			DictionaryKeyPair()
			{
			}
			DictionaryKeyPair(const K& object_key, O* object_refrence)
			    : key(object_key)
			    , object(object_refrence)
			{
				if (object)
					object->AddRef();
			}
			DictionaryKeyPair(const DictionaryKeyPair& src)
			    : key(src.key)
			    , object(src.object.Get())
			{
				if (object)
					object->AddRef();
			}
			DictionaryKeyPair(DictionaryKeyPair&& src)
			    : key(src.key)
			    , object(src.object)
			{
				src.object.operator=(0);
			}
			DictionaryKeyPair& operator=(const DictionaryKeyPair& src)
			{
				if (this == &src)
					return *this;
				key = src.key;
				object = src.object;
				if (object)
					object->AddRef();
				return *this;
			}

			bool friend operator==(const DictionaryKeyPair& a, const DictionaryKeyPair& b)
			{
				return a.key == b.key;
			}
			bool friend operator!=(const DictionaryKeyPair& a, const DictionaryKeyPair& b)
			{
				return a.key != b.key;
			}
			bool friend operator<(const DictionaryKeyPair& a, const DictionaryKeyPair& b)
			{
				return a.key < b.key;
			}
			bool friend operator>(const DictionaryKeyPair& a, const DictionaryKeyPair& b)
			{
				return a.key > b.key;
			}
			bool friend operator<=(const DictionaryKeyPair& a, const DictionaryKeyPair& b)
			{
				return a.key <= b.key;
			}
			bool friend operator>=(const DictionaryKeyPair& a, const DictionaryKeyPair& b)
			{
				return a.key >= b.key;
			}
		};

		/*template <class K, class O>
		using Dictionary = Map<K, O>;*/
#if 1
		template <class K, class O>
		class Dictionary : public Object
		{
			Array<DictionaryKeyPair<K, O>> data;

		public:
			Dictionary()
			    : data()
			{
			}
			explicit Dictionary(Int32 BlockSize)
			    : data(BlockSize)
			{
			}

			virtual bool InsertLast(const K& key, O* object)
			{
				if (data.GetLength())
				{
					auto pair = DictionaryKeyPair<K, O>(key, object);
					Int32 element_lesser_equal = BinarySearchLE(data, pair);
					if (element_lesser_equal < 0)
						data.InsertAt(0, pair);
					else if (data[element_lesser_equal] == pair)
					{
						return false;
					}
					else
					{
						if (element_lesser_equal == data.GetLength() - 1)
							data.InsertLast(pair);
						else
							data.InsertAt(element_lesser_equal + 1, pair);
					}
				}
				else
					data.InsertLast(DictionaryKeyPair<K, O>(key, object));
				return true;
			}
			virtual O* ElementByKey(const K& key) const
			{
				if (!data.GetLength())
					return 0;
				Int32 element = BinarySearchLE(data, DictionaryKeyPair<K, O>(key, 0));
				if (element < 0 || data[element].key != key)
					return 0;
				return data[element].object;
			}
			virtual const DictionaryKeyPair<K, O>& ElementByIndex(Int32 index) const
			{
				return data[index];
			}
			virtual DictionaryKeyPair<K, O>& ElementByIndex(Int32 index)
			{
				return data[index];
			}
			virtual void RemoveObject(O* object)
			{
				for (Int32 i = 0; i < data.GetLength(); i++)
					if (data[i].object.Get() == object)
					{
						data.EraseAt(i);
						break;
					}
			}
			virtual void RemoveByKey(const K& key)
			{
				if (!data.GetLength())
					return;
				Int32 element = BinarySearchLE(data, DictionaryKeyPair<K, O>(key, 0));
				if (element >= 0 && data[element].key == key)
					data.EraseAt(element);
			}
			virtual void RemoveAt(Int32 index)
			{
				data.EraseAt(index);
			}
			virtual void Clear()
			{
				data.Clear();
			}
			bool ElementPresent(const K& key) const
			{
				if (!data.GetLength())
					return false;
				Int32 element = BinarySearchLE(data, DictionaryKeyPair<K, O>(key, 0));
				if (element >= 0 && data[element].key == key)
					return true;
				else
					return false;
			}
			bool ElementPresent(O* object) const
			{
				for (Int32 i = 0; i < data.GetLength(); i++)
					if (data[i].object.Get() == object)
						return true;
				return false;
			}
			Int32 Length() const
			{
				return data.GetLength();
			}

			string ToString() const override
			{
				return L"Dictionary";
			}
			O* operator[](const K& key) const
			{
				return ElementByKey(key);
			}
			const DictionaryKeyPair<K, O>& operator[](Int32 index) const
			{
				return data[index];
			}
			DictionaryKeyPair<K, O>& operator[](Int32 index)
			{
				return data[index];
			}
		};
#endif

		enum class ExcludePolicy
		{
			DoNotExclude = 0,
			ExcludeLeastRefrenced = 1
		};
		template <class K, class O>
		class ObjectCache : public Dictionary<K, O>
		{
			Int32 max_capacity;
			ExcludePolicy policy;

		public:
			ObjectCache()
			    : max_capacity(0x100)
			    , Dictionary<K, O>(0x20)
			    , policy(ExcludePolicy::DoNotExclude)
			{
			}
			explicit ObjectCache(Int32 capacity)
			    : max_capacity(capacity)
			    , Dictionary<K, O>(0x20)
			    , policy(ExcludePolicy::DoNotExclude)
			{
			}
			explicit ObjectCache(Int32 capacity, ExcludePolicy exclude_policy)
			    : max_capacity(capacity)
			    , Dictionary<K, O>(0x20)
			    , policy(exclude_policy)
			{
			}
			virtual bool InsertLast(const K& key, O* object) override
			{
				if (Dictionary<K, O>::Length() == max_capacity)
				{
					if (policy == ExcludePolicy::DoNotExclude)
						return false;
					else if (policy == ExcludePolicy::ExcludeLeastRefrenced)
					{
						UInt32 least = Dictionary<K, O>::ElementByIndex(0).object->GetReferenceCount();
						Int32 least_index = 0;
						for (Int32 i = 1; i < Dictionary<K, O>::Length(); i++)
						{
							UInt32 l;
							if ((l = Dictionary<K, O>::ElementByIndex(i).object->GetReferenceCount()) < least)
							{
								least = l;
								least_index = i;
							}
						}
						Dictionary<K, O>::RemoveAt(least_index);
						return Dictionary<K, O>::InsertLast(key, object);
					}
					else
						return false;
				}
				else
					return Dictionary<K, O>::InsertLast(key, object);
			}
			string ToString() const override
			{
				return L"ObjectCache";
			}
		};
	}
}
