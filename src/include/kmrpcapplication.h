# pragma once
#include "kmrpcconfig.h"
#include "kmrpcchannel.h"
#include "kmrpccontroller.h"

// kmrpc��ܵĻ�����, �����ܵ�һЩ��ʼ������
class KmrpcApplication {
public:
    static void Init(int argc, char **argv);
    static KmrpcApplication& GetInstance();
    static KmrpcConfig& GetConfig();
private:
    static KmrpcConfig m_config;
    KmrpcApplication() {}
    KmrpcApplication(const KmrpcApplication&) = delete;
    KmrpcApplication(KmrpcApplication&&) = delete;
};