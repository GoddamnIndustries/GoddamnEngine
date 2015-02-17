/// ==========================================================================================
/// LockFreeList.h - Lock-free list class implementation.
/// Copyright (C) Goddamn Industries 2011 - 2015. All Rights Reserved.
/// ==========================================================================================

#if 0
#include <GoddamnEngine/Core/Containers/LockFreeList/LockFreeList.h>

#if (defined(GD_PLATFORM_WINDOWS))

GD_NAMESPACE_BEGIN

GD_NAMESPACE_END

#else	// if (defined(GD_PLATFORM_WINDOWS))

/// @todo Provide generic implementation of the LockFreeList.
#	error LockFreeList is not implemented for this platform.

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <windows.h>
#include <stdlib.h>
#include <queue>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <assert.h>
#include <time.h>
#include <list>
using namespace std;
#define InterLockedAlign 32
const int K = 15;
const int R = 1000;
const unsigned int THREADCOUNT = 20;
int iterations = 1000;
__declspec(align(InterLockedAlign)) struct Node
{
	int data;
	__declspec(align(InterLockedAlign))	Node *rlink;
	__declspec(align(InterLockedAlign)) Node *llink;
};
//Following is the structure for Insert operation
struct Insert
{
	//Following is the structure for arguments passed to Insert
	struct Arguments
	{
		Node *p;//node to be inserted
		Node *x;//p is inserted next to x
	}args;
	struct LocalVariables
	{
		Node *x_rlink_llink;//This announces the value of x->rlink->llink which needs to be set to p.
		Node **x_rlink_llink_address;//This announces the address of x->rlink->llink which is used as destination in InterlockedCompareExchange.
		Node *x_rlink;//This announces the value of x->rlink which needs to be set to p.
		Node **x_rlink_address;//This announces the address of x->rlink which is used as destination in InterlockedCompareExchange.
	}lv;
};
//Following is the structure for Delete operation
struct Delete
{
	//Following is the structure for arguments passed to Delete
	struct Arguments
	{
		Node *x;//node to be deleted.
	}args;
	struct LocalVariables
	{
		//Following variables announce the values and addresses of pointers found in nodes around x (including x as well).
		Node *x_llink;
		Node **x_llink_address;
		Node *x_rlink;
		Node **x_rlink_address;
		Node *x_llink_rlink;
		Node **x_llink_rlink_address;
		Node *x_rlink_llink;
		Node **x_rlink_llink_address;
		Node *x_llink_llink;
		Node **x_llink_llink_address;
		Node *x_rlink_rlink;
		Node **x_rlink_rlink_address;
		Node *x_llink_llink_rlink;
		Node **x_llink_llink_rlink_address;
		Node *x_rlink_rlink_llink;
		Node **x_rlink_rlink_llink_address;

		Node *replacement_x_llink;//This points to the replacement for the node left to x.
		Node *replacement_x_rlink;//This points to the replacement for the node right to x.
	}lv;
};
enum OperationName
{
	NONE, INSERT, DELET
};
//Following structure contains the operations defined earlier.
struct AnnounceOp
{
	OperationName opName;
	union
	{
		struct Insert insert;
		struct Delete del;
	};
};

struct HPRecType
{
	Node *HP[K];
	HPRecType *Next;
};
__declspec(align(InterLockedAlign)) struct LinkedList
{
	volatile AnnounceOp *announce;//current announcement
	Node *first;//first node
	Node *end;//end node	
	list<Node*> rlist[THREADCOUNT];
	int rcount[THREADCOUNT];
	HPRecType *HeadHPRec;
};

bool Lookup(list<Node*>&list, Node *node)
{
	for (std::list<Node*>::iterator iter = list.begin(); iter != list.end(); iter++)
	{
		if (*iter == node)
		{
			return true;
		}
	}
	return  false;
}

void Scan(HPRecType *head, list<Node*>&rlist, int &rcount)
{
	list<Node*>plist;
	HPRecType *hprec = head;
	while (hprec != NULL)
	{
		for (int i = 0; i < K; i++)
		{
			Node *hptr = hprec->HP[i];
			if (hptr != NULL)
			{
				plist.push_back(hptr);
			}
		}
		hprec = hprec->Next;
	}

	list<Node*> tmplist = rlist;
	rlist.clear();
	rcount = 0;

	Node *node = tmplist.back();
	tmplist.pop_back();
	while (node != NULL)
	{
		if (Lookup(plist, node))
		{
			rlist.push_back(node);
			rcount++;
		} else
		{
			_aligned_free(node);
		}
		if (tmplist.empty() == false)
		{
			node = tmplist.back();
			tmplist.pop_back();
		} else
		{
			node = NULL;
		}
	}
	plist.clear();
}
void RetireNode(HPRecType*HeadHPRec, Node *node, list<Node*>&rlist, int &rcount)
{
	rlist.push_back(node);
	rcount++;
	if (rcount >= THREADCOUNT*K)
	{
		Scan(HeadHPRec, rlist, rcount);
	}
}
void Initialize(LinkedList *l)
{
	//current announcement is that no operation is in progress	
	l->announce = (AnnounceOp*) _aligned_malloc(sizeof(struct AnnounceOp), InterLockedAlign);//new AnnounceOp;
	assert(l->announce);
	l->announce->opName = NONE;

	//create 4 node doubly linked list
	l->first = (Node *) _aligned_malloc(sizeof(struct Node), InterLockedAlign);//new Node;
	assert(l->first);
	l->end = (Node *) _aligned_malloc(sizeof(struct Node), InterLockedAlign);
	assert(l->end);
	l->first->rlink = (Node *) _aligned_malloc(sizeof(struct Node), InterLockedAlign);
	assert(l->first->rlink);
	l->first->rlink->rlink = (Node *) _aligned_malloc(sizeof(struct Node), InterLockedAlign);
	assert(l->first->rlink->rlink);
	l->first->llink = 0;
	l->first->rlink->llink = l->first;
	l->first->rlink->rlink->rlink = l->end;
	l->first->rlink->rlink->llink = l->first->rlink;
	l->end->rlink = 0;
	l->end->llink = l->first->rlink->rlink;
	l->HeadHPRec = new HPRecType;
	HPRecType *currHPRec = l->HeadHPRec;
	for (int i = 0; i < THREADCOUNT; i++)
	{
		for (int i = 0; i < K; i++)
		{
			currHPRec->HP[i] = 0;
		}
		currHPRec->Next = new HPRecType;

		currHPRec = currHPRec->Next;
	}
	currHPRec->Next = 0;
}

void Free(LinkedList *l)
{
	Node *iter = l->first;
	while (iter)
	{
		Node *temp = iter;
		iter = iter->rlink;
		_aligned_free(temp);
	}
	for (int i = 0; i < THREADCOUNT; i++)
	{
		list<Node*>*list = &l->rlist[i];
		std::list<Node*>::iterator iter;
		for (iter = list->begin(); iter != list->end(); iter++)
		{
			_aligned_free(*iter);

		}
		list->clear();
	}
	HPRecType*currHPRec = l->HeadHPRec;
	for (int i = 0; i < THREADCOUNT; i++)
	{
		HPRecType *temp = currHPRec;
		currHPRec = currHPRec->Next;
		delete temp;
	}
	delete currHPRec;
	_aligned_free((void*) l->announce);
}
//Second part of insert
void InsertHelper(LinkedList *l, AnnounceOp *curAnnouncedOp, list<Node*>&rlist, int &rcount)
{
	//set x's right link to node p (newly created node)
	InterlockedCompareExchange(reinterpret_cast<volatile LONG *>(curAnnouncedOp->insert.lv.x_rlink_address), (LONG) curAnnouncedOp->insert.args.p, (LONG) curAnnouncedOp->insert.lv.x_rlink);
	//set the left pointer of node next to x to point to p
	InterlockedCompareExchange(reinterpret_cast<volatile LONG *>(curAnnouncedOp->insert.lv.x_rlink_llink_address), (LONG) curAnnouncedOp->insert.args.p, (LONG) curAnnouncedOp->insert.lv.x_rlink_llink);
	//To announce that insert operation is complete.
	AnnounceOp *nextAnnounceOp = (AnnounceOp*) _aligned_malloc(sizeof(struct AnnounceOp), InterLockedAlign);
	assert(nextAnnounceOp);
	nextAnnounceOp->opName = NONE;
	void *v1 = reinterpret_cast<void *>(nextAnnounceOp);
	void *v2 = reinterpret_cast<void *>(curAnnouncedOp);
	if (InterlockedCompareExchange(reinterpret_cast<volatile LONG *>(&l->announce), (LONG) v1, (LONG) v2) == (LONG) v2)
	{
		RetireNode(l->HeadHPRec, (Node*) v2, rlist, rcount);
	} else
	{
		_aligned_free(nextAnnounceOp);
	}
}

//insert node p to the right of node x
int Insert(LinkedList *l, Node *p, Node *x, HPRecType *hPrec, list<Node*>&rlist, int &rcount)
{
	if (p == 0 || x == 0) return 0;
	AnnounceOp *curAnnouncedOp;
	AnnounceOp *nextAnnounceOp = (AnnounceOp*) _aligned_malloc(sizeof(struct AnnounceOp), InterLockedAlign);//To announce an insert operation.
	assert(nextAnnounceOp);
	while (1)
	{
		curAnnouncedOp = (AnnounceOp *) l->announce;
		hPrec->HP[2] = (Node*) curAnnouncedOp;
		if (l->announce != curAnnouncedOp) continue;
		if (curAnnouncedOp->opName == NONE)
		{
			try
			{
				if (l->first == x || l->end == x || l->end->llink == x)//insertion should not be after first or after end or after one node before end
				{
					_aligned_free(nextAnnounceOp);
					return 0;
				}
				p->llink = x;//set p's left as x
				p->rlink = x->rlink;//set p's right as x's right
				if (p->rlink == 0 || p->llink == 0)  goto label;
				nextAnnounceOp->opName = INSERT;//set INSERT as the next operation
				nextAnnounceOp->insert.args.p = p;
				nextAnnounceOp->insert.args.x = x;

				//announce the value of x->rlink which needs to be set to p
				nextAnnounceOp->insert.lv.x_rlink = x->rlink;
				hPrec->HP[3] = nextAnnounceOp->insert.lv.x_rlink;
				nextAnnounceOp->insert.lv.x_rlink_address = &x->rlink;//announce the address of x->rlink to be used as destination in InterlockedCompareExchange
				if (nextAnnounceOp->insert.lv.x_rlink != x->rlink) goto label;
				if (nextAnnounceOp->insert.lv.x_rlink == 0)  goto label;//node x is no more in the linked list



				//announce the value of x->rlink->llink which needs to be set to p
				nextAnnounceOp->insert.lv.x_rlink_llink = nextAnnounceOp->insert.lv.x_rlink->llink;
				hPrec->HP[4] = nextAnnounceOp->insert.lv.x_rlink_llink;
				nextAnnounceOp->insert.lv.x_rlink_llink_address = &nextAnnounceOp->insert.lv.x_rlink->llink;//announce the address of x->rlink->llink to be used as destination in InterlockedCompareExchange.
				if (nextAnnounceOp->insert.lv.x_rlink->llink != nextAnnounceOp->insert.lv.x_rlink_llink) goto label;
				if (nextAnnounceOp->insert.lv.x_rlink_llink == 0)  goto label;//node next to node x is unlinked

				hPrec->HP[5] = (Node*) nextAnnounceOp;

				//To announce the start of insert operation.
				void *v1 = reinterpret_cast<void *>(nextAnnounceOp);
				void *v2 = reinterpret_cast<void *>(curAnnouncedOp);
				void *res = (void *) InterlockedCompareExchange(reinterpret_cast<volatile LONG *>(&l->announce), (LONG) v1, (LONG) v2);
				if (res == v2)
				{
					RetireNode(l->HeadHPRec, (Node*) v2, rlist, rcount);

					InsertHelper(l, nextAnnounceOp, rlist, rcount);
					return 1;
				}
			} catch (...)
			{
				printf("Exception in Insert\n");
				_aligned_free(nextAnnounceOp);
				return 0;
			}
		} else if (curAnnouncedOp->opName == INSERT)
		{
			InsertHelper(l, curAnnouncedOp, rlist, rcount);
		}
	}
label:
	_aligned_free(nextAnnounceOp);
	return 0;
}
#endif	// if 0

#endif	// if (defined(GD_PLATFORM_WINDOWS))

