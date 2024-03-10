#pragma once

#define SAFE_DELETE(ptr)	\
if (ptr)					\
{							\
	delete ptr;				\
	ptr = nullptr;			\
}							\

#define USE_MANY_LOCKS(name, count) mutex _##name_locks[count];
#define USE_LOCK(name) USE_MANY_LOCKS(name, 1)
#define WRITE_LOCK_IDX(name, idx) lock_guard write_##name_LockGuard_##idx(_##name_locks[idx]);
#define WRITE_LOCK(name) WRITE_LOCK_IDX(name, 0)
