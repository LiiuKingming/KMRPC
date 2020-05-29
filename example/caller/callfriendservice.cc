#include <iostream>
#include "kmrpcchannel.h"
#include "kmrpcapplication.h"
#include "friend.pb.h"

int main(int argc, char **argv) {
    // 整个程序启动以后,使用kmrpc框架享受rpc服务则必须先调用框架初始化函数(只有一次)
    KmrpcApplication::Init(argc, argv);

    // 演示调用远程发布的rpc方法Login
    kingming::FriendServiceRpc_Stub stub(new KmrpcChannel());
    
    // rpc方法的请求参数
    kingming::GetFrinedsListRequest request;
    request.set_userid(7777);

    // rpc方法的响应
    kingming::GetFrinedsListResponse response;

    // 发起rpc方法调用 同步的rpc调用过程->KmrpcChannel::callMethod
    KmrpcController controller;
    // RpcChannel->RpcChannel::callMethod 集中来做所有rpc方法调用的参数序列化和网络发送
    stub.GetFrinedsList(&controller, &request, &response, nullptr);

    // 发生错误打印错误日志
    if (controller.Failed()) {
        std::cout << controller.ErrorText() << std::endl;
    } else {
        // 一次rpc调用完成,读取调用结果
        if (response.result().errcode() == 0) {
            std:: cout << "rpc login response success!" << std::endl;
            int size = response.friends_size();
            for (int i = 0; i < size; ++i) {
                std::cout << "index: " << (i + 1) << " name: " << response.friends(i) << std::endl; 
            }
        } else {
            std::cout << "rpc login response error : " << response.result().errmsg() << std::endl;
        }
    }
    return 0;
}