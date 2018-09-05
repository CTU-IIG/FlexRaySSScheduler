/// \file Utility.cpp Utility functions used for safe allocation/deallocation etc.
#include "Utility.h"

std::set<void*> allocatedMemory;

void ddeleteAll()
{
	std::set<void*>::iterator it;
	for(it = allocatedMemory.begin(); it != allocatedMemory.end();)
	{
		printf("fa%d ", *it);
		delete[] *it;
		allocatedMemory.erase(*it);
		it = allocatedMemory.begin();
	}
}