#pragma once
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

// �첽д��־�������
template<typename T>
class LockQueue {
public:
    // ���worker�̶߳���д��־
    void Push(const T &data) {
        std::lock_guard<std::mutex> lock(m_mtx);
        m_queue.push(data);
        // ���ڶ���־���߳�ֻ��һ��, ��������Ҳ���軽��һ��
        m_convariable.notify_one();
    }

    // һ���̶߳���־queue, д��־�ļ�
    T Pop() {
        std::unique_lock<std::mutex> lock(m_mtx);
        while (m_queue.empty()) {
            // ��־����Ϊ��, ��ȡ��־�߳̽�������
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