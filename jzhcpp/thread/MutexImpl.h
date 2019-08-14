// MutexImpl.h
// Definition of the Mutex.
// Copyleft 2018, jzh just do it

#ifndef THREAD_MUTEX_IMPL_H
#define THREAD_MUTEX_IMPL_H

#include "../base/Basic.h"
#include "Timestamp.h"

namespace yep {
namespace thread{

// <��ʵ����, windows �ٽ���Ĭ��֧�ֵݹ�>
class MutexImpl{
protected:
	MutexImpl() 
	{
		// <4000, ����ѭ������, EnterCriticalSection()�������ڲ�ѭ���жϴ��ٽ����Ƿ���Խ��룬ֱ�����Խ������N����>
		InitializeCriticalSectionAndSpinCount(&cs_, 4000);
	}

	~MutexImpl()
	{
		DeleteCriticalSection(&cs_);
	}

	void lockImpl();
	bool tryLockImpl();
	bool tryLockImpl(long milliseconds);
	void unlockImpl();

private:
	CRITICAL_SECTION cs_;
};

inline void MutexImpl::lockImpl()
{
	try 
	{
		EnterCriticalSection(&cs_);
	} 
	catch (...) 
	{
		// TODO: thread error ����thread error
		throw Error("can not lock mutex");
	}
}

inline bool MutexImpl::tryLockImpl()
{
	try 
	{
		return TryEnterCriticalSection(&cs_) != 0;
	}
	catch (...) 
	{
	}

	throw Error("can not lock mutex");
}

inline void MutexImpl::unlockImpl()
{
	LeaveCriticalSection(&cs_);
}

inline bool MutexImpl::tryLockImpl(long milliseconds)
{
	const int sleepMillis = 5;
	Timestamp now;

	// <int64 millisecond>
	Timestamp::TimeDiff diff(Timestamp::TimeDiff(milliseconds) * 1000);
	
	do 
	{
		try 
		{
			if (TryEnterCriticalSection(&cs_) == TRUE) {
				return true;
			}
		}
		catch (...) 
		{
			throw Error("can not lock mutex");
		}
	
		Sleep(sleepMillis);
	}
	while (!now.isElapsed(diff));

	return false;
}


} // !namespace thread
} // !namespace yep
#endif // !THREAD_MUTEX_IMPL_H