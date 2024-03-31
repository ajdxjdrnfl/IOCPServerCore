#pragma once
class BaseBehavior
{
public:
	BaseBehavior() = default;
	virtual ~BaseBehavior() { }

	virtual void Start() abstract;
	virtual void Process() abstract;

private:
	
};

