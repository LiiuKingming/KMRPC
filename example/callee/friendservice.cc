#include <iostream>
#include <string>
#include "friend.pb.h"
#include "kmrpcapplication.h"
#include "kmrpcprovider.h"
#include <vector>

class FriendService : public kingming::FriendServiceRpc {
public:
    std::vector<std::string> GetFrinedsList(uint32_t userid) {
        std::cout << "do GetFriendsList service! userid:" << userid << std::endl;
        std::vector<std::string> vec;
        vec.push_back("lu ben wei");
        vec.push_back("da si ma");
        vec.push_back("zha nan");
        return vec;
    }

    void GetFrinedsList(google::protobuf::RpcController* controller,
                       const ::kingming::GetFrinedsListRequest* request,
                       ::kingming::GetFrinedsListResponse* response,
                       ::google::protobuf::Closure* done) {
        uint32_t userid = request->userid();
        std::vector<std::string> friendsList = GetFrinedsList(userid);
        response->mutable_result()->set_errcode(0);
        response->mutable_result()->set_errmsg("");

        for (std::string &name : friendsList) {
            std::string *p = response->add_friends();
            *p = name;
        }
        done->Run();
    }
};


int main(int argc, char **argv) {
    // 调用框架的初始化操作
    KmrpcApplication::Init(argc, argv);

    // provider是一个rpc网络服务对象. 把UserService对象发布到Rpc节点上
    KmrpcProvider provider;
    provider.NotifyService(new FriendService());

    provider.Run();
    
    return 0;
}