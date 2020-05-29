#pragma once
#include <semaphore.h>
#include <zookeeper/zookeeper.h>
#include <string>

// ��װ��zk�ͻ�����
class ZkClient {
public:
    ZkClient();
    ~ZkClient();
    
    // zkclient��������zkserver
    void Start();

    // ��zkserver�ϸ���ָ����path����znode�ڵ�
    void Create(const char *path, const char *data, int datalen, int state = 0);

    // ���ݲ���ָ����znode�ڵ�·��, ����znode�ڵ��ֵ
    std::string Getdata(const char *path);

private:
    // zk�Ŀͻ��˾��
    zhandle_t *m_zhandle;
};