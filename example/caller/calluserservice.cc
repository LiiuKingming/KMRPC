#include <iostream>
#include "kmrpcchannel.h"
#include "kmrpcapplication.h"
#include "user.pb.h"

int main(int argc, char **argv) {
    // �������������Ժ�,ʹ��kmrpc�������rpc����������ȵ��ÿ�ܳ�ʼ������(ֻ��һ��)
    KmrpcApplication::Init(argc, argv);

    // ��ʾ����Զ�̷�����rpc����Login
    kingming::UserServiceRpc_Stub stub(new KmrpcChannel());
    
    // rpc�������������
    kingming::LoginRequest request;
    request.set_name("lbwnb");
    request.set_pwd("123456");
    // rpc��������Ӧ
    kingming::LoginResponse response;
    // ����rpc�������� ͬ����rpc���ù���->KmrpcChannel::callMethod
    // RpcChannel->RpcChannel::callMethod ������������rpc�������õĲ������л������緢��
    stub.Login(nullptr, &request, &response, nullptr);

    // һ��rpc�������,��ȡ���ý��
    if (response.result().errcode() == 0) {
        std:: cout << "rpc login response success:" << response.success() << std::endl;
    } else {
        std::cout << "rpc login response error : " << response.result().errmsg() << std::endl;
    }


    // ��ʾ����Զ�̷�����rpc����Login
    // rpc�������������
    kingming::RegisterRequest req;
    req.set_id(7777);
    req.set_name("kmrpc");
    req.set_pwd("999999");
    // rpc��������Ӧ
    kingming::RegisterResponse rsp;
    // ����rpc�������� ͬ����rpc���ù���->KmrpcChannel::callMethod
    // RpcChannel->RpcChannel::callMethod ������������rpc�������õĲ������л������緢��
    stub.Register(nullptr, &req, &rsp, nullptr);

    // һ��rpc�������,��ȡ���ý��
    if (rsp.result().errcode() == 0) {
        std:: cout << "rpc register response success:" << response.success() << std::endl;
    } else {
        std::cout << "rpc register response error : " << response.result().errmsg() << std::endl;
    }

    return 0;
}