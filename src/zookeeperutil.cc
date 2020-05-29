#include "zookeeperutil.h"
#include "kmrpcapplication.h"
#include <semaphore.h>
#include <iostream>

// 全局的watcher观察器-->回调, zkserver给zkclient的通知
void global_watcher (zhandle_t *zh, int type, 
            int state, const char *path, void *watcherCtx) {
    // 回调的消息类型是和会话相关的消息类型
    if (type == ZOO_SESSION_EVENT) {
        // zkclient 和 zkserver连接成功
        if (state == ZOO_CONNECTED_STATE) {
            sem_t *sem = (sem_t*)zoo_get_context(zh);
            sem_post(sem);
        }
    }
}

ZkClient::ZkClient() : m_zhandle(nullptr) {}

ZkClient::~ZkClient() {
    if (m_zhandle != nullptr) {
        // 关闭句柄释放资源
        zookeeper_close(m_zhandle);
    }
}

// 连接zkserver
void ZkClient::Start() {
    std::string host = KmrpcApplication::GetInstance().GetConfig().Load("zookeeperip");
    std::string post = KmrpcApplication::GetInstance().GetConfig().Load("zookeeperport");
    std::string connstr = host + ":" + post;

    /*
    zookeeper_mt:多线程版本
    zookeeper的客户端程序提供了三个线程
    API调用线程--主线程
    网络I/O线程 pthread_create -> poll
    watcher回调线程 pthread_create
    */
    // zookeeper_init仅表示向zkServer发送连接请求, 不代表已完成连接
    m_zhandle = zookeeper_init(connstr.c_str(), global_watcher, 30000, nullptr, nullptr, 0);
    if (m_zhandle == nullptr) {
        std::cout << "zookeeper_init error!" << std::endl;
        exit(EXIT_FAILURE);
    }

    sem_t sem;
    sem_init(&sem, 0, 0);
    zoo_set_context(m_zhandle, &sem);

    sem_wait(&sem);
    std::cout << "zookeeper_init success!" << std::endl;
}

void ZkClient::Create(const char *path, const char *data, int datalen, int state) {
    char path_buf[128];
    int bufferlen = sizeof(path_buf);
    int flag;
    // 先判断path表示的znode节点是否存在, 若存在则不需要重复创建
    flag = zoo_exists(m_zhandle, path, 0, nullptr);
    
    // 表示path的znode节点不存在
    if (flag == ZNONODE) {
        // 创建指定path的znode节点
        flag = zoo_create(m_zhandle, path, data, datalen, 
                    &ZOO_OPEN_ACL_UNSAFE, state, path_buf, bufferlen);
        
        if (flag == ZOK) {
            std::cout << "znode create success ... path: " << path << std::endl;
        } else {
            std::cout << "flag: " << flag << std::endl;
            std::cout << "znode create error... path: " << path << std::endl;
            exit(EXIT_FAILURE);
        }
    }
}

// 根据指定的path, 获取znode节点的值
std::string ZkClient::Getdata(const char *path) {
    char buffer[64];
    int bufferlen = sizeof (buffer);
    int flag = zoo_get(m_zhandle, path, 0, buffer, &bufferlen, nullptr);
    if (flag != ZOK) {
        std::cout << "get znode error... path: " << path << std::endl;
        return "";
    } else {
        return buffer;
    }
}

