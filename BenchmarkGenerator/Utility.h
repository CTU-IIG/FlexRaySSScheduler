/// \file Utility.h Header file of the Utility.cpp and the sourcecode of templates

#ifndef UTILITY_H_
#define UTILITY_H_
#include <set>
#include <stdio.h>

/// Safe allocator without initialization
/// @param itemCount Number of items in array to be allocated
/// @return Allocated memory
template<class TYPE> TYPE* nnew(int itemCount);
/// Safe allocator with initialization
/// @param itemCount Number of items in array to be allocated and initalized
/// @return Allocated and initialized memory
template<class TYPE> TYPE* nnewi(int itemCount);
/// Save dealocator
/// @param Memory Pointer to the memory to be freed
template<class TYPE> void ddelete(TYPE* Memory);
/// Dealocate all the memory that was safely allocated and display it
void ddeleteAll();

extern std::set<void*> allocatedMemory; ///< The set of allocated memory

template<class TYPE>
TYPE* nnew(int itemCount)
{
	TYPE* memory = new TYPE[itemCount];
	allocatedMemory.insert(static_cast<void*> (memory));
	return memory;
}

template<class TYPE>
TYPE* nnewi(int itemCount)
{
	TYPE* memory = new TYPE[itemCount]();
	allocatedMemory.insert(static_cast<void*>(memory));
	return memory;
}

template<class TYPE>
void ddelete(TYPE* Memory)
{
	std::set<void*>::iterator it;
	it = allocatedMemory.find(static_cast<void*>(Memory));
	if(it != allocatedMemory.end())
	{
		delete[] Memory;
		allocatedMemory.erase(it);
	}
	else
	{
		printf("fe%d ", Memory);
	}
}

template<class TYPE>
bool isAllocated(TYPE *Memory) {
	if (Memory == nullptr)
		return false;
	return allocatedMemory.find(static_cast<void *>(Memory)) != allocatedMemory.end();
}

#endif