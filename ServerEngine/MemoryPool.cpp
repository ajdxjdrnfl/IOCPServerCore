#include "pch.h"
#include "MemoryPool.h"


SmallMemoryPool::SmallMemoryPool(int32 allocSize) : _allocSize(allocSize)
{

}

SmallMemoryPool::~SmallMemoryPool()
{
	while (!_queue.empty())
	{
		 auto& k = _queue.front();
		 _queue.pop();
		 ::free(k);
		 k = nullptr;
	}
}


MemoryHeader* SmallMemoryPool::Pop()
{
	{
		WRITE_LOCK(POOL);
		if (!_queue.empty())
		{
			MemoryHeader* ptr = _queue.front();
			_queue.pop();
			return ptr;

		}
	}
	
	MemoryHeader* ptr = reinterpret_cast<MemoryHeader*>(::malloc(_allocSize));

	return ptr;

}

void SmallMemoryPool::Push(MemoryHeader* ptr)
{
	{
		WRITE_LOCK(POOL);
		ptr->allocSize = 0;
		_queue.push(ptr);
	}
}

MemoryPool::MemoryPool()
{
	int32 size = 0;
	int32 tableIndex = 0;

	for (size = 32; size <= 1024; size += 32)
	{
		SmallMemoryPool* pool = new SmallMemoryPool(size);
		_pools.push_back(pool);

		while (tableIndex <= size)
		{
			_poolTable[tableIndex] = pool;
			tableIndex++;
		}
	}

	for (; size <= 2048; size += 128)
	{
		SmallMemoryPool* pool = new SmallMemoryPool(size);
		_pools.push_back(pool);

		while (tableIndex <= size)
		{
			_poolTable[tableIndex] = pool;
			tableIndex++;
		}
	}

	for (; size <= 4096; size += 256)
	{
		SmallMemoryPool* pool = new SmallMemoryPool(size);
		_pools.push_back(pool);

		while (tableIndex <= size)
		{
			_poolTable[tableIndex] = pool;
			tableIndex++;
		}
	}
}

MemoryPool::~MemoryPool()
{
	for (SmallMemoryPool* pool : _pools)
	{
		delete pool;
		pool = nullptr;
	}

	_pools.clear();
}

void* MemoryPool::Allocate(int32 size)
{
	MemoryHeader* header = nullptr;
	const int32 allocSize = size + sizeof(header);

	if (allocSize > MAX_ALLOC_SIZE)
	{
		header = reinterpret_cast<MemoryHeader*>(::malloc(allocSize));
	}
	else
	{
		header = _poolTable[allocSize]->Pop();
	}

	return MemoryHeader::AttachHeader(header, allocSize);
}

void MemoryPool::Release(void* ptr)
{
	MemoryHeader* header = MemoryHeader::DetachHeader(ptr);

	const int32 allocSize = header->allocSize;
	assert(allocSize > 0);

	if (allocSize > MAX_ALLOC_SIZE)
		::free(header);
	else
		_poolTable[allocSize]->Push(header);


}

