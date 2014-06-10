#ifndef GD_CORE_OBJECT_REFPTR
#define GD_CORE_OBJECT_REFPTR

#include <GoddamnEngine/Include.h>
#include <GoddamnEngine/Core/Object/Object.h>

GD_NAMESPACE_BEGIN

	/// Class provides automatic reference counting
	///	Usage:
	///	@code
	///		// Manual counting:
	///		{
	///			MyObject* const object = ...;
	///			object->AddReference();
	///			object->DoSomething();
	///			DoSomething2(object);
	///			object->RemoveReference();
	///		}
	///
	///		// Automatic counting:
	///		{
	///			auto object = RefPtr<MyObject>(...);
	///			object->DoSomething();
	///			DoSomething2(object);
	///		}
	///	@endcode
	template<class ObjectType>
	class RefPtr final
	{
		static_assert(TypeTraits::IsBaseType<Object, ObjectType>::Value,
			"'RefPtr<ObjectType>' error: Using 'RefPtr' class is allowed only for Object-derived classes");

	private:
		ObjectType* object;

	public:

		/// @brief			Initializes smart pointer with existing object
		/// @param object	Object to provide reference counting
		/// @note			Reference counter is not incremented
		GDINL RefPtr(ObjectType* const object = nullptr);

		/// @brief			Copies smart pointer from another one
		/// @param refPtr	Smart pointer to copy from
		/// @note			Reference counter is incremented
		GDINL RefPtr(const RefPtr<ObjectType>& refPtr);

		/// @brief			Destroys smart pointer and decrements reference counter 
		GDINL ~RefPtr();

		/// @brief			Returns real pointer to object this smart pointer
		///					contains
		///	@returns		Real object pointer
		GDINL const ObjectType* GetPointer() const;

		/// @see RefPtr::GetPointer
		GDINL ObjectType* GetPointer();

		GDINL ObjectType* Release()
		{
			ObjectType* object = self->object;
			self->object = nullptr;
			return object;
		}

		/// @name Operators
		/// @{

		GDINL bool operator== (ObjectType const* const Obj) const { return self->object == Obj; }
		GDINL bool operator!= (ObjectType const* const Obj) const { return self->object != Obj; }
		GDINL bool operator== (const RefPtr<ObjectType>& refPtr) const;

		GDINL RefPtr<ObjectType>& operator= (const RefPtr<ObjectType>& refPtr);

		GDINL RefPtr<ObjectType>& operator= (ObjectType* const ptr);

		GDINL const ObjectType* operator-> () const;

		GDINL ObjectType* operator-> ();

		GDINL const ObjectType& operator* () const;

		GDINL ObjectType& operator* ();

		GDINL RefPtr<ObjectType>& operator++ ();

		GDINL RefPtr<ObjectType>& operator-- ();

		/// @}
	};

#if (!defined(GD_DOCUMENTATION))
	template<typename ObjectType, typename ThisObjectType>
	auto IterateChildObjects(RefPtr<ThisObjectType> const& TheObject) -> decltype(IterateChildObjects<ObjectType>(TheObject.GetPointer()))
	{	// Returning object wrapped with iteration adapter.
		return IterateChildObjects<ObjectType>(TheObject.GetPointer());
	}
#endif	// if (!defined(GD_DOCUMENTATION))

GD_NAMESPACE_END

#include <GoddamnEngine/Core/Object/RefPtr/RefPtr.inl>

#endif