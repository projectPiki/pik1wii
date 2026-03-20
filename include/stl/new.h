#ifndef NEW_H
#define NEW_H

inline void* operator new(size_t size, void* mem)
{
	return mem;
}

//void operator delete[](void *ptr);

#endif // NEW_H