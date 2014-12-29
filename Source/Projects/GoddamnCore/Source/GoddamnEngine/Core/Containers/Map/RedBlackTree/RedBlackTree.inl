/// ==========================================================================================
/// RedBlackTree.inl - Red Black tree data structure implementation.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// ==========================================================================================

#if (!defined(GD_CORE_CONTAINERS_MAP_RED_BLACK_TREE))
#	error Attempting to include 'RedBlackTree.inl' file. Please, use 'RedBlackTree.h' instead.
#endif	// if (!defined(GD_CORE_CONTAINERS_MAP_RED_BLACK_TREE))

/// ==========================================================================================
/// Adapted from the following code written by Emin Martinian:
/// http://web.mit.edu/~emin/www/source_code/red_black_tree/index.html
///
/// Copyright (c) 2001 Emin Martinian
///
/// Redistribution and use in source and binary forms, with or without modification, are 
/// permitted provided that neither the name of Emin Martinian nor the names of any 
/// contributors are be used to endorse or promote products derived from this software without 
/// specific prior written permission.
///
/// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS 
/// OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
/// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE 
/// COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
/// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE 
/// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
/// AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
/// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED 
/// OF THE POSSIBILITY OF SUCH DAMAGE.
/// ==========================================================================================

GD_NAMESPACE_BEGIN

	/// ==========================================================================================
	/// RedBlackTree class.
	/// ==========================================================================================
	
	/// ------------------------------------------------------------------------------------------
	/// Constructor and destructor.
	/// ------------------------------------------------------------------------------------------

	template<typename KeyType, typename ValueType>
	inline RedBlackTree<KeyType, ValueType>::RedBlackTree()
	{
		this->CreateNode(this->NullNode);
		this->CreateNode(this->RootNode);
	}

	template<typename KeyType, typename ValueType>
	inline RedBlackTree<KeyType, ValueType>::RedBlackTree(RedBlackTree&& Other)
	{
		this->RootNode = Other.RootNode;
		this->NullNode = Other.NullNode;
		this->Length = Other.Length;

		Other.RootNode = nullptr;
		Other.NullNode = nullptr;
		Other.Length = 0;
	}

	template<typename KeyType, typename ValueType>
	inline RedBlackTree<KeyType, ValueType>::RedBlackTree(RedBlackTree const& Other)
	{
		this->CreateNode(this->NullNode);
		this->CopyNode(Other.RootNode, Other.NullNode, this->RootNode);
		this->Length = Other.Length;
		this->RootNode->ParentNode = this->NullNode;
	}

	template<typename KeyType, typename ValueType>
	inline RedBlackTree<KeyType, ValueType>::~RedBlackTree()
	{
		this->DestroyNode(this->RootNode->Left);
		delete this->NullNode;
		delete this->RootNode;
	}

	/// ------------------------------------------------------------------------------------------
	/// Private methods.
	/// ------------------------------------------------------------------------------------------

	template<typename KeyType, typename ValueType>
	inline void RedBlackTree<KeyType, ValueType>::CreateNode(Node*& NewNode)
	{
		NewNode = new Node();
		NewNode->Left = this->NullNode;
		NewNode->Right = this->NullNode;
		NewNode->Parent = this->NullNode;
		NewNode->Color = NodeColor::Black;
		NewNode->Data = nullptr;
	}

	template<typename KeyType, typename ValueType>
	inline void RedBlackTree<KeyType, ValueType>::CopyNode(Node* const TheNode, Node* const NullNode, Node*& OutNode)
	{
		if (TheNode != NullNode) {
			OutNode = new Node();
			OutNode->Value = new Pair<KeyType, ValueType>(*TheNode->Value);
			OutNode->Color = TheNode->Color;
			this->CopyNode(TheNode->Left, NullNode, OutNode->Left);
			this->CopyNode(TheNode->Right, NullNode, OutNode->Right);
			OutNode->Left->Parent = OutNode;
			OutNode->Right->Parent = OutNode;
		} else {
			OutNode = this->NullNode;
		}
	}

	template<typename KeyType, typename ValueType>
	inline void RedBlackTree<KeyType, ValueType>::DestroyNode(Node* const TheNode)
	{
		if (TheNode != this->NullNode) {
			this->DestroyNode(TheNode->Left);
			this->DestroyNode(TheNode->Right);
			delete TheNode->Data;
			delete TheNode;
		}
	}

	template<typename KeyType, typename ValueType>
	inline typename RedBlackTree<KeyType, ValueType>::Node const* RedBlackTree<KeyType, ValueType>::NextNode(Node const* const TheNode) const
	{
		Node const* IterNode = TheNode->Right;
		if (this->NullNode != IterNode) {
			while (IterNode->Left != this->NullNode) {
				IterNode = IterNode->Left;
			}

			return IterNode;
		} else {
			IterNode = TheNode->Parent;
			while (TheNode == IterNode->Right) {
				TheNode = IterNode;
				IterNode = IterNode->Parent;
			}

			if (IterNode == this->RootNode) {
				return this->NullNode;
			} else {
				return IterNode;
			}
		}
	}

	template<typename KeyType, typename ValueType>
	inline typename RedBlackTree<KeyType, ValueType>::Node* RedBlackTree<KeyType, ValueType>::NextNode(Node* const TheNode)
	{
		return const_cast<Node*>(const_cast<RedBlackTree const*>(this)->NextNode(TheNode));
	}

	template<typename KeyType, typename ValueType>
	inline typename RedBlackTree<KeyType, ValueType>::Node const* RedBlackTree<KeyType, ValueType>::PrevNode(Node const* const TheNode) const
	{
		Node* IterNode = TheNode->Left;
		if (this->NullNode != IterNode) {
			while (IterNode->Right != this->NullNode) {
				IterNode = IterNode->Right;
			}

			return IterNode;
		} else {
			IterNode = TheNode->Parent;
			while (TheNode == IterNode->Left) {
				if (IterNode == this->RootNode) {
					return this->NullNode;
				}

				TheNode = IterNode;
				IterNode = IterNode->Parent;
			}

			return IterNode;
		}
	}

	template<typename KeyType, typename ValueType>
	inline typename RedBlackTree<KeyType, ValueType>::Node* RedBlackTree<KeyType, ValueType>::PrevNode(Node* const TheNode)
	{
		return const_cast<Node*>(const_cast<RedBlackTree const*>(this)->PrevNode(TheNode));
	}

	template<typename KeyType, typename ValueType>
	inline void RedBlackTree<KeyType, ValueType>::RotateLeft(Node* const TheNode)
	{
		Node* ChildNode = TheNode->Right;
		TheNode->Right = ChildNode->Left;
		if (ChildNode->Left != this->NullNode) {
			ChildNode->Left->Parent = TheNode;
		}

		ChildNode->Parent = TheNode->Parent;
		if (TheNode == TheNode->Parent->Left) {
			TheNode->parent->Left = ChildNode;
		} else {
			TheNode->parent->Right = ChildNode;
		}

		ChildNode->Left = TheNode;
		TheNode->Parent = ChildNode;
	}

	template<typename KeyType, typename ValueType>
	inline void RedBlackTree<KeyType, ValueType>::RotateRight(Node* const TheNode)
	{
		Node* ChildNode = TheNode->Left;
		TheNode->Left = ChildNode->Right;
		if (ChildNode->Right != this->NullNode) {
			ChildNode->Right->Parent = TheNode;
		}

		ChildNode->Parent = TheNode->Parent;
		if (TheNode == TheNode->Parent->Left) {
			TheNode->Parent->Left = ChildNode;
		} else {
			TheNode->Parent->Right = ChildNode;
		}

		ChildNode->Right = TheNode;
		TheNode->Parent = ChildNode;
	}

	template<typename KeyType, typename ValueType>
	inline void RedBlackTree<KeyType, ValueType>::Insert(ThisPairType* const Pair)
	{
		GD_DEBUG_ASSERT(Pair != nullptr, "Null pointer value specified.");
		this->Length += 1;

		Node* NewNode = nullptr;
		Node* ParentNode = this->RootNode;
		Node* CurrentNode = this->RootNode->Left;

		this->CreateNode(NewNode);
		NewNode->Value = Pair;
		NewNode->Color = NodeColor::Red;

		while (CurrentNode != this->NullNode) {
			ParentNode = CurrentNode;
			if (CurrentNode->Data->Key > NewNode->Data->Key) {
				CurrentNode = CurrentNode->Left;
			} else {
				CurrentNode = CurrentNode->Right;
			}
		}
		if ((ParentNode == this->RootNode) || (CurrentNode->Data->Key > NewNode->Data->Key)) {
			ParentNode->Left = NewNode;
		} else {
			ParentNode->Right = NewNode;
		}

		CurrentNode = NewNode;
		CurrentNode->Parent = ParentNode;
		while (CurrentNode->Parent->Color == NodeColor::Red) {
			if (CurrentNode->Parent == CurrentNode->Parent->Parent->Left) {
				ParentNode = CurrentNode->Parent->Parent->Right;
				if (ParentNode->Color == NodeColor::Red) {
					ParentNode->Color = NodeColor::Black;

					CurrentNode->Parent->Color = NodeColor::Black;
					CurrentNode->Parent->Parent->Color = NodeColor::Red;
					CurrentNode = CurrentNode->Parent->Parent;
				} else {
					if (CurrentNode == CurrentNode->Parent->Right) {
						CurrentNode = CurrentNode->Parent;
						this->RotateLeft(CurrentNode);
					}

					CurrentNode->Parent->Color = NodeColor::Black;
					CurrentNode->Parent->Parent->Color = NodeColor::Red;
					this->RotateRight(CurrentNode->Parent->Parent);
				}
			} else {
				ParentNode = CurrentNode->Parent->Parent->Left;
				if (ParentNode->Color == NodeColor::Red) {
					ParentNode->Color = NodeColor::Black;

					CurrentNode->Parent->Color = NodeColor::Black;
					CurrentNode->Parent->Parent->Color = NodeColor::Red;
					CurrentNode = CurrentNode->Parent->Parent;
				} else {
					if (CurrentNode == CurrentNode->Parent->Left) {
						CurrentNode = CurrentNode->Parent;
						this->RotateRight(CurrentNode);
					}

					CurrentNode->Parent->Color = NodeColor::Black;
					CurrentNode->Parent->Parent->Color = NodeColor::Red;
					this->RotateLeft(CurrentNode->Parent->Parent);
				}
			}
		}

		this->RootNode->Left->Color = NodeColor::Black;
	}

	template<typename KeyType, typename ValueType>
	inline void RedBlackTree<KeyType, ValueType>::Repair(Node* const TheNode)
	{
		Node* const FirstNode = this->RootNode->Left;
		Node* SiblingNode = nullptr;

		while ((TheNode->Color == NodeColor::Black) && (FirstNode != TheNode)) {
			if (TheNode == TheNode->Parent->Left) {
				SiblingNode = TheNode->Parent->Right;
				if (SiblingNode->Color == NodeColor::Red) {
					SiblingNode->Color = NodeColor::Black;
					TheNode->Parent->Color = NodeColor::Red;

					this->RotateLeft(TheNode->Parent);
					SiblingNode = TheNode->Parent->Right;
				}

				if ((SiblingNode->Right->Color == NodeColor::Black) && (SiblingNode->Left->Color == NodeColor::Black)) {
					SiblingNode->Color = NodeColor::Red;
					TheNode = TheNode->Parent;
				} else {
					if (SiblingNode->Right->Color == NodeColor::Black) {
						SiblingNode->Left->Color = NodeColor::Black;
						SiblingNode->Color = NodeColor::Red;

						this->RotateRight(SiblingNode);
						SiblingNode = TheNode->Parent->Right;
					}

					SiblingNode->Color = TheNode->Parent->Color;
					SiblingNode->Right->Color = NodeColor::Black;
					TheNode->Parent->Color = NodeColor::Black;

					this->RotateLeft(TheNode->Parent);
					TheNode = FirstNode;
				}
			} else {
				SiblingNode = TheNode->Parent->Left;
				if (SiblingNode->Color == NodeColor::Red) {
					SiblingNode->Color = NodeColor::Black;
					TheNode->Parent->Color = NodeColor::Red;

					this->RotateRight(TheNode->Parent);
					SiblingNode = TheNode->Parent->Left;
				}

				if ((SiblingNode->Right->Color == NodeColor::Black) && (SiblingNode->Left->Color == NodeColor::Black)) {
					SiblingNode->Color = NodeColor::Red;
					TheNode = TheNode->Parent;
				} else {
					if (SiblingNode->Left->Color == NodeColor::Black) {
						SiblingNode->Right->Color = NodeColor::Black;
						SiblingNode->Color = NodeColor::Red;

						this->RotateLeft(SiblingNode);
						SiblingNode = TheNode->Parent->Left;
					}

					SiblingNode->Color = TheNode->Parent->Color;
					TheNode->Parent->Color = NodeColor::Black;
					SiblingNode->Left->Color = NodeColor::Black;

					this->RotateRight(TheNode->Parent);
					TheNode = FirstNode;
				}
			}
		}

		TheNode->Color = NodeColor::Black;
	}

	template<typename KeyType, typename ValueType>
	inline void RedBlackTree<KeyType, ValueType>::Delete(Node* const TheNode)
	{
		this->Length -= 1;

		Node* ChildNode = (TheNode->Left == this->NullNode) || (TheNode->Right == this->NullNode) ? TheNode : this->NextNode(TheNode);
		Node* SiblingNode = ChildNode->Left == this->NullNode ? ChildNode->Right : ChildNode->Left;

		SiblingNode->Parent = ChildNode->Parent;
		if (this->RootNode == SiblingNode->Parent) {
			this->RootNode->Left = SiblingNode;
		} else {
			if (ChildNode == ChildNode->Parent->Left) {
				ChildNode->Parent->Left = SiblingNode;
			} else {
				ChildNode->Parent->Right = SiblingNode;
			}
		}

		if (ChildNode != TheNode) {
			if (ChildNode->Color == NodeColor::Black) {
				this->Repair(SiblingNode)
			}

			ChildNode->Left = TheNode->Left;
			ChildNode->Right = TheNode->Right;
			ChildNode->Parent = TheNode->Parent;
			ChildNode->Color = TheNode->Color;
			TheNode->Left->Parent = ChildNode;
			TheNode->Right->Parent = ChildNode;
			if (TheNode == TheNode->Parent->Left) {
				TheNode->Parent->Left = ChildNode;
			} else {
				TheNode->Parent->Right = ChildNode;
			}

			delete TheNode->Data;
			delete TheNode;
		} else {
			if (ChildNode->Color == NodeColor::Black) {
				this->Repair(SiblingNode)
			}

			delete ChildNode->Data;
			delete ChildNode;
		}
	}

	template<typename KeyType, typename ValueType>
	inline typename RedBlackTree<KeyType, ValueType>::Node const* RedBlackTree<KeyType, ValueType>::Query(KeyTypeConstRef Key) const
	{
		Node* TheNode = this->RootNode->Left;
		while (TheNode == this->NullNode) {
			if (TheNode->Data->Key == Key) {
				return TheNode;
			} else if (TheNode->Data->Key > Key) {
				TheNode = TheNode->Left;
			} else {
				TheNode = TheNode->Right;
			}
		}

		return TheNode;
	}

	template<typename KeyType, typename ValueType>
	inline typename RedBlackTree<KeyType, ValueType>::Node* RedBlackTree<KeyType, ValueType>::Query(KeyTypeConstRef Key)
	{
		return const_cast<Node*>(const_cast<RedBlackTree const*>(this)->Query(Key));
	}

	/// ------------------------------------------------------------------------------------------
	/// RedBlackTree::TreeBaseIterator class.
	/// ------------------------------------------------------------------------------------------

	template<typename KeyType, typename ValueType> template<typename TreeType, typename TreePairType>
	GDINL RedBlackTree<KeyType, ValueType>::TreeBaseIterator<TreeType, TreePairType>::TreeBaseIterator(TreeType& Container, TreeTypeNode* const StartNode)
		: Container(Container), IterNode(StartNode)
	{
	}

	template<typename KeyType, typename ValueType> template<typename TreeType, typename TreePairType>
	GDINL RedBlackTree<KeyType, ValueType>::TreeBaseIterator<TreeType, TreePairType>::TreeBaseIterator(TreeBaseIterator const& Other)
		: Container(Other.Container), IterNode(Other.RootNode)
	{
	}

	template<typename KeyType, typename ValueType> template<typename TreeType, typename TreePairType>
	GDINL typename RedBlackTree<KeyType, ValueType>::TreeBaseIterator<TreeType, TreePairType>& RedBlackTree<KeyType, ValueType>::TreeBaseIterator<TreeType, TreePairType>::operator= (TreeBaseIterator const& Other)
	{
		GD_DEBUG_ASSERT(&this->Container == &Other.Container, "Assigning two iterators pointing to different containers.");
		this->IterNode = Other.IterNode;
	}

	template<typename KeyType, typename ValueType> template<typename TreeType, typename TreePairType>
	GDINL typename RedBlackTree<KeyType, ValueType>::TreeBaseIterator<TreeType, TreePairType>& RedBlackTree<KeyType, ValueType>::TreeBaseIterator<TreeType, TreePairType>::operator++()
	{
		this->IterNode = this->Container->NextNode(this->IterNode);
		return *this;
	}

	template<typename KeyType, typename ValueType> template<typename TreeType, typename TreePairType>
	GDINL typename RedBlackTree<KeyType, ValueType>::TreeBaseIterator<TreeType, TreePairType> RedBlackTree<KeyType, ValueType>::TreeBaseIterator<TreeType, TreePairType>::operator++(int const)
	{
		TreeBaseIterator Copy(*this);
		this->IterNode = this->Container->NextNode(this->IterNode);
		return Copy;
	}

	template<typename KeyType, typename ValueType> template<typename TreeType, typename TreePairType>
	GDINL typename RedBlackTree<KeyType, ValueType>::TreeBaseIterator<TreeType, TreePairType>& RedBlackTree<KeyType, ValueType>::TreeBaseIterator<TreeType, TreePairType>::operator--()
	{
		this->IterNode = this->Container->PrevNode(this->IterNode);
		return *this;
	}

	template<typename KeyType, typename ValueType> template<typename TreeType, typename TreePairType>
	GDINL typename RedBlackTree<KeyType, ValueType>::TreeBaseIterator<TreeType, TreePairType> RedBlackTree<KeyType, ValueType>::TreeBaseIterator<TreeType, TreePairType>::operator--(int const)
	{
		TreeBaseIterator Copy(*this);
		this->IterNode = this->Container->PrevNode(this->IterNode);
		return Copy;
	}

	template<typename KeyType, typename ValueType> template<typename TreeType, typename TreePairType>
	GDINL bool RedBlackTree<KeyType, ValueType>::TreeBaseIterator<TreeType, TreePairType>::operator== (TreeBaseIterator const& Other) const
	{
		return (&this->Container == &Other.Container) && (this->IterNode == Other.IterNode);
	}

	template<typename KeyType, typename ValueType> template<typename TreeType, typename TreePairType>
	GDINL bool RedBlackTree<KeyType, ValueType>::TreeBaseIterator<TreeType, TreePairType>::operator!= (TreeBaseIterator const& Other) const
	{
		return (&this->Container != &Other.Container) || (this->IterNode != Other.IterNode);
	}

	template<typename KeyType, typename ValueType> template<typename TreeType, typename TreePairType>
	GDINL TreePairType* RedBlackTree<KeyType, ValueType>::TreeBaseIterator<TreeType, TreePairType>::operator-> () const
	{
		return this->IterNode->Data;
	}

	template<typename KeyType, typename ValueType> template<typename TreeType, typename TreePairType>
	GDINL TreePairType& RedBlackTree<KeyType, ValueType>::TreeBaseIterator<TreeType, TreePairType>::operator* () const
	{
		return *this->IterNode->Data;
	}

	/// ------------------------------------------------------------------------------------------
	/// Red-Black tree iteration.
	/// ------------------------------------------------------------------------------------------

	template<typename KeyType, typename ValueType>
	GDINL typename RedBlackTree<KeyType, ValueType>::Iterator RedBlackTree<KeyType, ValueType>::Begin()
	{
		return Iterator(*this, this->RootNode);
	}

	template<typename KeyType, typename ValueType>
	GDINL typename RedBlackTree<KeyType, ValueType>::ConstIterator RedBlackTree<KeyType, ValueType>::Begin() const
	{
		return ConstIterator(*this, this->RootNode);
	}

	template<typename KeyType, typename ValueType>
	GDINL typename RedBlackTree<KeyType, ValueType>::Iterator RedBlackTree<KeyType, ValueType>::End()
	{
		return Iterator(*this, this->NullNode);
	}

	template<typename KeyType, typename ValueType>
	GDINL typename RedBlackTree<KeyType, ValueType>::ConstIterator RedBlackTree<KeyType, ValueType>::End() const
	{
		return ConstIterator(*this, this->NullNode);
	}

	/// ------------------------------------------------------------------------------------------
	/// Dynamic size management.
	/// ------------------------------------------------------------------------------------------

	template<typename KeyType, typename ValueType>
	GDINL size_t RedBlackTree<KeyType, ValueType>::GetLength() const
	{
		return this->Length;
	}

	template<typename KeyType, typename ValueType>
	GDINL bool RedBlackTree<KeyType, ValueType>::IsEmpty() const
	{
		return (this->Length == 0);
	}

	template<typename KeyType, typename ValueType>
	inline void RedBlackTree<KeyType, ValueType>::Clear()
	{
		delete this->RootNode->Left->Data;
		this->DestroyNode(this->RootNode->Left);
		this->RootNode->Left = nullptr;

		delete this->RootNode->Right;
		this->DestroyNode(this->RootNode->Right);
		this->RootNode->Right = nullptr;
	}

	/// ------------------------------------------------------------------------------------------
	/// @name Elements access.
	/// ------------------------------------------------------------------------------------------

	template<typename KeyType, typename ValueType>
	GDINL typename RedBlackTree<KeyType, ValueType>::ConstIterator RedBlackTree<KeyType, ValueType>::FindIteratorWithKey(KeyTypeConstRef Key) const
	{
		return ConstIterator(*this, this->Query(Key));
	}

	template<typename KeyType, typename ValueType>
	GDINL typename RedBlackTree<KeyType, ValueType>::Iterator RedBlackTree<KeyType, ValueType>::FindIteratorWithKey(KeyTypeConstRef Key)
	{
		return ConstIterator(*this, this->Query(Key));
	}

	template<typename KeyType, typename ValueType>
	GDINL ValueType const& RedBlackTree<KeyType, ValueType>::GetValueWithKey(KeyTypeConstRef Key) const
	{
		ConstIterator FoundIter = this->FindIteratorWithKey(Key);
		GD_ASSERT(FoundIter != this->End(), "Iterator with specified key does not exist.");

		return FoundIter->Value;
	}

	template<typename KeyType, typename ValueType>
	GDINL ValueType& RedBlackTree<KeyType, ValueType>::GetValueWithKey(KeyTypeConstRef Key)
	{
		return const_cast<ValueType&>(const_cast<RedBlackTree const*>(this)->GetValueWithKey(Key));
	}

	template<typename KeyType, typename ValueType>
	GDINL void RedBlackTree<KeyType, ValueType>::SetValueWithKey(KeyTypeConstRef Key, ValueType&& Value)
	{
		this->GetValueWithKey(Key)->SetValue(Forward<ValueType>(Value));
	}

	template<typename KeyType, typename ValueType>
	GDINL void RedBlackTree<KeyType, ValueType>::SetValueWithKey(KeyTypeConstRef Key, ValueTypeConstRef Value)
	{
		this->GetValueWithKey(Key)->SetValue(Value);
	}

	template<typename KeyType, typename ValueType>
	GDINL void RedBlackTree<KeyType, ValueType>::InsertKeyValue(KeyType&& Key, ValueType&& Value)
	{
		this->Insert(new ThisPairType(Forward<KeyType>(Key), Forward<ValueType>(Value)));
	}

	template<typename KeyType, typename ValueType>
	GDINL void RedBlackTree<KeyType, ValueType>::InsertKeyValue(KeyTypeConstRef Key, ValueTypeConstRef Value)
	{
		this->Insert(new ThisPairType(Key, Value));
	}

	template<typename KeyType, typename ValueType>
	GDINL void RedBlackTree<KeyType, ValueType>::RemoveElementAt(KeyTypeConstRef Key)
	{
		Iterator FoundIter = this->FindIteratorWithKey(Key);
		GD_ASSERT(FoundIter != this->End(), "Iterator with specified key does not exist.");
		
		this->Delete(FoundIter->IterNode);
	}

	/// ------------------------------------------------------------------------------------------
	/// Operators.
	/// ------------------------------------------------------------------------------------------

	template<typename KeyType, typename ValueType>
	inline RedBlackTree<KeyType, ValueType>& RedBlackTree<KeyType, ValueType>::operator= (RedBlackTree&& Other)
	{
		this->Clear();
		this->RootNode = Other.RootNode;
		this->NullNode = Other.NullNode;
		this->Length = Other.Length;

		Other.RootNode = nullptr;
		Other.NullNode = nullptr;
		Other.Length = 0;
	}

	template<typename KeyType, typename ValueType>
	inline RedBlackTree<KeyType, ValueType>& RedBlackTree<KeyType, ValueType>::operator= (RedBlackTree const& Other)
	{
		this->Clear();
		this->CreateNode(this->NullNode);
		this->CopyNode(Other.RootNode, Other.NullNode, this->RootNode);
		this->Length = Other.Length;
		this->RootNode->ParentNode = this->NullNode;
	}

	template<typename KeyType, typename ValueType>
	GDINL ValueType const& RedBlackTree<KeyType, ValueType>::operator[](KeyTypeConstRef Key) const
	{
		return this->GetValueWithKey(Key);
	}

	template<typename KeyType, typename ValueType>
	GDINL ValueType& RedBlackTree<KeyType, ValueType>::operator[](KeyTypeConstRef Key)
	{
		return this->GetValueWithKey(Key);
	}

GD_NAMESPACE_END
