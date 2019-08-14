// ThreadLocal.h
// Definition of the ThreadLocal.
// Copyleft 2018, jzh just do it

#ifndef THREAD_THREADPOOL_H
#define THREAD_THREADPOOL_H
#include <vector>
#include "Mutex.h"
#include "Thread.h"

namespace yep {
namespace thread {

class Runnable;
class PooledThread;

// TODO: int to int32

// <no used thread will be removed>
class ThreadPool
{
public:
	// <�߳̿��ܳ���idletime����ɾ��>
	ThreadPool(int minCapacity = 2,
		int maxCapacity = 16,
		int idleTime = 60,
		int stackSize = 0);

	ThreadPool(const std::string& name,
		int minCapacity = 2,
		int maxCapacity = 16,
		int idleTime = 60,
		int stackSize = 0);

	~ThreadPool();

	const std::string& name() const { return name_; }

	// <n ��������>
	void addCapacity(int n);

	int capacity() const { Mutex::ScopedLock lock(mutex_); return maxCapacity_; }

	void setStackSize(int size) { stackSize_ = size; }

	int getStackSize() const { return stackSize_; };

	// <��ǰʹ��>
	int used() const;

	// <��ǰ����>
	int allocated() const;

	// <��ǰ����>
	int available() const;

	void start(Runnable& target);

	void start(Runnable& target, const std::string& name);

	void startWithPriority(Thread::Priority priority, Runnable& target);

	void startWithPriority(Thread::Priority priority, Runnable& target, const std::string& name);

	void stopAll();

	// <�ȴ�runabled �߳����(��������)>
	void joinAll();

	// <�Ƴ�����ʹ�õ��߳�, ��start(), addCapacity(), joinAll() ��������˽����>
	void collect();

	// <Ĭ���̳߳�,����>
	static ThreadPool& defaultPool();
public:
	ThreadPool(const ThreadPool& pool) = delete;
	ThreadPool& operator = (const ThreadPool& pool) = delete;
protected:
	PooledThread* getThread();
	PooledThread* createThread();

	// <����idle�߳�>
	void housekeep();
private:
	typedef std::vector<PooledThread*> ThreadVec;

	std::string name_;
	unsigned int minCapacity_;	// <����>
	unsigned int maxCapacity_;
	int			idleTime_;		// <����ʱ��>
	int			serial_;		// <���>
	int			age_;
	int			stackSize_;

	ThreadVec	  threads_;
	mutable Mutex mutex_;
};

} // !namespace thread
} // !namespace yep

#endif // !THREAD_THREADPOOL_H