#include "pch.h"
#include "MemoryPool.h"


MemoryPool::MemoryPool(int32 allocSize) : _allocSize(allocSize)
{

}

MemoryPool::~MemoryPool()
{
	while (!_queue.empty())
	{
		 auto& k = _queue.front();
		 delete k;
		 k = nullptr;
	}
}


MemoryHeader* MemoryPool::Pop()
{
	if (!_queue.empty())
	{
		MemoryHeader* ptr =	_queue.front();
		_queue.pop();
		return ptr;
	}

	malloc(_allocSize);

}

void MemoryPool::Push()
{

}