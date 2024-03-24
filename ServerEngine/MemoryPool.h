#pragma once

struct MemoryHeader
{
	int32 allocSize;

	MemoryHeader(int32 size) : allocSize(size) { }

	static void* AttachHeader(MemoryHeader* header, int32 size)
	{
		// size = allocSize + headerSize
		new(header)MemoryHeader(size);
		return reinterpret_cast<void*>(++header);
	}

	static MemoryHeader* DetachHeader(void* ptr)
	{
		MemoryHeader* header = reinterpret_cast<MemoryHeader*>(ptr) - 1;
		return header;
	}
};

class SmallMemoryPool
{
public:
	SmallMemoryPool(int32 allocSize);
	~SmallMemoryPool();

	MemoryHeader* Pop();
	void Push(MemoryHeader* ptr);

private:
	USE_LOCK(POOL)
	queue<MemoryHeader*> _queue;
	int32 _allocSize;
};

class MemoryPool
{
	enum
	{
		POOL_COUNT = (1024 / 32) + (2048 / 128) + (4096 / 256),
		MAX_ALLOC_SIZE = 4096
	};

public:
	MemoryPool();
	~MemoryPool();

	void* Allocate(int32 size);
	void Release(void* ptr);


private:
	vector<SmallMemoryPool*> _pools;

	SmallMemoryPool* _poolTable[MAX_ALLOC_SIZE+1];
};
