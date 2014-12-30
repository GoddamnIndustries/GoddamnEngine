/// ==========================================================================================
/// RedBlackTree.cpp - Red Black tree data structure implementation.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// ==========================================================================================

/// ------------------------------------------------------------------------------------------
/// Low-level Red-Black Tree algorithms.
/// Adapted from the following code written by Emin Martinian:
/// http://web.mit.edu/~emin/www.old/source_code/red_black_tree/index.html
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
/// ------------------------------------------------------------------------------------------

#include <GoddamnEngine/Core/Containers/RedBlackTree/RedBlackTree.h>

GD_NAMESPACE_BEGIN

	/// ==========================================================================================
	/// RedBlackTree class.
	/// ==========================================================================================

	RedBlackTreeBaseNode* RedBlackTree::NullNode = nullptr;

	/// ------------------------------------------------------------------------------------------
	/// Constructor and destructor.
	/// ------------------------------------------------------------------------------------------

	GDAPI RedBlackTree::RedBlackTree()
	{
		if (RedBlackTree::NullNode == nullptr) {
			this->InternalCreateNode(RedBlackTree::NullNode);
		}

		this->InternalCreateNode(this->RootNode);
	}

	GDAPI RedBlackTree::RedBlackTree(RedBlackTree&& Other)
	{
		this->RootNode = Other.RootNode;
		this->Length = Other.Length;

		Other.RootNode = nullptr;
		Other.Length = 0;
	}

	GDAPI RedBlackTree::~RedBlackTree()
	{
		this->InternalDestroyNode(this->RootNode->Left);
		delete this->RootNode;
	}

	/// ------------------------------------------------------------------------------------------
	/// Tree iteration.
	/// ------------------------------------------------------------------------------------------

	GDAPI RedBlackTreeBaseNode const* RedBlackTree::GetLastNode() const
	{
		RedBlackTreeBaseNode const* IterNode = this->GetFirstNode();
		while (IterNode->Right != RedBlackTree::NullNode) {
			IterNode = IterNode->Right;
		}

		return IterNode;
	}

	GDAPI RedBlackTreeBaseNode const* RedBlackTree::GetNextNode(RedBlackTreeBaseNode const* const TheNode) const
	{
		RedBlackTreeBaseNode const* CurrentNode = TheNode;
		RedBlackTreeBaseNode const* IterNode = CurrentNode->Right;
		if (IterNode != RedBlackTree::NullNode) {
			while (IterNode->Left != RedBlackTree::NullNode) {
				IterNode = IterNode->Left;
			}

			return IterNode;
		} else {
			IterNode = CurrentNode->Parent;
			while (CurrentNode == IterNode->Right) {
				CurrentNode = IterNode;
				IterNode = IterNode->Parent;
			}

			if (IterNode == this->RootNode) {
				return RedBlackTree::NullNode;
			} else {
				return IterNode;
			}
		}
	}

	GDAPI RedBlackTreeBaseNode const* RedBlackTree::GetPrevNode(RedBlackTreeBaseNode const* const TheNode) const
	{
		RedBlackTreeBaseNode const* CurrentNode = TheNode;
		RedBlackTreeBaseNode const* IterNode = CurrentNode->Left;
		if (RedBlackTree::NullNode != IterNode) {
			while (IterNode->Right != RedBlackTree::NullNode) {
				IterNode = IterNode->Right;
			}

			return IterNode;
		} else {
			IterNode = CurrentNode->Parent;
			while (CurrentNode == IterNode->Left) {
				if (IterNode == this->RootNode) {
					return RedBlackTree::NullNode;
				}

				CurrentNode = IterNode;
				IterNode = IterNode->Parent;
			}

			return IterNode;
		}
	}
	
	/// ------------------------------------------------------------------------------------------
	/// Tree manipulation.
	/// ------------------------------------------------------------------------------------------

	GDAPI void RedBlackTree::InternalCreateNode(RedBlackTreeBaseNode*& NewNode)
	{
		NewNode = new RedBlackTreeBaseNode(nullptr);
		NewNode->Left = RedBlackTree::NullNode;
		NewNode->Right = RedBlackTree::NullNode;
		NewNode->Parent = RedBlackTree::NullNode;
		NewNode->Color = RedBlackTreeNodeColor::Black;
	}

	GDAPI void RedBlackTree::InternalDestroyNode(RedBlackTreeBaseNode* const TheNode)
	{
		if (TheNode != RedBlackTree::NullNode) {
			this->InternalDestroyNode(TheNode->Left);
			this->InternalDestroyNode(TheNode->Right);
			delete TheNode;
		}
	}

	GDAPI void RedBlackTree::RotateLeft(RedBlackTreeBaseNode* const TheNode)
	{
		RedBlackTreeBaseNode* ChildNode = TheNode->Right;
		TheNode->Right = ChildNode->Left;
		if (ChildNode->Left != RedBlackTree::NullNode) {
			ChildNode->Left->Parent = TheNode;
		}

		ChildNode->Parent = TheNode->Parent;
		if (TheNode == TheNode->Parent->Left) {
			TheNode->Parent->Left = ChildNode;
		} else {
			TheNode->Parent->Right = ChildNode;
		}

		ChildNode->Left = TheNode;
		TheNode->Parent = ChildNode;
	}

	GDAPI void RedBlackTree::RotateRight(RedBlackTreeBaseNode* const TheNode)
	{
		RedBlackTreeBaseNode* ChildNode = TheNode->Left;
		TheNode->Left = ChildNode->Right;
		if (ChildNode->Right != RedBlackTree::NullNode) {
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

	GDAPI void RedBlackTree::Repair(RedBlackTreeBaseNode* const TheNode)
	{
		RedBlackTreeBaseNode* const FirstNode = this->RootNode->Left;
		RedBlackTreeBaseNode* SiblingNode = nullptr;
		RedBlackTreeBaseNode* CurrentNode = TheNode;

		while ((CurrentNode->Color == RedBlackTreeNodeColor::Black) && (FirstNode != CurrentNode)) {
			if (CurrentNode == CurrentNode->Parent->Left) {
				SiblingNode = CurrentNode->Parent->Right;
				if (SiblingNode->Color == RedBlackTreeNodeColor::Red) {
					SiblingNode->Color = RedBlackTreeNodeColor::Black;
					CurrentNode->Parent->Color = RedBlackTreeNodeColor::Red;

					this->RotateLeft(CurrentNode->Parent);
					SiblingNode = CurrentNode->Parent->Right;
				}

				if ((SiblingNode->Right->Color == RedBlackTreeNodeColor::Black) && (SiblingNode->Left->Color == RedBlackTreeNodeColor::Black)) {
					SiblingNode->Color = RedBlackTreeNodeColor::Red;
					CurrentNode = CurrentNode->Parent;
				} else {
					if (SiblingNode->Right->Color == RedBlackTreeNodeColor::Black) {
						SiblingNode->Left->Color = RedBlackTreeNodeColor::Black;
						SiblingNode->Color = RedBlackTreeNodeColor::Red;

						this->RotateRight(SiblingNode);
						SiblingNode = CurrentNode->Parent->Right;
					}

					SiblingNode->Color = CurrentNode->Parent->Color;
					SiblingNode->Right->Color = RedBlackTreeNodeColor::Black;
					CurrentNode->Parent->Color = RedBlackTreeNodeColor::Black;

					this->RotateLeft(CurrentNode->Parent);
					CurrentNode = FirstNode;
				}
			} else {
				SiblingNode = CurrentNode->Parent->Left;
				if (SiblingNode->Color == RedBlackTreeNodeColor::Red) {
					SiblingNode->Color = RedBlackTreeNodeColor::Black;
					CurrentNode->Parent->Color = RedBlackTreeNodeColor::Red;

					this->RotateRight(CurrentNode->Parent);
					SiblingNode = CurrentNode->Parent->Left;
				}

				if ((SiblingNode->Right->Color == RedBlackTreeNodeColor::Black) && (SiblingNode->Left->Color == RedBlackTreeNodeColor::Black)) {
					SiblingNode->Color = RedBlackTreeNodeColor::Red;
					CurrentNode = CurrentNode->Parent;
				} else {
					if (SiblingNode->Left->Color == RedBlackTreeNodeColor::Black) {
						SiblingNode->Right->Color = RedBlackTreeNodeColor::Black;
						SiblingNode->Color = RedBlackTreeNodeColor::Red;

						this->RotateLeft(SiblingNode);
						SiblingNode = CurrentNode->Parent->Left;
					}

					SiblingNode->Color = CurrentNode->Parent->Color;
					CurrentNode->Parent->Color = RedBlackTreeNodeColor::Black;
					SiblingNode->Left->Color = RedBlackTreeNodeColor::Black;

					this->RotateRight(CurrentNode->Parent);
					CurrentNode = FirstNode;
				}
			}
		}

		CurrentNode->Color = RedBlackTreeNodeColor::Black;
	}

	/// ------------------------------------------------------------------------------------------
	/// Tree modification.
	/// ------------------------------------------------------------------------------------------

	GDAPI void RedBlackTree::Insert(RedBlackTreeBaseNode* const NewNode)
	{
		GD_DEBUG_ASSERT(NewNode != nullptr, "Null pointer node.");
		this->Length += 1;

		RedBlackTreeBaseNode* ParentNode = this->RootNode;
		RedBlackTreeBaseNode* CurrentNode = this->RootNode->Left;

		while (CurrentNode != RedBlackTree::NullNode) {
			ParentNode = CurrentNode;
			if (this->CompareComparands(CurrentNode, NewNode)) {
				CurrentNode = CurrentNode->Left;
			} else {
				CurrentNode = CurrentNode->Right;
			}
		}
		if ((this->CompareComparands(CurrentNode->Comparand, NewNode->Comparand) > 0) || (ParentNode == this->RootNode)) {
			ParentNode->Left = NewNode;
		} else {
			ParentNode->Right = NewNode;
		}

		CurrentNode = NewNode;
		CurrentNode->Parent = ParentNode;
		CurrentNode->Color = RedBlackTreeNodeColor::Red;
		while (CurrentNode->Parent->Color == RedBlackTreeNodeColor::Red) {
			if (CurrentNode->Parent == CurrentNode->Parent->Parent->Left) {
				ParentNode = CurrentNode->Parent->Parent->Right;
				if (ParentNode->Color == RedBlackTreeNodeColor::Red) {
					ParentNode->Color = RedBlackTreeNodeColor::Black;

					CurrentNode->Parent->Color = RedBlackTreeNodeColor::Black;
					CurrentNode->Parent->Parent->Color = RedBlackTreeNodeColor::Red;
					CurrentNode = CurrentNode->Parent->Parent;
				} else {
					if (CurrentNode == CurrentNode->Parent->Right) {
						CurrentNode = CurrentNode->Parent;
						this->RotateLeft(CurrentNode);
					}

					CurrentNode->Parent->Color = RedBlackTreeNodeColor::Black;
					CurrentNode->Parent->Parent->Color = RedBlackTreeNodeColor::Red;
					this->RotateRight(CurrentNode->Parent->Parent);
				}
			} else {
				ParentNode = CurrentNode->Parent->Parent->Left;
				if (ParentNode->Color == RedBlackTreeNodeColor::Red) {
					ParentNode->Color = RedBlackTreeNodeColor::Black;

					CurrentNode->Parent->Color = RedBlackTreeNodeColor::Black;
					CurrentNode->Parent->Parent->Color = RedBlackTreeNodeColor::Red;
					CurrentNode = CurrentNode->Parent->Parent;
				} else {
					if (CurrentNode == CurrentNode->Parent->Left) {
						CurrentNode = CurrentNode->Parent;
						this->RotateRight(CurrentNode);
					}

					CurrentNode->Parent->Color = RedBlackTreeNodeColor::Black;
					CurrentNode->Parent->Parent->Color = RedBlackTreeNodeColor::Red;
					this->RotateLeft(CurrentNode->Parent->Parent);
				}
			}
		}

		this->RootNode->Left->Color = RedBlackTreeNodeColor::Black;
	}

	GDAPI void RedBlackTree::Delete(RedBlackTreeBaseNode* const TheNode)
	{
		this->Length -= 1;

		RedBlackTreeBaseNode* ChildNode = (TheNode->Left == RedBlackTree::NullNode) || (TheNode->Right == RedBlackTree::NullNode) ? TheNode : this->GetNextNode(TheNode);
		RedBlackTreeBaseNode* SiblingNode = ChildNode->Left == RedBlackTree::NullNode ? ChildNode->Right : ChildNode->Left;

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
			if (ChildNode->Color == RedBlackTreeNodeColor::Black) {
				this->Repair(SiblingNode);
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
		} else {
			if (TheNode->Color == RedBlackTreeNodeColor::Black) {
				this->Repair(SiblingNode);
			}
		}
	}

	GDAPI RedBlackTreeBaseNode const* RedBlackTree::Query(handle const Comparand) const
	{
		RedBlackTreeBaseNode* TheNode = this->RootNode->Left;
		while (TheNode == RedBlackTree::NullNode) {
			int const ComparingResult = this->CompareComparands(TheNode->Comparand, Comparand);
			if (ComparingResult == 0) {
				return TheNode;
			} else if (ComparingResult > 0) {
				TheNode = TheNode->Left;
			} else {
				TheNode = TheNode->Right;
			}
		}

		return TheNode;
	}

	GDINL void RedBlackTree::Clear()
	{
		this->InternalDestroyNode(this->RootNode->Left);
		this->RootNode->Left = RedBlackTree::NullNode;

		this->InternalDestroyNode(this->RootNode->Right);
		this->RootNode->Right = RedBlackTree::NullNode;
	}

GD_NAMESPACE_END
