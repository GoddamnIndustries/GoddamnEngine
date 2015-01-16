/// ==========================================================================================
/// RedBlackTree.cpp - Red Black this data structure implementation.
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
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

	/// ------------------------------------------------------------------------------------------
	/// Constructor and destructor.
	/// ------------------------------------------------------------------------------------------

	GDAPI RedBlackTreeBase::RedBlackTreeBase()
	{
		this->InternalCreateNode(this->NullNode);
		this->InternalCreateNode(this->RootNode);
	}

	GDAPI RedBlackTreeBase::RedBlackTreeBase(RedBlackTreeBase&& Other)
	{
		this->RootNode = Other.RootNode;
		this->NullNode = Other.NullNode;
		this->Length = Other.Length;

		Other.RootNode = nullptr;
		Other.NullNode = nullptr;
		Other.Length = 0;
	}

	GDAPI RedBlackTreeBase::~RedBlackTreeBase()
	{
		this->InternalDestroyNode(this->RootNode->Left);
		delete this->RootNode;
		delete this->NullNode;
	}

	/// ------------------------------------------------------------------------------------------
	/// Tree iteration.
	/// ------------------------------------------------------------------------------------------

	GDAPI RedBlackTreeBaseNode const* RedBlackTreeBase::GetLastNode() const
	{
		RedBlackTreeBaseNode const* IterNode = this->GetFirstNode();
		while (IterNode->Right != RedBlackTreeBase::NullNode) {
			IterNode = IterNode->Right;
		}

		return IterNode;
	}

	GDAPI RedBlackTreeBaseNode const* RedBlackTreeBase::GetNextNode(RedBlackTreeBaseNode const* X) const
	{
		RedBlackTreeBaseNode const* Y;
		RedBlackTreeBaseNode const* NullNode = this->NullNode;
		RedBlackTreeBaseNode const* RootNode = this->RootNode;

		if (NullNode != (Y = X->Right)) { // assignment to Y is intentional
			while (Y->Left != NullNode) { // returns the minimum of the Right subtree of X
				Y = Y->Left;
			}
			return(Y);
		} else {
			Y = X->Parent;
			while (X == Y->Right) { // sentinel used instead of checking for NullNode
				X = Y;
				Y = Y->Parent;
			}
			if (Y == RootNode) return(NullNode);
			return(Y);
		}
	}

	GDAPI RedBlackTreeBaseNode const* RedBlackTreeBase::GetPrevNode(RedBlackTreeBaseNode const* X) const
	{
		RedBlackTreeBaseNode const* Y;
		RedBlackTreeBaseNode const* NullNode = this->NullNode;
		RedBlackTreeBaseNode const* RootNode = this->RootNode;

		if (NullNode != (Y = X->Left)) { // assignment to Y is intentional
			while (Y->Right != NullNode) { // returns the maximum of the Left subtree of X
				Y = Y->Right;
			}
			return(Y);
		} else {
			Y = X->Parent;
			while (X == Y->Left) {
				if (Y == RootNode) {
					return(NullNode);
				}
				X = Y;
				Y = Y->Parent;
			}
			return(Y);
		}
	}
	
	/// ------------------------------------------------------------------------------------------
	/// Tree manipulation.
	/// ------------------------------------------------------------------------------------------

	GDAPI void RedBlackTreeBase::InternalCreateNode(RedBlackTreeBaseNode*& NewNode)
	{
		NewNode = new RedBlackTreeBaseNode(nullptr);
		NewNode->Left = RedBlackTreeBase::NullNode;
		NewNode->Right = RedBlackTreeBase::NullNode;
		NewNode->Parent = RedBlackTreeBase::NullNode;
		NewNode->IsRed = false;
	}

	GDAPI void RedBlackTreeBase::InternalDestroyNode(RedBlackTreeBaseNode* const Node)
	{
		if (Node != RedBlackTreeBase::NullNode) {
			this->InternalDestroyNode(Node->Left);
			this->InternalDestroyNode(Node->Right);
			delete Node;
		}
	}

	GDAPI void RedBlackTreeBase::RotateLeft(RedBlackTreeBaseNode* X)
	{
		RedBlackTreeBaseNode* Y;
		RedBlackTreeBaseNode* NullNode = this->NullNode;

		//  I originally wrote this function to use the sentinel for
		//  NullNode to avoid checking for NullNode.  However this introduces a
		//  very subtle bug because sometimes this function modifies
		//  the Parent pointer of NullNode.  This can be a problem if a
		//  function which calls this->RotateLeft also uses the NullNode sentinel
		//  and expects the NullNode sentinel's Parent pointer to be unchanged
		//  after calling this function.  For example, when RBDeleteFixUP
		//  calls this->RotateLeft it expects the Parent pointer of NullNode to be
		//  unchanged.

		Y = X->Right;
		X->Right = Y->Left;

		if (Y->Left != NullNode) {
			Y->Left->Parent = X; 
			// used to use sentinel here
			// and do an unconditional assignment instead of testing for NullNode
		}

		Y->Parent = X->Parent;

		// instead of checking if X->Parent is the RootNode as in the book, we
		// count on the RootNode sentinel to implicitly take care of this case
		if (X == X->Parent->Left) {
			X->Parent->Left = Y;
		} else {
			X->Parent->Right = Y;
		}
		Y->Left = X;
		X->Parent = Y;

		GD_DEBUG_ASSERT(!this->NullNode->IsRed, "NullNode not IsRed in this->RotateLeft");
	}

	GDAPI void RedBlackTreeBase::RotateRight(RedBlackTreeBaseNode* Y)
	{
		RedBlackTreeBaseNode* X;
		RedBlackTreeBaseNode* NullNode = this->NullNode;

		//  I originally wrote this function to use the sentinel for
		//  NullNode to avoid checking for NullNode.  However this introduces a
		//  very subtle bug because sometimes this function modifies
		//  the Parent pointer of NullNode.  This can be a problem if a
		//  function which calls this->RotateLeft also uses the NullNode sentinel
		//  and expects the NullNode sentinel's Parent pointer to be unchanged
		//  after calling this function.  For example, when RBDeleteFixUP
		//  calls this->RotateLeft it expects the Parent pointer of NullNode to be
		//  unchanged.

		X = Y->Left;
		Y->Left = X->Right;

		if (NullNode != X->Right) {
			X->Right->Parent = Y; 
			// used to use sentinel here
			// and do an unconditional assignment instead of testing for NullNode
		}

		// instead of checking if X->Parent is the RootNode as in the book, we
		// count on the RootNode sentinel to implicitly take care of this case
		X->Parent = Y->Parent;
		if (Y == Y->Parent->Left) {
			Y->Parent->Left = X;
		} else {
			Y->Parent->Right = X;
		}
		X->Right = Y;
		Y->Parent = X;

		GD_DEBUG_ASSERT(!this->NullNode->IsRed, "NullNode not IsRed in this->RotateRight");
	}

	GDAPI void RedBlackTreeBase::Repair(RedBlackTreeBaseNode* X)
	{
		RedBlackTreeBaseNode* RootNode = this->RootNode->Left;
		RedBlackTreeBaseNode* w;

		while ((!X->IsRed) && (RootNode != X)) {
			if (X == X->Parent->Left) {
				w = X->Parent->Right;
				if (w->IsRed) {
					w->IsRed = false;
					X->Parent->IsRed = true;
					this->RotateLeft(X->Parent);
					w = X->Parent->Right;
				}
				if ((!w->Right->IsRed) && (!w->Left->IsRed)) {
					w->IsRed = true;
					X = X->Parent;
				} else {
					if (!w->Right->IsRed) {
						w->Left->IsRed = false;
						w->IsRed = true;
						this->RotateRight(w);
						w = X->Parent->Right;
					}
					w->IsRed = X->Parent->IsRed;
					X->Parent->IsRed = false;
					w->Right->IsRed = false;
					this->RotateLeft(X->Parent);
					X = RootNode; // this is to exit while loop
				}
			} else { // the code below is has Left and Right switched from above
				w = X->Parent->Left;
				if (w->IsRed) {
					w->IsRed = false;
					X->Parent->IsRed = true;
					this->RotateRight(X->Parent);
					w = X->Parent->Left;
				}
				if ((!w->Right->IsRed) && (!w->Left->IsRed)) {
					w->IsRed = true;
					X = X->Parent;
				} else {
					if (!w->Left->IsRed) {
						w->Right->IsRed = false;
						w->IsRed = true;
						this->RotateLeft(w);
						w = X->Parent->Left;
					}
					w->IsRed = X->Parent->IsRed;
					X->Parent->IsRed = false;
					w->Left->IsRed = false;
					this->RotateRight(X->Parent);
					X = RootNode; // this is to exit while loop
				}
			}
		}
		X->IsRed = false;

		GD_DEBUG_ASSERT(!this->NullNode->IsRed, "NullNode not black in this->Repair");
	}

	/// ------------------------------------------------------------------------------------------
	/// Tree modification.
	/// ------------------------------------------------------------------------------------------

	GDAPI void RedBlackTreeBase::Insert(RedBlackTreeBaseNode* NewNode)
	{
		GD_DEBUG_ASSERT(NewNode != nullptr, "Null pointer node.");
		this->Length += 1;

		auto const TreeInsertHelp = [this](RedBlackTreeBaseNode* Z) {
			//  This function should only be called by InsertRBTree (see above)
			RedBlackTreeBaseNode* X;
			RedBlackTreeBaseNode* Y;
			RedBlackTreeBaseNode* NullNode = this->NullNode;

			Z->Left = Z->Right = NullNode;
			Y = this->RootNode;
			X = this->RootNode->Left;
			while (X != NullNode) {
				Y = X;
				if (1 == this->CompareElements(X->Element, Z->Element)) { // X.key > Z.key
					X = X->Left;
				} else { // X,key <= Z.key
					X = X->Right;
				}
			}
			Z->Parent = Y;
			if ((Y == this->RootNode) || (1 == this->CompareElements(Y->Element, Z->Element))) { // Y.key > Z.key
				Y->Left = Z;
			} else {
				Y->Right = Z;
			}

			GD_DEBUG_ASSERT(!this->NullNode->IsRed, "NullNode not IsRed in TreeInsertHelp");
		};

		RedBlackTreeBaseNode* Y;
		RedBlackTreeBaseNode* X;

		// Node comes already preallocated..
		//X = (RedBlackTreeBaseNode*) SafeMalloc(sizeof(RedBlackTreeBaseNode));
		//X->Element = key;
		//X->info = info;

		TreeInsertHelp(NewNode);
		X = NewNode;
		X->IsRed = true;
		while (X->Parent->IsRed) { // use sentinel instead of checking for RootNode
			if (X->Parent == X->Parent->Parent->Left) {
				Y = X->Parent->Parent->Right;
				if (Y->IsRed) {
					X->Parent->IsRed = false;
					Y->IsRed = false;
					X->Parent->Parent->IsRed = true;
					X = X->Parent->Parent;
				} else {
					if (X == X->Parent->Right) {
						X = X->Parent;
						this->RotateLeft(X);
					}
					X->Parent->IsRed = false;
					X->Parent->Parent->IsRed = true;
					this->RotateRight(X->Parent->Parent);
				}
			} else { // case for X->Parent == X->Parent->Parent->Right
				Y = X->Parent->Parent->Left;
				if (Y->IsRed) {
					X->Parent->IsRed = false;
					Y->IsRed = false;
					X->Parent->Parent->IsRed = true;
					X = X->Parent->Parent;
				} else {
					if (X == X->Parent->Left) {
						X = X->Parent;
						this->RotateRight(X);
					}
					X->Parent->IsRed = false;
					X->Parent->Parent->IsRed = true;
					this->RotateLeft(X->Parent->Parent);
				}
			}
		}
		this->RootNode->Left->IsRed = false;

		GD_DEBUG_ASSERT(!this->NullNode->IsRed, "NullNode not IsRed in RBTreeInsert");
		GD_DEBUG_ASSERT(!this->RootNode->IsRed, "RootNode not IsRed in RBTreeInsert");
	}

	GDAPI void RedBlackTreeBase::Delete(RedBlackTreeBaseNode* const Z)
	{
		RedBlackTreeBaseNode* Y;
		RedBlackTreeBaseNode* X;
		RedBlackTreeBaseNode* NullNode = this->NullNode;
		RedBlackTreeBaseNode* RootNode = this->RootNode;

		Y = ((Z->Left == NullNode) || (Z->Right == NullNode)) ? Z : this->GetNextNode(Z);
		X = (Y->Left == NullNode) ? Y->Right : Y->Left;
		if (RootNode == (X->Parent = Y->Parent)) { // assignment of Y->p to X->p is intentional
			RootNode->Left = X;
		} else {
			if (Y == Y->Parent->Left) {
				Y->Parent->Left = X;
			} else {
				Y->Parent->Right = X;
			}
		}
		if (Y != Z) { // Y should not be NullNode in this case

			GD_DEBUG_ASSERT((Y != this->NullNode), "Y is NullNode in RBDelete\n");
			// Y is the node to splice out and X is its child

			if (!(Y->IsRed)) this->Repair(X);

			//this->DestroyKey(Z->Element);
			//this->DestroyInfo(Z->info);
			Y->Left = Z->Left;
			Y->Right = Z->Right;
			Y->Parent = Z->Parent;
			Y->IsRed = Z->IsRed;
			Z->Left->Parent = Z->Right->Parent = Y;
			if (Z == Z->Parent->Left) {
				Z->Parent->Left = Y;
			} else {
				Z->Parent->Right = Y;
			}
			//free(Z);
		} else {
			//this->DestroyKey(Y->Element);
			//this->DestroyInfo(Y->info);
			if (!(Y->IsRed)) this->Repair(X);
			//free(Y);
		}

		GD_DEBUG_ASSERT(!this->NullNode->IsRed, "NullNode not black in RBDelete");
	}

	GDAPI RedBlackTreeBaseNode const* RedBlackTreeBase::Query(chandle const Element) const
	{
		RedBlackTreeBaseNode* X = this->RootNode->Left;
		RedBlackTreeBaseNode* NullNode = this->NullNode;
		int CompVal;
		if (X == NullNode) {
			// return(0);
			return(X);
		}
		CompVal = this->CompareElements(X->Element, Element);
		while (0 != CompVal) {//assignemnt
			if (1 == CompVal) { // X->Element > q
				X = X->Left;
			} else {
				X = X->Right;
			}
			if (X == NullNode) {
				// return(0);
				return(X);
			}
			CompVal = this->CompareElements(X->Element, Element);
		}
		return(X);
	}

	GDINL void RedBlackTreeBase::Clear()
	{
		this->InternalDestroyNode(this->RootNode->Left);
		this->RootNode->Left = RedBlackTreeBase::NullNode;

		this->InternalDestroyNode(this->RootNode->Right);
		this->RootNode->Right = RedBlackTreeBase::NullNode;
	}

GD_NAMESPACE_END
