#include <iostream>
#include "kmrpcchannel.h"
#include "kmrpcapplication.h"
#include "friend.pb.h"

int main(int argc, char **argv) {
    // �������������Ժ�,ʹ��kmrpc�������rpc����������ȵ��ÿ�ܳ�ʼ������(ֻ��һ��)
    KmrpcApplication::Init(argc, argv);

    // ��ʾ����Զ�̷�����rpc����Login
    kingming::FriendServiceRpc_Stub stub(new KmrpcChannel());
    
    // rpc�������������
    kingming::GetFrinedsListRequest request;
    request.set_userid(7777);

    // rpc��������Ӧ
    kingming::GetFrinedsListResponse response;

    // ����rpc�������� ͬ����rpc���ù���->KmrpcChannel::callMethod
    KmrpcController controller;
    // RpcChannel->RpcChannel::callMethod ������������rpc�������õĲ������л������緢��
    stub.GetFrinedsList(&controller, &request, &response, nullptr);

    // ���������ӡ������־
    if (controller.Failed()) {
        std::cout << controller.ErrorText() << std::endl;
    } else {
        // һ��rpc�������,��ȡ���ý��
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