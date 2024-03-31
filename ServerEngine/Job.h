#pragma once

class Job
{
public:
	Job(::function<void(void)>&& callback) : _callback(::forward(callback)) { }
	
	template<typename T, typename F, typename... Args>
	Job(shared_ptr<T> owener, F func, Args&&... args)
	{
		_callback = [owner, func, args...]()
		{
			
		}
	}

	void Execute()
	{
		_callback();
	}

private:
	std::function<void(void)> _callback;
};

