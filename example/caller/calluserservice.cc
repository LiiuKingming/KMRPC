#include <iostream>
#include "kmrpcchannel.h"
#include "kmrpcapplication.h"
#include "user.pb.h"

int main(int argc, char **argv) {
    // 整个程序启动以后,使用kmrpc框架享受rpc服务则必须先调用框架初始化函数(只有一次)
    KmrpcApplication::Init(argc, argv);

    // 演示调用远程发布的rpc方法Login
    kingming::UserServiceRpc_Stub stub(new KmrpcChannel());
    
    // rpc方法的请求参数
    kingming::LoginRequest request;
    request.set_name("lbwnb");
    request.set_pwd("123456");
    // rpc方法的响应
    kingming::LoginResponse response;
    // 发起rpc方法调用 同步的rpc调用过程->KmrpcChannel::callMethod
    // RpcChannel->RpcChannel::callMethod 集中来做所有rpc方法调用的参数序列化和网络发送
    stub.Login(nullptr, &request, &response, nullptr);

    // 一次rpc调用完成,读取调用结果
    if (response.result().errcode() == 0) {
        std:: cout << "rpc login response success:" << response.success() << std::endl;
    } else {
        std::cout << "rpc login response error : " << response.result().errmsg() << std::endl;
    }


    // 演示调用远程发布的rpc方法Login
    // rpc方法的请求参数
    kingming::RegisterRequest req;
    req.set_id(7777);
    req.set_name("kmrpc");
    req.set_pwd("999999");
    // rpc方法的响应
    kingming::RegisterResponse rsp;
    // 发起rpc方法调用 同步的rpc调用过程->KmrpcChannel::callMethod
    // RpcChannel->RpcChannel::callMethod 集中来做所有rpc方法调用的参数序列化和网络发送
    stub.Register(nullptr, &req, &rsp, nullptr);

    // 一次rpc调用完成,读取调用结果
    if (rsp.result().errcode() == 0) {
        std:: cout << "rpc register response success:" << response.success() << std::endl;
    } else {
        std::cout << "rpc register response error : " << response.result().errmsg() << std::endl;
    }

    return 0;
}