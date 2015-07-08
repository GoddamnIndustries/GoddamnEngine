// ==========================================================================================
// Copyright (C) Goddamn Industries 2015. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

//! @file GoddamnEngine/Core/Containers/RedBlackTree/RedBlackTree.cpp
//! Red Black this data structure implementation.
// ------------------------------------------------------------------------------------------
// Low-level Red-Black Tree algorithms.
// Adapted from the following code written by Emin Martinian:
// http://web.mit.edu/~emin/www.old/source_code/red_black_tree/index.html
//
// Copyright (c) 2001 Emin Martinian
//
// Redistribution and use in source and binary forms, with or without modification, are 
// permitted provided that neither the name of Emin Martinian nor the names of any 
// contributors are be used to endorse or promote products derived from this software without 
// specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS 
// OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE 
// COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE 
// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
// AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED 
// OF THE POSSIBILITY OF SUCH DAMAGE.
// ------------------------------------------------------------------------------------------

#include <GoddamnEngine/Core/Containers/RedBlackTree.h>

GD_NAMESPACE_BEGIN

	// ==========================================================================================
	// RedBlackTree class.
	// ==========================================================================================

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Constructor and destructor.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Initializes a new Red-Black Tree.
	GDAPI RedBlackTreeBase::RedBlackTreeBase()
	{
		this->Internal_CreateNode(this->NullNode);
		this->Internal_CreateNode(this->RootNode);
	}

	// ------------------------------------------------------------------------------------------
	//! Moves other Red-Black Tree here.
	//! @param Other Other tree to move here.
	GDAPI RedBlackTreeBase::RedBlackTreeBase(RedBlackTreeBase&& Other)
	{
		this->RootNode = Other.RootNode;
		this->NullNode = Other.NullNode;
		this->Length = Other.Length;

		Other.RootNode = nullptr;
		Other.NullNode = nullptr;
		Other.Length = 0;
	}

	// ------------------------------------------------------------------------------------------
	//! Deinitializes a Red-Black Tree and destroys all internal data.
	GDAPI RedBlackTreeBase::~RedBlackTreeBase()
	{
		this->Clear();
		GD_DELETE(this->RootNode);
		GD_DELETE(this->NullNode);
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Tree iteration.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Returns the first node of the tree or null.
	//! @returns The first node of the tree or null if no first node exists.
	GDAPI RedBlackTreeBaseNode const* RedBlackTreeBase::GetLastNode() const
	{
		RedBlackTreeBaseNode const* IterNode = this->GetFirstNode();
		while (IterNode->Right != this->NullNode)
		{
			IterNode = IterNode->Right;
		}

		return IterNode;
	}

	// ------------------------------------------------------------------------------------------
	//! Returns next node to specified one or null.
	//! @param Node Some node.
	//! @returns Next node to this one or null node if not exists.
	GDAPI RedBlackTreeBaseNode const* RedBlackTreeBase::GetNextNode(RedBlackTreeBaseNode const* X) const
	{
		RedBlackTreeBaseNode const* Y = nullptr;

		if (this->NullNode != (Y = X->Right))
		{	// assignment to Y is intentional
			while (Y->Left != this->NullNode)
			{	// returns the minimum of the Right subtree of X
				Y = Y->Left;
			}
			return(Y);
		}
		else
		{
			Y = X->Parent;
			while (X == Y->Right)
			{					// sentinel used instead of checking for this->NullNode
				X = Y;
				Y = Y->Parent;
			}
			if (Y == this->RootNode)
			{
				return(this->NullNode);
			}
			return(Y);
		}
	}

	// ------------------------------------------------------------------------------------------
	//! Returns previous node to specified one or null.
	//! @param Node Some node.
	//! @returns Previous node to this one or null node if not exists.
	GDAPI RedBlackTreeBaseNode const* RedBlackTreeBase::GetPrevNode(RedBlackTreeBaseNode const* X) const
	{
		RedBlackTreeBaseNode const* Y = nullptr;

		if (this->NullNode != (Y = X->Left))
		{		// assignment to Y is intentional
			while (Y->Right != this->NullNode)
			{	// returns the maximum of the Left subtree of X
				Y = Y->Right;
			}
			return(Y);
		}
		else
		{
			Y = X->Parent;
			while (X == Y->Left)
			{
				if (Y == this->RootNode)
				{
					return(this->NullNode);
				}
				X = Y;
				Y = Y->Parent;
			}
			return(Y);
		}
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Tree manipulation.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Internally creates a new node.
	//! @param NewNode Reference to new node.
	GDAPI void RedBlackTreeBase::Internal_CreateNode(RedBlackTreeBaseNode*& NewNode)
	{
		NewNode = reinterpret_cast<RedBlackTreeBaseNode*>(GD_MALLOC(sizeof(RedBlackTreeBaseNode) - sizeof(Byte)));
		new (NewNode) RedBlackTreeBaseNode();
		NewNode->Left   = this->NullNode;
		NewNode->Right  = this->NullNode;
		NewNode->Parent = this->NullNode;
		NewNode->IsRed  = false;
	}

	// ------------------------------------------------------------------------------------------
	//! Internally destroys a specified node and all is children.
	//! @param Node Node that is going to be destroyed.
	GDAPI void RedBlackTreeBase::Internal_DestroyNode(RedBlackTreeBaseNode* const Node)
	{
		if (Node != this->NullNode) 
		{
			this->Internal_DestroyNode(Node->Left);
			this->Internal_DestroyNode(Node->Right);
			this->DestroyNode(Node);
		}
	}

	// ------------------------------------------------------------------------------------------
	//! Performs a left rotation of the tree.
	//! @param Node The node to rotate on.
	GDAPI void RedBlackTreeBase::Internal_RotateLeft(RedBlackTreeBaseNode* X)
	{
		RedBlackTreeBaseNode* Y;

		//  I originally wrote this function to use the sentinel for
		//  this->NullNode to avoid checking for this->NullNode.  However this introduces a
		//  very subtle bug because sometimes this function modifies
		//  the Parent pointer of this->NullNode.  This can be a problem if a
		//  function which calls this->Internal_RotateLeft also uses the this->NullNode sentinel
		//  and expects the this->NullNode sentinel's Parent pointer to be unchanged
		//  after calling this function.  For example, when RBDeleteFixUP
		//  calls this->Internal_RotateLeft it expects the Parent pointer of this->NullNode to be
		//  unchanged.

		Y = X->Right;
		X->Right = Y->Left;

		if (Y->Left != this->NullNode)
		{
			Y->Left->Parent = X;
			// used to use sentinel here
			// and do an unconditional assignment instead of testing for this->NullNode
		}

		Y->Parent = X->Parent;

		// instead of checking if X->Parent is the this->RootNode as in the book, we
		// count on the this->RootNode sentinel to implicitly take care of this case
		if (X == X->Parent->Left)
		{
			X->Parent->Left = Y;
		}
		else
		{
			X->Parent->Right = Y;
		}
		Y->Left = X;
		X->Parent = Y;

		GD_DEBUG_ASSERT(!this->NullNode->IsRed, "NullNode not IsRed in Internal_RotateLeft");
	}

	// ------------------------------------------------------------------------------------------
	//! Performs a right rotation of the tree.
	//! @param Node The node to rotate on.
	GDAPI void RedBlackTreeBase::Internal_RotateRight(RedBlackTreeBaseNode* Y)
	{
		RedBlackTreeBaseNode* X;

		//  I originally wrote this function to use the sentinel for
		//  this->NullNode to avoid checking for this->NullNode.  However this introduces a
		//  very subtle bug because sometimes this function modifies
		//  the Parent pointer of this->NullNode.  This can be a problem if a
		//  function which calls this->Internal_RotateLeft also uses the this->NullNode sentinel
		//  and expects the this->NullNode sentinel's Parent pointer to be unchanged
		//  after calling this function.  For example, when RBDeleteFixUP
		//  calls this->Internal_RotateLeft it expects the Parent pointer of this->NullNode to be
		//  unchanged.

		X = Y->Left;
		Y->Left = X->Right;

		if (this->NullNode != X->Right)
		{
			X->Right->Parent = Y;
			// used to use sentinel here
			// and do an unconditional assignment instead of testing for this->NullNode
		}

		// instead of checking if X->Parent is the this->RootNode as in the book, we
		// count on the this->RootNode sentinel to implicitly take care of this case
		X->Parent = Y->Parent;
		if (Y == Y->Parent->Left)
		{
			Y->Parent->Left = X;
		}
		else
		{
			Y->Parent->Right = X;
		}
		X->Right = Y;
		Y->Parent = X;

		GD_DEBUG_ASSERT(!this->NullNode->IsRed, "NullNode not IsRed in Internal_RotateRight");
	}

	// ------------------------------------------------------------------------------------------
	//! Fixes the tree and restores it's red-black properties.
	//! @param Node The node to start fixing.
	GDAPI void RedBlackTreeBase::Internal_Repair(RedBlackTreeBaseNode* X)
	{
		RedBlackTreeBaseNode* W;

		while ((!X->IsRed) && (this->RootNode != X))
		{
			if (X == X->Parent->Left)
			{
				W = X->Parent->Right;
				if (W->IsRed)
				{
					W->IsRed = false;
					X->Parent->IsRed = true;
					this->Internal_RotateLeft(X->Parent);
					W = X->Parent->Right;
				}
				if ((!W->Right->IsRed) && (!W->Left->IsRed))
				{
					W->IsRed = true;
					X = X->Parent;
				}
				else
				{
					if (!W->Right->IsRed)
					{
						W->Left->IsRed = false;
						W->IsRed = true;
						this->Internal_RotateRight(W);
						W = X->Parent->Right;
					}
					W->IsRed = (X->Parent->IsRed);
					X->Parent->IsRed = false;
					W->Right->IsRed = false;
					this->Internal_RotateLeft(X->Parent);
					X = this->RootNode; // this is to exit while loop
				}
			}
			else
			{ // the code below is has Left and Right switched from above
				W = X->Parent->Left;
				if (W->IsRed)
				{
					W->IsRed = false;
					X->Parent->IsRed = true;
					this->Internal_RotateRight(X->Parent);
					W = X->Parent->Left;
				}
				if ((!W->Right->IsRed) && (!W->Left->IsRed))
				{
					W->IsRed = true;
					X = X->Parent;
				}
				else
				{
					if (!W->Left->IsRed)
					{
						W->Right->IsRed = false;
						W->IsRed = true;
						this->Internal_RotateLeft(W);
						W = X->Parent->Left;
					}
					W->IsRed = (X->Parent->IsRed);
					X->Parent->IsRed = false;
					W->Left->IsRed = false;
					this->Internal_RotateRight(X->Parent);
					X = this->RootNode; // this is to exit while loop
				}
			}
		}
		X->IsRed = false;

		GD_DEBUG_ASSERT(!this->NullNode->IsRed, "NullNode not black in Internal_Repair");
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Tree modification.
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ------------------------------------------------------------------------------------------
	//! Searches for node with specified key.
	//! @param Key Key we are looking for.
	GDAPI RedBlackTreeBaseNode const* RedBlackTreeBase::QueryNode(CHandle const Element) const
	{
		RedBlackTreeBaseNode* X = this->RootNode->Left;
		if (X == this->NullNode)
		{
			// return(0);
			return(X);
		}

		auto CompVal = this->CompareElements(X->GetElementUntyped(), Element);
		while (0 != CompVal)
		{//assignment
			if (1 == CompVal)
			{ // X->GetElementUntyped() > q
				X = X->Left;
			}
			else
			{
				X = X->Right;
			}
			if (X == this->NullNode)
			{
				// return(0);
				return(X);
			}
			CompVal = this->CompareElements(X->GetElementUntyped(), Element);
		}
		return(X);
	}

	// ------------------------------------------------------------------------------------------
	//! Performs an insertion of the new node with specified key and value.
	//! @param NewNode The node that would be inserted. Should be already created.
	GDAPI void RedBlackTreeBase::InsertNode(RedBlackTreeBaseNode* Z)
	{
		GD_DEBUG_ASSERT(Z != nullptr, "Null pointer node.");
		GD_DEBUG_ASSERT(this->QueryNode(Z->GetElementUntyped()) == this->GetNullNode()
			, "Node with specified element already exist in the tree.");
		this->Length += 1;

		RedBlackTreeBaseNode* Y;
		RedBlackTreeBaseNode* X;

		Z->Left = Z->Right = this->NullNode;
		Y = this->RootNode;
		X = this->RootNode->Left;
		while (X != this->NullNode)
		{
			Y = X;
			if (1 == this->CompareElements(X->GetElementUntyped(), Z->GetElementUntyped()))
			{ // X.key > Z.key
				X = X->Left;
			}
			else
			{ // X,key <= Z.key
				X = X->Right;
			}
		}
		Z->Parent = Y;
		if ((Y == this->RootNode) || (1 == this->CompareElements(Y->GetElementUntyped(), Z->GetElementUntyped())))
		{ // Y.key > Z.key
			Y->Left = Z;
		}
		else
		{
			Y->Right = Z;
		}

		GD_DEBUG_ASSERT(!this->NullNode->IsRed, "NullNode not IsRed in TreeInsertHelp");

		X = Z;
		X->IsRed = true;
		while (X->Parent->IsRed)
		{ // use sentinel instead of checking for this->RootNode
			if (X->Parent == X->Parent->Parent->Left)
			{
				Y = X->Parent->Parent->Right;
				if (Y->IsRed)
				{
					X->Parent->IsRed = false;
					Y->IsRed = false;
					X->Parent->Parent->IsRed = true;
					X = X->Parent->Parent;
				}
				else
				{
					if (X == X->Parent->Right)
					{
						X = X->Parent;
						this->Internal_RotateLeft(X);
					}
					X->Parent->IsRed = false;
					X->Parent->Parent->IsRed = true;
					this->Internal_RotateRight(X->Parent->Parent);
				}
			}
			else
			{ // case for X->Parent == X->Parent->Parent->Right
				Y = X->Parent->Parent->Left;
				if (Y->IsRed)
				{
					X->Parent->IsRed = false;
					Y->IsRed = false;
					X->Parent->Parent->IsRed = true;
					X = X->Parent->Parent;
				}
				else
				{
					if (X == X->Parent->Left)
					{
						X = X->Parent;
						this->Internal_RotateRight(X);
					}
					X->Parent->IsRed = false;
					X->Parent->Parent->IsRed = true;
					this->Internal_RotateLeft(X->Parent->Parent);
				}
			}
		}
		this->RootNode->Left->IsRed = false;

		GD_DEBUG_ASSERT(!this->NullNode->IsRed, "NullNode not IsRed in Insert");
		GD_DEBUG_ASSERT(!this->RootNode->IsRed, "RootNode not IsRed in Insert");
	}

	// ------------------------------------------------------------------------------------------
	//! Deletes the node from the tree.
	//! @param Node Node that would be deleted. Should be manually deleted.
	GDAPI void RedBlackTreeBase::RemoveNode(RedBlackTreeBaseNode* const Z)
	{
		RedBlackTreeBaseNode* Y;
		RedBlackTreeBaseNode* X;

		Y = ((Z->Left == this->NullNode) || (Z->Right == this->NullNode)) ? Z : this->GetNextNode(Z);
		X = (Y->Left == this->NullNode) ? Y->Right : Y->Left;
		if (this->RootNode == (X->Parent = Y->Parent))
		{ // assignment of Y->p to X->p is intentional
			this->RootNode->Left = X;
		}
		else
		{
			if (Y == Y->Parent->Left)
			{
				Y->Parent->Left = X;
			}
			else
			{
				Y->Parent->Right = X;
			}
		}
		if (Y != Z)
		{ // Y should not be this->NullNode in this case

			GD_DEBUG_ASSERT((Y != this->NullNode), "Y is this->NullNode in RBDelete\n");
			// Y is the node to splice out and X is its child

			if (!Y->IsRed)
			{
				this->Internal_Repair(X);
			}

			//this->DestroyKey(Z->GetElementUntyped());
			//this->DestroyInfo(Z->info);
			Y->Left = Z->Left;
			Y->Right = Z->Right;
			Y->Parent = Z->Parent;
			Y->IsRed = Z->IsRed;
			Z->Left->Parent = Z->Right->Parent = Y;
			if (Z == Z->Parent->Left)
			{
				Z->Parent->Left = Y;
			}
			else
			{
				Z->Parent->Right = Y;
			}
			//free(Z);
			this->DestroyNode(Z);
		}
		else
		{
			//this->DestroyKey(Y->GetElementUntyped());
			//this->DestroyInfo(Y->info);
			if (!(Y->IsRed))
			{
				this->Internal_Repair(X);
			}
			this->DestroyNode(Y);
			//free(Y);
		}

		GD_DEBUG_ASSERT(!this->NullNode->IsRed, "this->NullNode not black in RBDelete");
	}

	// ------------------------------------------------------------------------------------------
	//! Destroys all elements in container with memory deallocation.
	GDAPI void RedBlackTreeBase::Clear()
	{
		this->Internal_DestroyNode(this->RootNode->Left);
		this->Internal_DestroyNode(this->RootNode->Right);
		this->RootNode->Left = this->NullNode;
		this->RootNode->Right = this->NullNode;
	}

GD_NAMESPACE_END
