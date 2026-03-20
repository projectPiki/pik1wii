#ifndef NW4HBM_UT_LIST_H
#define NW4HBM_UT_LIST_H

#include <types.h>

namespace nw4hbm { 
namespace ut {

struct List {
	void *headObject;	// _00
	void *tailObject;	// _04
	u16 numObjects;		// _08
	u16 offset;			// _0A
};

struct Link {
	void *prevObject;	// _00
	void *nextObject;	// _04
};

void List_Init(List *list, u16 offset);

void List_Append(List *list, void *object);
void List_Prepend(List *list, void *object);
void List_Insert(List *list, void *target, void *object);
void List_Remove(List *list, void *object);

void *List_GetNext(List const *list, void const *object);
void *List_GetPrev(List const *list, void const *object);
void *List_GetNth(List const *list, u16 index);

inline void *List_GetFirst(List const *list)
{
	return List_GetNext(list, nullptr);
}

inline u16 List_GetSize(List const *list)
{
	return list->numObjects;
}

inline void const *List_GetNthConst(List const *list, u16 const index)
{
	return List_GetNth(list, index);
}

} // namespace ut
} // namespace nw4hbm

#endif // NW4HBM_UT_LIST_H
