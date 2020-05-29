#include <iostream>
#include <string>
#include "user.pb.h"
#include "kmrpcapplication.h"
#include "kmrpcprovider.h"

// UserServiceRpcԭ����һ�����ط���, �ṩ�����������ڵı��ط���, Login��GetFriendLists

class UserService : public kingming::UserServiceRpc {
public:
    bool Login(std::string name, std::string pwd) {
        std::cout << "doing local service : Login" << std::endl;
        std::cout << "name:" << name << " pwd:" << pwd << std::endl;
        return true;
    }

    bool Register(uint32_t id, std::string name, std::string pwd) {
        std::cout << "doing register service : Register" << std::endl;
        std::cout <<"id" << id << "name:" << name << " pwd:" << pwd << std::endl;
        return true;
    }

    void Login(::google::protobuf::RpcController* controller,
                       const ::kingming::LoginRequest* request,
                       ::kingming::LoginResponse* response,
                       ::google::protobuf::Closure* done) {
        // ��ܸ�ҵ���ϱ����������LoginRequest, Ӧ�û�ȡ��Ӧ������������ҵ��
        std::string name = request->name();
        std::string pwd = request->pwd();

        // ������ҵ��
        bool login_result = Login(name, pwd);

        // ����Ӧд��, ����������, ������Ϣ, ����ֵ
        kingming::ResultCode *code = response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("");
        response->set_success(login_result);

        // ִ�лص�����, ִ����Ӧ�������ݵ����л������緢��(���ɿ�����)
        done->Run();
    }

    void Register(::google::protobuf::RpcController* controller,
                       const ::kingming::RegisterRequest* request,
                       ::kingming::RegisterResponse* response,
                       ::google::protobuf::Closure* done) {
        uint32_t id = request->id();
        std::string name = request->name();
        std::string pwd = request->pwd();

        bool ret = Register(id, name, pwd);

        response->mutable_result()->set_errcode(0);
        response->mutable_result()->set_errmsg("");
        response->set_success(ret);

        done->Run();
    }
};

int main(int argc, char **argv) {
    // ���ÿ�ܵĳ�ʼ������
    KmrpcApplication::Init(argc, argv);

    // provider��һ��rpc����������. ��UserService���󷢲���Rpc�ڵ���
    KmrpcProvider provider;
    provider.NotifyService(new UserService());

    provider.Run();
    
    return 0;
}
