#include <iostream>
#include <string>
#include "user.pb.h"
#include "kmrpcapplication.h"
#include "kmrpcprovider.h"

// UserServiceRpc原来是一个本地服务, 提供了两个进程内的本地方法, Login和GetFriendLists

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
        // 框架给业务上报了请求参数LoginRequest, 应用获取相应的数据做本地业务
        std::string name = request->name();
        std::string pwd = request->pwd();

        // 做本地业务
        bool login_result = Login(name, pwd);

        // 把响应写入, 包括错误码, 错误消息, 返回值
        kingming::ResultCode *code = response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("");
        response->set_success(login_result);

        // 执行回调操作, 执行响应对象数据的序列化和网络发送(都由框架完成)
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
    // 调用框架的初始化操作
    KmrpcApplication::Init(argc, argv);

    // provider是一个rpc网络服务对象. 把UserService对象发布到Rpc节点上
    KmrpcProvider provider;
    provider.NotifyService(new UserService());

    provider.Run();
    
    return 0;
}
