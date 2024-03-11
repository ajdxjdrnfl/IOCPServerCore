#pragma once

struct MemoryHeader
{
	int32 allocSize;
};

class MemoryPool
{
public:
	MemoryPool(int32 allocSize);
	~MemoryPool();

	MemoryHeader* Pop();
	void Push();

private:
	USE_LOCK(POOL)
	queue<MemoryHeader*> _queue;
	int32 _allocSize;
};

