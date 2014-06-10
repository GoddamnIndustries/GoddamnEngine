#include <GoddamnEngine/Core/Object/Object.h>
#include <GoddamnEngine/Core/Reflection/FieldInformation/FieldInformation.h>

GD_NAMESPACE_BEGIN

	//////////////////////////////////////////////////////////////////////////
	// Object class
	//////////////////////////////////////////////////////////////////////////

	GD_TYPEINFORMATION_IMPLEMENTATION_C(Object, NoBaseClass, GDAPI, nullptr);

	//////////////////////////////////////////////////////////////////////////
	// Constructor / Destructor
	//////////////////////////////////////////////////////////////////////////

	Object::Object(ObjectTreeLockingFlags const TreeLockingFlags /* = SomeLongValue */, Object* const ParentObject /* = nullptr */) : TreeLockingFlags(TreeLockingFlags)
	{	// If child object specified, attaching to it.
		if (ParentObject != nullptr) self->AttachToObject(ParentObject);
	}

	Object::~Object()
	{
		GD_DEBUG_ASSERT((self->RefenceCount == 0),
			"Object destructor invoked but reference counter is not zero. "
			"Please, do not delete objects yourself and use 'Object::RemoveReference()' method");

		if (self->GetParentObject() != nullptr)
			self->DetachFromParentObject(self->GetTreeLockingFlags() & (~TreeLockingFlagsDisableDetaching));

		for (auto const ChildObject : IterateChildObjects<Object>(self))
		{
			ChildObject->ParentObject          = nullptr;
			ChildObject->NextSiblingObject     = nullptr;
			ChildObject->PreviousSiblingObject = nullptr;
			ChildObject->LastChildObject       = nullptr;
			ChildObject->RemoveReference();
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// Reference Counting
	//////////////////////////////////////////////////////////////////////////

	ObjectRefenceCount Object::AddReference() const
	{
		GD_DEBUG_ASSERT((self->RefenceCount != 0),	"Reference counter is zero. Please, use 'Object::AddReference' and 'Object::RemoveReference' to provide correct reference counting");
		return (self->RefenceCount += 1);
	}

	ObjectRefenceCount Object::RemoveReference() const
	{
		GD_DEBUG_ASSERT((self->RefenceCount != 0), "Reference counter is zero. Please, use 'Object::AddReference' and 'Object::RemoveReference' to provide correct reference counting");
		
		if ((self->RefenceCount -= 1) == 0)
			delete self;

		GD_WARNING_SUPPRESS(6001);
		return (self->RefenceCount);
	}

	//////////////////////////////////////////////////////////////////////////
	// Object tree management
	//////////////////////////////////////////////////////////////////////////

	void Object::AttachToObject(Object* const ParentObject, ObjectTreeLockingFlags const CustomTreeLockingFlags)
	{
		if ((CustomTreeLockingFlags & Object::TreeLockingFlagsCheckValidity) != 0)
		{
			GD_ASSERT((self->GetParentObject() == nullptr) || ((CustomTreeLockingFlags & Object::TreeLockingFlagsDisableAttachingToObject) == 0),
				"'TreeLockingFlagsDisableAttaching' flag was set, unable to attach to different object");
			GD_ASSERT(((self->GetParentObject() == nullptr) && ((self->GetNextSiblingObject() == nullptr) && (self->GetPreviousSiblingObject() == nullptr))),
				"This object is already attached, please detach from previous parent first");
		}

		GD_ASSERT((ParentObject != nullptr), "Attempt to attach to nullptr object");

		if ((CustomTreeLockingFlags & TreeLockingFlagsAutomaticReferenceCounting) != 0)
			self->AddReference();

		/**/self->ParentObject = ParentObject;
		if (self->ParentObject->GetFirstChildObject() == nullptr)
			self->ParentObject->FirstChildObject = self;
		if (self->ParentObject->GetLastChildObject() != nullptr)
		{
			self->PreviousSiblingObject = ParentObject->GetLastChildObject();
			self->PreviousSiblingObject->NextSiblingObject = self;
		}

		self->ParentObject->LastChildObject = self;
	}

	void Object::DetachFromParentObject(ObjectTreeLockingFlags const CustomTreeLockingFlags)
	{
		if ((CustomTreeLockingFlags & Object::TreeLockingFlagsCheckValidity) != 0)
		{
			GD_ASSERT((CustomTreeLockingFlags & Object::TreeLockingFlagsDisableDetaching) == 0,
				"'Object::DetachFromParent' error: 'TreeLockingFlagsDisableDetaching' flag was set, "
				"unable to detach from parent object");

			GD_ASSERT(self->GetParentObject() != nullptr,
				"'Object::DetachFromParent' error: this object was not attched to any objects");
		}


		if ((CustomTreeLockingFlags & TreeLockingFlagsAutomaticReferenceCounting) != 0)
			self->RemoveReference();

		if (self->GetParentObject()->LastChildObject == self)
			self->GetParentObject()->LastChildObject = nullptr;
		if (self->ParentObject->GetFirstChildObject() == self)
			self->ParentObject->FirstChildObject = nullptr;
		if (self->GetPreviousSiblingObject() != nullptr)
			self->GetPreviousSiblingObject()->NextSiblingObject = self->GetNextSiblingObject();
		if (self->GetNextSiblingObject() != nullptr)
			self->GetNextSiblingObject()->PreviousSiblingObject = self->GetPreviousSiblingObject();

		self->PreviousSiblingObject = nullptr;
		self->NextSiblingObject     = nullptr;
		self->ParentObject          = nullptr;
	}

	//////////////////////////////////////////////////////////////////////////
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
			GD_ASSERT(((self->GetParentObject() == SiblingObject->GetParentObject()) && (self->GetParentObject() != nullptr)),
				"Unable to swap objects that are not sibling");
		}

		if (self == SiblingObject)
		{
			return;
		}

		Object* const parentObject = self->GetParentObject();
		if (parentObject->GetLastChildObject() == self)
		{
			parentObject->LastChildObject = SiblingObject;
		}

		if (parentObject->GetLastChildObject() == SiblingObject)
		{
			parentObject->LastChildObject = self;
		}

		Object* const SelfPreviousSiblingObject = self->GetPreviousSiblingObject();
		Object* const SelfNextSiblingObject = self->GetNextSiblingObject();

		self->PreviousSiblingObject = SiblingObject->GetPreviousSiblingObject();
		self->NextSiblingObject = SiblingObject->GetNextSiblingObject();

		SiblingObject->PreviousSiblingObject = SelfPreviousSiblingObject;
		SiblingObject->NextSiblingObject = SelfNextSiblingObject;
	}

	//////////////////////////////////////////////////////////////////////////
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

			GD_ASSERT(((self->GetParentObject() == SiblingObject->GetParentObject())
				&& (self->GetParentObject() != nullptr)),
				"Unable to swap objects that are not sibling");

			GD_ASSERT((self == self->GetParentObject()->GetLastChildObject()),
				"Moving is supported only for objects that are at the end of child list");
		}

		if ((self->GetPreviousSiblingObject() == SiblingObject) || (self == SiblingObject))
		{
			return;
		}

		self->GetParentObject()->LastChildObject = self->GetPreviousSiblingObject();
		self->GetPreviousSiblingObject()->NextSiblingObject = nullptr;

		self->PreviousSiblingObject = SiblingObject;
		self->NextSiblingObject = SiblingObject->GetNextSiblingObject();
		SiblingObject->GetNextSiblingObject()->PreviousSiblingObject = self;
		SiblingObject->NextSiblingObject = self;
	}

#if (defined(GD_DEBUG) || defined(GD_DOCUMENTATION))
	void Object::DumpTreeData(String& dumpedTreeData) const
	{
		static size_t DumpedTreeDataRecursionLevel = 0;
		if (dumpedTreeData.GetSize() == 0)
		{
			DumpedTreeDataRecursionLevel = 0;
		}

		DumpedTreeDataRecursionLevel += 1;

		for (Object const*
			 object  = self->GetLastChildObject(); 
			 object != nullptr;
			 object  = object->GetPreviousSiblingObject())
		{
			dumpedTreeData += (String(nullptr, DumpedTreeDataRecursionLevel - 1, '\t')
				+ String::Format("Object 0x%x with RTTI: %s\n", object, &object->GetTypeInformation()->Name[0]));
			 
			object->DumpTreeData(dumpedTreeData);
		}

		DumpedTreeDataRecursionLevel -= 1;
	}
#endif
GD_NAMESPACE_END
