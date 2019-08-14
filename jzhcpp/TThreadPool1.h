#ifndef TTHREADPOOL_H
#define TTHREADPOOL_H

#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include <condition_variable>
#include <queue>
#include <functional>
#include <future>

namespace TP{

    class TthreadPool
    {
    public:
        ~TthreadPool() {
            ShutDown();
            mCVvar.notify_all();
            
            // �ȴ������߳̽���
            for (auto& t : mPool) {
                if (t.joinable())
                    t.join();
            }
        }

        void ShutDown() {
            // ��atomic��������true�� memory_order_release��memory_order_acquireһ��ʹ��
            // store memory_order_acquire��load memory_order_releaseһ���ܵõ�store��ֵ
            mStop.store(true, std::memory_order_release);
        }
        
        TthreadPool(int n = 0) : mStop(false) {
            int nthreads = n;
            if (nthreads <= 0)
            {
                nthreads = std::thread::hardware_concurrency();
                nthreads = (nthreads == 0 ? 2 : nthreads);
            }

            for (int i = 0; i != nthreads; ++i) {
                mPool.emplace_back(std::thread([this](){

                    // ��ȡstopֵ�� ֹͣdonothing    ע�ⲻҪд��if����ֻ����nthreads�࣬���������������
                    while (!mStop.load(std::memory_order_acquire)) {

                        // ����������һ��ʹ��
                        std::unique_lock<std::mutex> ulk(this->mMutex);

                        // �����ȴ�ֱ��stopΪtrue��������Ϊ��
                        mCVvar.wait(ulk, [this](){return this->mStop.load(std::memory_order_acquire) ||
                            !this->mTasks.empty();
                        });

                        // ���stopΪtrue || ����Ϊ��donothing
                        if (mStop.load(std::memory_order_acquire) || mTasks.empty()) 
                            return;

                        // ִ������
                        Task task = std::move(mTasks.front());
                        mTasks.pop();
                        task();
                    }
                }));
            }
        }
        
        template<class fun, class... args>
        std::future<typename std::result_of<fun(args...)>::type> Add(fun&& f, args&&... arg) {
            // typedef ����ֵ����
            typedef typename std::result_of<fun(args...)>::type returntype;
            typedef std::packaged_task<returntype()> task;

            // ʹ������ָ�룬 ��ȡfuture
            // std::forward<T>(u) ������������T �� u����TΪ��ֵ��������ʱ��u����ת��ΪT���͵���ֵ������u����ת��ΪT������ֵ��
            // ��˶���std::forward��Ϊ����ʹ����ֵ���ò����ĺ���ģ���н������������ת������
            auto t = std::make_shared<task>(std::bind(std::forward<fun>(f), std::forward<args>(arg)...));
            auto ret = t->get_future();

            // ����
            std::lock_guard<std::mutex> ulk(mMutex);

            // �̳߳�ֹͣ���׳��쳣
            if (mStop.load(std::memory_order_acquire))
                throw std::runtime_error("thread pool has stopped");

            // ������� ��׽t��ִ��t
            mTasks.emplace([t]{(*t)();});

            // ֪ͨ�߳�
            mCVvar.notify_one();

            return ret;
        }

    private:
        TthreadPool(const TthreadPool&) =               delete;
        TthreadPool(TthreadPool&&) =                    delete;
        TthreadPool& operator=(const TthreadPool&) =    delete;
        TthreadPool& operator=(TthreadPool&&) =         delete;

    private:
        using Task = std::function<void()>;


        std::vector<std::thread>    mPool;
        std::queue<Task>            mTasks;
        std::mutex                  mMutex;
        std::atomic<bool>           mStop;
        std::condition_variable     mCVvar;
    };
}


#endif // TTHREADPOOL_H