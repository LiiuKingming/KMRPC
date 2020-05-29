#pragma once
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

// 异步写日志缓冲队列
template<typename T>
class LockQueue {
public:
    // 多个worker线程都会写日志
    void Push(const T &data) {
        std::lock_guard<std::mutex> lock(m_mtx);
        m_queue.push(data);
        // 由于读日志的线程只有一个, 条件变量也仅需唤醒一个
        m_convariable.notify_one();
    }

    // 一个线程读日志queue, 写日志文件
    T Pop() {
        std::unique_lock<std::mutex> lock(m_mtx);
        while (m_queue.empty()) {
            // 日志队列为空, 读取日志线程进入阻塞
            m_convariable.wait(lock);
        }

        T data = m_queue.front();
        m_queue.pop();
        return data;
    }

private:
    std::queue<T> m_queue;
    std::mutex m_mtx;
    std::condition_variable m_convariable;
};