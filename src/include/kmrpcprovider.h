#pragma once
#include "google/protobuf/service.h"
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>
#include <google/protobuf/descriptor.h>
#include <string>
#include <functional>
#include <unordered_map>

// ����ṩ��ר�ŷ���rpc���������������൱��һ��RpcServer
class KmrpcProvider {
public:
    // �����ǿ���ṩ���ⲿʹ�õģ����Է���rpc�����ĺ����ӿ�
    void NotifyService(google::protobuf::Service *service);

    // ����rpc����ڵ㣬��ʼ�ṩrpcԶ��������÷���
    void Run();

private:
    // ���EventLoop
    muduo::net::EventLoop m_eventLoop;

    struct ServerceInfo {
        google::protobuf::Service *m_service; // ����������
        std::unordered_map<std::string, const google::protobuf::MethodDescriptor*> m_methodMap;// �洢���񷽷�
    };
    
    // �洢�ɹ�ע��ķ�����������񷽷���������Ϣ <name, info>
    std::unordered_map<std::string, ServerceInfo> m_serviceMap;


    // �µ�socket���ӻص�
    void OnConnection(const muduo::net::TcpConnectionPtr&);
    // �ѽ��������û��Ķ�д�¼��ص�
    void OneMessage(const muduo::net::TcpConnectionPtr&, muduo::net::Buffer*, muduo::Timestamp);
    // �ú����󶨵�Closure�Ļص��������������л�rpc����Ӧ�����緢��
    void SendRpcResponse(const muduo::net::TcpConnectionPtr&, google::protobuf::Message*);
};