#include "pch.h"
#include "ThreadManager.h"


ThreadManager::ThreadManager()
{

}

ThreadManager::~ThreadManager()
{

}

void ThreadManager::Launch(::function<void(void)> callback)
{
	::lock_guard lockGuard(_lock);

	_threads.push_back(thread([=]()
	{
		InitTLS();
		callback();
		DestroyTLS();
	}));

}

void ThreadManager::Join()
{
	for (auto& thread : _threads)
	{
		if (thread.joinable())
		{
			thread.join();
		}
	}

	_threads.clear();
}

void ThreadManager::InitTLS()
{
	static ::atomic<uint32> SThreadId = 1;
	LThreadId = SThreadId.fetch_add(1);
}

void ThreadManager::DestroyTLS()
{
	
}