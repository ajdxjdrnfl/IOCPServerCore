#pragma once
#include "BaseBehavior.h"

class BehaviorManager
{
public:
	BehaviorManager() { }
	~BehaviorManager() { }

	void AddBehavior(const wstring& name, BaseBehavior* behavior) { _behaviors[name] = behavior; }

private:
	map<wstring, BaseBehavior*> _behaviors;
};

