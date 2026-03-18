#ifndef NW4HBM_UT_LINK_LIST_H
#define NW4HBM_UT_LINK_LIST_H

#include "homebuttonLib/nw4hbm/ut/inlines.h"
#include <types.h>


/* Range-for loop macros */

// Sort of like C++11's range for, except now it's a macro for nw4r iterators.
#define NW4HBM_RANGE_FOR(it_, list_)	\
	for (DECLTYPE((list_).GetBeginIter()) (it_) = (list_).GetBeginIter(); (it_) != (list_).GetEndIter(); ++(it_))

/* This macro specifically is for the for loops which declare an
 * Iterator currIt = it++; in the body, so that it does not get incremented
 * twice.
 */
#define NW4HBM_RANGE_FOR_NO_AUTO_INC(it_, list_)	\
	for (DECLTYPE((list_).GetBeginIter()) (it_) = (list_).GetBeginIter(); (it_) != (list_).GetEndIter();)

namespace nw4hbm { 
namespace ut {

namespace detail { 
class LinkListImpl; 
}

class LinkListNode : private NonCopyable {
public:
	LinkListNode()
	: mNext(nullptr)
	, mPrev(nullptr)
	{
	}

	LinkListNode *GetNext() const { return mNext; }

private:
	LinkListNode *mNext;	// _00
	LinkListNode *mPrev;	// _04
friend class detail::LinkListImpl;
}; 

namespace detail{
namespace dummy { 
class Iterator {	
}; 
} 

class LinkListImpl : private NonCopyable {
public:
	class Iterator : public dummy::Iterator {
	public:
		Iterator(LinkListNode *p)
			:mPointer(p)
			{
			}

		/* ~Iterator() = default; */

		Iterator const &operator ++()
		{
			mPointer = mPointer->GetNext();
			return *this;
		}

		LinkListNode *operator ->() const { return mPointer; }

		friend bool operator ==(Iterator it1, Iterator it2)
		{
			return it1.mPointer == it2.mPointer;
		}

	private:
		LinkListNode	*mPointer;	// _00
		
		friend class LinkListImpl;
	};

	LinkListImpl() { Initialize_(); }
	~LinkListImpl();

	Iterator GetBeginIter() { return mNode.GetNext(); }
	Iterator GetEndIter() { return &mNode; }
	bool IsEmpty() const { return mSize == 0; }

	Iterator Insert(Iterator it, LinkListNode *p);
	void PushFront(LinkListNode *p) { Insert(GetBeginIter(), p); }

	Iterator Erase(Iterator it);
	Iterator Erase(Iterator itFirst, Iterator itLast);
	Iterator Erase(LinkListNode *p);
	void Clear();

	void Reverse();

	static Iterator GetIteratorFromPointer(LinkListNode *p)
	{
		return p;
	}

private:
	void Initialize_()
	{
		mSize		= 0;
		mNode.mNext	= &mNode;
		mNode.mPrev	= &mNode;
	}


	u32				mSize;	// _00
	LinkListNode	mNode;	// _04
};
} // namespace detail

namespace dummy { 
template <typename, int> 
class Iterator {
	
}; 
}

template <typename T, int I>
class LinkList : private detail::LinkListImpl {
public:
	class Iterator : public dummy::Iterator<T, I> {
	public:
		Iterator(detail::LinkListImpl::Iterator it)
			:it_(it)
			{
			}

		/* ~Iterator() = default; */

		Iterator const &operator ++()
		{
			it_.operator ++();

			return *this;
		}

		Iterator operator ++(int)
		{
			Iterator it = *this;

			operator ++();

			return it;
		}

		T &operator *() const
		{
			T *p = (*this).operator ->();

			return *p;
		}

		T *operator ->() const
		{
			return GetPointerFromNode(it_.operator ->());
		}

		friend bool operator ==(Iterator it1, Iterator it2)
		{
			return operator ==(it1.it_, it2.it_);
		}

		friend bool operator !=(Iterator it1, Iterator it2)
		{
			return !operator ==(it1, it2);
		}

	private:
		detail::LinkListImpl::Iterator	it_;	// _00

		friend class LinkList<T, I>;
	};

	LinkList() {}
	/* ~LinkList() = default; */
	
	Iterator GetBeginIter() { return LinkListImpl::GetBeginIter(); }
	Iterator GetEndIter() { return LinkListImpl::GetEndIter(); }

	Iterator Insert(Iterator it, T *p)
	{
		return LinkListImpl::Insert(it.it_, GetNodeFromPointer(p));
	}

	void PushBack(T *p) { Insert(GetEndIter(), p); }

	Iterator Erase(Iterator it) { return LinkListImpl::Erase(it.it_); }
	Iterator Erase(T *p)
	{
		return LinkListImpl::Erase(GetNodeFromPointer(p));
	}

	static LinkListNode *GetNodeFromPointer(T *p)
	{
		return POINTER_ADD_TYPE(LinkListNode *, p, I);
	}

	static T *GetPointerFromNode(LinkListNode *p)
	{
		return POINTER_ADD_TYPE(T *, p, -I);
	}

	static Iterator GetIteratorFromPointer(T *p)
	{
		return GetIteratorFromPointer(GetNodeFromPointer(p));
	}

	static Iterator GetIteratorFromPointer(LinkListNode *p)
	{
		return LinkListImpl::GetIteratorFromPointer(p);
	}

private:

};
} // namespace ut
} // namespace nw4hbm

#endif // NW4HBM_UT_LINK_LIST_H
