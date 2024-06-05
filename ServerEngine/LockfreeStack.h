#pragma once

template <typename T>
struct Node
{
	T data;
	Node<T>* next = nullptr;

	Node() : data(0), next(nullptr) {}
	Node(const T& _data) : data(_data), next(nullptr) { }
};

template <typename T>
struct HazardPointer
{
	Node<T>* node = nullptr;
	HazardPointer<T>* next = nullptr;
	::atomic<bool> active = false;
};

template <typename T>
class LockfreeStack
{
public:
	LockfreeStack(int size)
	{
		_retireList.resize(size);
	}

	void Push(const T& data)
	{
		Node<T>* newNode = new Node<T>(data);

		newNode->next = _head.load();

		while (_head.compare_exchange_strong(newNode->next, newNode) == false);

	}

	bool Pop(T& value)
	{

		HazardPointer<T>* hp = AllocateHazardPointer();

		hp->node = _head.load();

		if (hp->node == nullptr)
		{
			ReleaseHazardPointer(hp);
			return false;
		}

		while (hp->node != nullptr && _head.compare_exchange_strong(hp->node, hp->node->next) == false);

		if (hp->node == nullptr)
		{
			ReleaseHazardPointer(hp);
			return false;
		}

		Node<T>* node = hp->node;
		node->next = nullptr;
		value = hp->node->data;
		AddRetireList(node);
		ReleaseHazardPointer(hp);

		return true;
	}

	HazardPointer<T>* AllocateHazardPointer()
	{

		HazardPointer<T>* p = nullptr;

		for (p = _hpHead.load(); p != nullptr; p = p->next)
		{
			bool active = p->active.load();

			if (active == false)
			{
				if (p->active.compare_exchange_weak(active, true))
					return p;
			}
		}

		p = new HazardPointer<T>();
		p->active.store(true);
		p->node = nullptr;

		p->next = _hpHead.load();

		while (_hpHead.compare_exchange_weak(p->next, p) == false);

		return p;


	}

	void ReleaseHazardPointer(HazardPointer<T>* hp)
	{
		hp->node = nullptr;
		hp->active.store(false);
	}

	void AddRetireList(Node<T>* node)
	{
		_retireList[_threadId].push_back(node);

		if (_retireList[_threadId].size() >= 10)
		{
			RemoveRetireList();
		}
	}

	void RemoveRetireList()
	{
		vector<Node<T>*> hpList;

		HazardPointer<T>* p = nullptr;

		vector<Node<T>*> tempList;

		for (p = _hpHead.load(); p != nullptr; p = p->next)
		{

			Node<T>* node = p->node;

			if (node != nullptr)
				hpList.push_back(node);
		}

		auto iter = _retireList[_threadId].end();
		auto prevIter = iter;
		for (int i = 0; i < hpList.size(); i++)
		{
			Node<T>* node = hpList[i];
			iter = remove_if(_retireList[_threadId].begin(), prevIter, [node](Node<T>* list)
			{
				return list == node;
			});

			if (iter != prevIter)
			{
				tempList.push_back(node);
			}

			prevIter = iter;
		}

		int index = iter - _retireList[_threadId].begin();

		for (int i = 0; i < index; i++)
		{
			Node<T>* node = _retireList[_threadId][i];
			delete node;
			_retireList[_threadId][i] = nullptr;
		}

		if (tempList.empty())
			_retireList[_threadId].clear();
		else
			_retireList[_threadId] = tempList;

	}

	void SetThreadId(int threadId)
	{
		_threadId = threadId;
	}



private:
	// CRITICAL SECTION
	::atomic<Node<T>*> _head = nullptr;
	::atomic<HazardPointer<T>*> _hpHead = nullptr;

private:
	vector<vector<Node<T>*>> _retireList;

public:
	static thread_local int _threadId;
};

template <typename T>
thread_local int LockfreeStack<T>::_threadId = 0;
