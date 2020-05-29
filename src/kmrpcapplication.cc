#include "kmrpcapplication.h"
#include <iostream>
#include <unistd.h>
#include <string>

KmrpcConfig KmrpcApplication::m_config;

void ShowArgsHelp() {
    std::cout << "format: command -i <configfile>" << std::endl;
}

void KmrpcApplication::Init(int argc, char **argv) {
    if (argc < 2) {
        ShowArgsHelp();
        exit(EXIT_FAILURE);
    }

    int c = 0;
    std::string config_file;
    while ((c = getopt(argc, argv, "i:")) != -1) {
        switch (c) {
        case 'i':
            config_file = optarg;
            break;
        case '?':
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        case ':':
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        default:
            break;
        }
    }

    // ��ʼ���������ļ��� 
    // rpcserver_ip=  rpcserver_port   
    // zookeeper_ip=  zookepper_port=
    m_config.LoadConfigFile(config_file.c_str());
    
    // std::cout << "rpcserverip:" << m_config.Load("rpcserverip") << std::endl;
    // std::cout << "rpcserverport:" << m_config.Load("rpcserverport") << std::endl;
    // std::cout << "zookeeperip:" << m_config.Load("zookeeperip") << std::endl;
    // std::cout << "zookeeperport:" << m_config.Load("zookeeperport") << std::endl;

}

KmrpcApplication& KmrpcApplication::GetInstance() {
    static KmrpcApplication app;
    return app;
}

KmrpcConfig& KmrpcApplication::GetConfig() {
    return m_config;
}