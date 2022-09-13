/*
 * @Author       : mark
 * @Date         : 2020-06-15
 * @copyleft Apache 2.0
 */ 

#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <mutex>
#include <condition_variable>
#include <queue>
#include <thread>
#include <functional>
class ThreadPool {
public:
    explicit ThreadPool(size_t threadCount = 8): pool_(std::make_shared<Pool>()) {
            assert(threadCount > 0);//断言，测试用的
            //创建threadCount个子线程
            for(size_t i = 0; i < threadCount; i++) {
                std::thread([pool = pool_] {//c++中创建线程
                    std::unique_lock<std::mutex> locker(pool->mtx);
                    while(true) {//每个线程无限从任务队列取任务
                        if(!pool->tasks.empty()) {
                            //取任务
                            auto task = std::move(pool->tasks.front());
                            //移除任务
                            pool->tasks.pop();
                            locker.unlock();//锁共享数据
                            task();//任务执行
                            locker.lock();
                        } 
                        else if(pool->isClosed) break;//线程结束，自动释放
                        else pool->cond.wait(locker);//线程池为空且没退出，阻塞
                    }
                }).detach();//设置线程分离防止僵尸进程
            }
    }

    ThreadPool() = default;//无参构造函数

    ThreadPool(ThreadPool&&) = default;
    
    ~ThreadPool() {
        if(static_cast<bool>(pool_)) {
            {
                std::lock_guard<std::mutex> locker(pool_->mtx);
                pool_->isClosed = true;
            }
            pool_->cond.notify_all();//唤醒所有休眠码代码
        }
    }

    template<class F>
    void AddTask(F&& task) {
        {
            std::lock_guard<std::mutex> locker(pool_->mtx);
            pool_->tasks.emplace(std::forward<F>(task));
        }
        pool_->cond.notify_one();//任务队列新增任务，通知线程池
    }

private:
    //结构体，池子
    struct Pool {
        std::mutex mtx;//互斥锁
        std::condition_variable cond;//条件变量
        bool isClosed;//是否关闭
        std::queue<std::function<void()>> tasks;//任务队列
    };
    std::shared_ptr<Pool> pool_;
};


#endif //THREADPOOL_H