#include "homebuttonLib/nw4hbm/ut/list.h"
#include <types.h>

#define ObjectToLink_(list_, object_)	\
	POINTER_ADD_TYPE(Link *, object_, (list_)->offset)

namespace nw4hbm { 
namespace ut {

void List_Init(List *list, u16 offset)
{
	list->headObject = nullptr;
	list->tailObject = nullptr;

	list->numObjects = 0;

	list->offset = offset;
}

static void SetFirstObject(List *list, void *object)
{
	Link *link = ObjectToLink_(list, object);

	link->nextObject = nullptr;
	link->prevObject = nullptr;

	list->headObject = object;
	list->tailObject = object;

	++list->numObjects;
}

void List_Append(List *list, void *object)
{
	if (!list->headObject)
	{
		SetFirstObject(list, object);
	}
	else
	{
		Link *link = ObjectToLink_(list, object);

		link->prevObject = list->tailObject;
		link->nextObject = nullptr;

		ObjectToLink_(list, list->tailObject)->nextObject = object;

		list->tailObject = object;

		++list->numObjects;
	}
}

void List_Prepend(List *list, void *object)
{
	if (!list->headObject)
	{
		SetFirstObject(list, object);
	}
	else
	{
		Link *link = ObjectToLink_(list, object);

		link->prevObject = nullptr;
		link->nextObject = list->headObject;

		ObjectToLink_(list, list->headObject)->prevObject = object;

		list->headObject = object;

		++list->numObjects;
	}
}

void List_Insert(List *list, void *target, void *object)
{
	if (!target)
	{
		List_Append(list, object);
	}
	else if (target == list->headObject)
	{
		List_Prepend(list, object);
	}
	else
	{
		Link *link = ObjectToLink_(list, object);
		void *prevObj = ObjectToLink_(list, target)->prevObject;
		Link *prevLnk = ObjectToLink_(list, prevObj);

		link->prevObject = prevObj;
		link->nextObject = target;

		prevLnk->nextObject = object;

		ObjectToLink_(list, target)->prevObject = object;

		++list->numObjects;
	}
}

void List_Remove(List *list, void *object)
{
	Link *link = ObjectToLink_(list, object);

	if (!link->prevObject)
		list->headObject = link->nextObject;
	else
		ObjectToLink_(list, link->prevObject)->nextObject = link->nextObject;

	if (!link->nextObject)
		list->tailObject = link->prevObject;
	else
		ObjectToLink_(list, link->nextObject)->prevObject = link->prevObject;

	link->prevObject = nullptr;
	link->nextObject = nullptr;

	--list->numObjects;
}

void *List_GetNext(List const *list, void const *object)
{
	if (object == nullptr) // NOTE: ELOGNOT
		return list->headObject;
	else
		return ObjectToLink_(list, object)->nextObject;
}

void *List_GetPrev(List const *list, void const *object)
{
	if (object == nullptr) // NOTE: ELOGNOT
		return list->tailObject;
	else
		return ObjectToLink_(list, object)->prevObject;
}

void *List_GetNth(List const *list, u16 index)
{
	int count = 0;
	Link *object = nullptr;

	for (; (object = static_cast<Link *>(List_GetNext(list, object))); ++count)
	{
		if (index == count)
			return object;
	}

	return nullptr;
}

} // namespace ut
} // namespace nw4hbm
