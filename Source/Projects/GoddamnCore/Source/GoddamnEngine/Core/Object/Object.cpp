#include <GoddamnEngine/Core/Object/Object.h>

GD_NAMESPACE_BEGIN

	/// ==========================================================================================
	// Object class
	/// ==========================================================================================

	GD_CLASSINFO_IMPLEMENTATION(Object, IClass, GDAPI);

	/// ==========================================================================================
	// Constructor / Destructor
	/// ==========================================================================================

	Object::Object(ObjectTreeLockingFlags const TreeLockingFlags /* = SomeLongValue */, Object* const ParentObject /* = nullptr */) : TreeLockingFlags(TreeLockingFlags)
	{	// If child object specified, attaching to it.
		if (ParentObject != nullptr) this->AttachToObject(ParentObject);
	}

	Object::~Object()
	{
		GD_DEBUG_ASSERT((this->ReferenceCount == 0),
			"Object destructor invoked but reference counter is not zero. "
			"Please, do not delete objects yourthis and use 'Object::RemoveReference()' method");

		if (this->GetParentObject() != nullptr)
			this->DetachFromParentObject(this->GetTreeLockingFlags() & (~TreeLockingFlagsDisableDetaching));

		for (auto const ChildObject : IterateChildObjects<Object>(this))
		{
			ChildObject->ParentObject          = nullptr;
			ChildObject->NextSiblingObject     = nullptr;
			ChildObject->PreviousSiblingObject = nullptr;
			ChildObject->LastChildObject       = nullptr;
			ChildObject->RemoveReference();
		}
	}

	/// ==========================================================================================
	// Reference Counting
	/// ==========================================================================================

	ObjectRefenceCount Object::AddReference() const
	{
		GD_DEBUG_ASSERT((this->ReferenceCount != 0),	"Reference counter is zero. Please, use 'Object::AddReference' and 'Object::RemoveReference' to provide correct reference counting");
		return (this->ReferenceCount += 1);
	}

	ObjectRefenceCount Object::RemoveReference() const
	{
		GD_DEBUG_ASSERT((this->ReferenceCount != 0), "Reference counter is zero. Please, use 'Object::AddReference' and 'Object::RemoveReference' to provide correct reference counting");
		if ((--this->ReferenceCount) == 0) {
			delete this;
		}

		return (this->ReferenceCount);
	}

	/// ==========================================================================================
	// Object tree management
	/// ==========================================================================================

	void Object::AttachToObject(Object* const ParentObject, ObjectTreeLockingFlags const CustomTreeLockingFlags)
	{
		if ((CustomTreeLockingFlags & Object::TreeLockingFlagsCheckValidity) != 0)
		{
			GD_ASSERT((this->GetParentObject() == nullptr) || ((CustomTreeLockingFlags & Object::TreeLockingFlagsDisableAttachingToObject) == 0),
				"'TreeLockingFlagsDisableAttaching' flag was set, unable to attach to different object");
			GD_ASSERT(((this->GetParentObject() == nullptr) && ((this->GetNextSiblingObject() == nullptr) && (this->GetPreviousSiblingObject() == nullptr))),
				"This object is already attached, please detach from previous parent first");
		}

		GD_ASSERT((ParentObject != nullptr), "Attempt to attach to nullptr object");

		if ((CustomTreeLockingFlags & TreeLockingFlagsAutomaticReferenceCounting) != 0)
			this->AddReference();

		/**/this->ParentObject = ParentObject;
		if (this->ParentObject->GetFirstChildObject() == nullptr)
			this->ParentObject->FirstChildObject = this;
		if (this->ParentObject->GetLastChildObject() != nullptr)
		{
			this->PreviousSiblingObject = ParentObject->GetLastChildObject();
			this->PreviousSiblingObject->NextSiblingObject = this;
		}

		this->ParentObject->LastChildObject = this;
	}

	void Object::DetachFromParentObject(ObjectTreeLockingFlags const CustomTreeLockingFlags)
	{
		if ((CustomTreeLockingFlags & Object::TreeLockingFlagsCheckValidity) != 0)
		{
			GD_ASSERT((CustomTreeLockingFlags & Object::TreeLockingFlagsDisableDetaching) == 0,
				"'Object::DetachFromParent' error: 'TreeLockingFlagsDisableDetaching' flag was set, "
				"unable to detach from parent object");

			GD_ASSERT(this->GetParentObject() != nullptr,
				"'Object::DetachFromParent' error: this object was not attched to any objects");
		}


		if ((CustomTreeLockingFlags & TreeLockingFlagsAutomaticReferenceCounting) != 0)
			this->RemoveReference();

		if (this->GetParentObject()->LastChildObject == this)
			this->GetParentObject()->LastChildObject = nullptr;
		if (this->ParentObject->GetFirstChildObject() == this)
			this->ParentObject->FirstChildObject = nullptr;
		if (this->GetPreviousSiblingObject() != nullptr)
			this->GetPreviousSiblingObject()->NextSiblingObject = this->GetNextSiblingObject();
		if (this->GetNextSiblingObject() != nullptr)
			this->GetNextSiblingObject()->PreviousSiblingObject = this->GetPreviousSiblingObject();

		this->PreviousSiblingObject = nullptr;
		this->NextSiblingObject     = nullptr;
		this->ParentObject          = nullptr;
	}

	/// ==========================================================================================
	void Object::SwapToSiblingObject(
		_In_ Object* const SiblingObject, 
		_In_ ObjectTreeLockingFlags const CustomTreeLockingFlags
	)
	{
		GD_ASSERT((SiblingObject != nullptr),
			"Attempted swapping with 'nullptr' Object");

		if (((CustomTreeLockingFlags & Object::TreeLockingFlagsCheckValidity) != 0)	|| ((SiblingObject->TreeLockingFlags & Object::TreeLockingFlagsCheckValidity) != 0))
		{
			GD_ASSERT((((CustomTreeLockingFlags & Object::TreeLockingFlagsDisableSiblingSwapping) == 0)	&& ((SiblingObject->TreeLockingFlags & Object::TreeLockingFlagsDisableSiblingSwapping) == 0)),
				"'TreeLockingFlagsDisableSiblingSwapping' was set, unable to swap child objects");
			GD_ASSERT(((this->GetParentObject() == SiblingObject->GetParentObject()) && (this->GetParentObject() != nullptr)),
				"Unable to swap objects that are not sibling");
		}

		if (this == SiblingObject)
		{
			return;
		}

		Object* const parentObject = this->GetParentObject();
		if (parentObject->GetLastChildObject() == this)
		{
			parentObject->LastChildObject = SiblingObject;
		}

		if (parentObject->GetLastChildObject() == SiblingObject)
		{
			parentObject->LastChildObject = this;
		}

		Object* const thisPreviousSiblingObject = this->GetPreviousSiblingObject();
		Object* const thisNextSiblingObject = this->GetNextSiblingObject();

		this->PreviousSiblingObject = SiblingObject->GetPreviousSiblingObject();
		this->NextSiblingObject = SiblingObject->GetNextSiblingObject();

		SiblingObject->PreviousSiblingObject = thisPreviousSiblingObject;
		SiblingObject->NextSiblingObject = thisNextSiblingObject;
	}

	/// ==========================================================================================
	void Object::MoveToSiblingObject(
		_In_ Object* const SiblingObject,
		_In_ ObjectTreeLockingFlags const CustomTreeLockingFlags
	)
	{
		GD_ASSERT((SiblingObject != nullptr),
			"Attempted swapping with 'nullptr' Object");
		 
		if (((CustomTreeLockingFlags & Object::TreeLockingFlagsCheckValidity) != 0)
			|| ((SiblingObject->TreeLockingFlags & Object::TreeLockingFlagsCheckValidity) != 0))
		{
			GD_ASSERT((((CustomTreeLockingFlags & Object::TreeLockingFlagsDisableSiblingMoving) == 0)
				&& ((SiblingObject->TreeLockingFlags & Object::TreeLockingFlagsDisableSiblingMoving) == 0)),
				"'TreeLockingFlagsDisableSiblingMoving' was set, unable to move child objects");

			GD_ASSERT(((this->GetParentObject() == SiblingObject->GetParentObject())
				&& (this->GetParentObject() != nullptr)),
				"Unable to swap objects that are not sibling");

			GD_ASSERT((this == this->GetParentObject()->GetLastChildObject()),
				"Moving is supported only for objects that are at the end of child list");
		}

		if ((this->GetPreviousSiblingObject() == SiblingObject) || (this == SiblingObject))
		{
			return;
		}

		this->GetParentObject()->LastChildObject = this->GetPreviousSiblingObject();
		this->GetPreviousSiblingObject()->NextSiblingObject = nullptr;

		this->PreviousSiblingObject = SiblingObject;
		this->NextSiblingObject = SiblingObject->GetNextSiblingObject();
		SiblingObject->GetNextSiblingObject()->PreviousSiblingObject = this;
		SiblingObject->NextSiblingObject = this;
	}

#if (defined(GD_DEBUG) || defined(GD_DOCUMENTATION))
	void Object::DumpTreeData(String& dumpedTreeData) const
	{
		static size_t DumpedTreeDataRecursionLevel = 0;
		if (dumpedTreeData.GetLength() == 0)
		{
			DumpedTreeDataRecursionLevel = 0;
		}

		DumpedTreeDataRecursionLevel += 1;

		for (Object const*
			 object  = this->GetLastChildObject(); 
			 object != nullptr;
			 object  = object->GetPreviousSiblingObject())
		{
			dumpedTreeData += (String(DumpedTreeDataRecursionLevel - 1, '\t') + String::Format("Object 0x%x with RTTI: %s\n", object, &object->GetClassInfo()->GetName()[0]));
			 
			object->DumpTreeData(dumpedTreeData);
		}

		DumpedTreeDataRecursionLevel -= 1;
	}
#endif
GD_NAMESPACE_END
