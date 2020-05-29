#include "kmrpcprovider.h"
#include "kmrpcapplication.h"
#include "rpcheader.pb.h"
#include "logger.h"
#include "zookeeperutil.h"

void KmrpcProvider::NotifyService(google::protobuf::Service *service) {
    ServerceInfo service_info;

    // 获取服务对象的描述信息 指针
    const google::protobuf::ServiceDescriptor *pServiceDesc = service->GetDescriptor();
    // 获取服务对象的名字
    std::string service_name = pServiceDesc->name();
    // 获取服务对象的方法数量
    int methodCnt = pServiceDesc->method_count();

    // std::cout << "service_name: " << service_name << std::endl;
    LOG_INFO("service_name: %s", service_name.c_str());

    for (int i = 0; i < methodCnt; ++i) {
        // 获取服务对象指定下标服务方法的描述(抽象描述)
        const google::protobuf::MethodDescriptor *pMethodDesc = pServiceDesc->method(i);
        std::string method_name = pMethodDesc->name();
        service_info.m_methodMap.insert({method_name, pMethodDesc});

        // std::cout << "method_name: " << method_name << std::endl;
        LOG_INFO("method_name: %s", method_name.c_str());
    }

    service_info.m_service = service;
    m_serviceMap.insert({service_name, service_info});
}

void KmrpcProvider::Run() {
    // 读取配置文件rpcserver的信息
    std::string ip = KmrpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    uint16_t port = atoi(KmrpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());
    muduo::net::InetAddress address(ip, port);

    // 创建TcpServer对象
    muduo::net::TcpServer server(&m_eventLoop, address, "KmrpcProvider");

    // 绑定连接回调 消息读写回调方法, 分离网络代码和业务代码
    server.setConnectionCallback(std::bind(&KmrpcProvider::OnConnection, this, std::placeholders::_1));
    server.setMessageCallback(std::bind(&KmrpcProvider::OneMessage, this, 
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    // 设置线程库数量
    server.setThreadNum(4);

    // 把当前rpc节点上要发布的服务全部注册到zk上, 让rpc client可以从zk发现服务
    // session timeout 30s ---> zkclient网络I/O线程会每隔1/3 * timeout时间发送ping消息(心跳)
    ZkClient zkCli;
    zkCli.Start();
    // service_name 为永久性节点, method_name为临时性节点
    for (auto &sp : m_serviceMap) {
        // /service_name->/UserServiceRpc
        std::string service_path = "/" + sp.first;
        zkCli.Create(service_path.c_str(), nullptr, 0);
        for (auto &mp : sp.second.m_methodMap) {
            // /service_name/method_name->UserServiceRpc/Login 存储当前这个rpc服务节点主机的ip和port
            std::string method_path = service_path + "/" + mp.first;
            char method_path_data[128] = {0};
            sprintf(method_path_data, "%s:%d", ip.c_str(), port);
            // ZOO_EPHEMERAL表示znode是一个临时性节点
            zkCli.Create(method_path.c_str(), method_path_data, strlen(method_path_data), ZOO_EPHEMERAL);
        }
    }



    //rpc服务端准备启动, 打印信息
    std::cout << "KmrpcProvider start service at ip: " << ip << "port: " << port <<std::endl;
    
    // 启动网络服务
    server.start();
    m_eventLoop.loop();
}

// 新的socket连接回调
void KmrpcProvider::OnConnection(const muduo::net::TcpConnectionPtr &conn) {
    if (!conn->connected()) {
        // 和rpc client的连接断开了
        conn->shutdown();
    }
}
/*
在框架内部，RpcProvider和RpcConsumer协商好之间通信用的protobuf数据类型
service_name method_name args    定义proto的message类型，进行数据头的序列化和反序列化
                                 service_name method_name args_size
16UserServiceLoginzhang san123456   

header_size(4个字节) + header_str + args_str
*/
// 已建立连接用户的读写事件回调,如果远程有一个rpc服务调用请求, 那么OneMessage就会响应
void KmrpcProvider::OneMessage(const muduo::net::TcpConnectionPtr &conn, 
                            muduo::net::Buffer *buf, 
                            muduo::Timestamp) {
    // 网络上接收的远程rpc调用请求的字符流 包含Login args
    std::string recv_buf = buf->retrieveAllAsString();
    
    //从字符流中获取前4个字节的内容,即header_size
    uint32_t header_size = 0;
    recv_buf.copy((char*)&header_size, 4, 0);

    //根据header_size读取数据头的原始字符流, 反序列化数据, 得到rpc请求的详细信息
    std::string rpc_header_str = recv_buf.substr(4, header_size);
    kmrpc::RpcHeader rpcHeader;
    std::string service_name;
    std::string method_name;
    uint32_t args_size;

    if (rpcHeader.ParseFromString(rpc_header_str)) {
        // 数据头反序列化成功
        service_name = rpcHeader.service_name();
        method_name = rpcHeader.method_name();
        args_size = rpcHeader.args_size();
    } else {
        // 数据头反序列化失败
        // std::cout << "rpc_header_str: " << rpc_header_str << std::endl;
        LOG_ERR("rpc_header_str: %s", rpc_header_str.c_str());
        return;
    }

    // 获取rpc方法参数的字符流数据
    std::string args_str = recv_buf.substr(4 + header_size, args_size);

    // 打印调试信息
    std::cout << "============================================" << std::endl;
    std::cout << "header_size: " << header_size << std::endl; 
    std::cout << "rpc_header_str: " << rpc_header_str << std::endl; 
    std::cout << "service_name: " << service_name << std::endl; 
    std::cout << "method_name: " << method_name << std::endl; 
    std::cout << "args_str: " << args_str << std::endl; 
    std::cout << "============================================" << std::endl;

/*
    struct ServerceInfo {
        google::protobuf::Service *m_service; // 保存服务对象
        std::unordered_map<std::string, const google::protobuf::MethodDescriptor*> m_methodMap;// 存储服务方法
    };
    // 存储成功注册的服务对象和其服务方法的所有信息 <name, info>
    std::unordered_map<std::string, ServerceInfo> m_serviceMap;
*/
    // 获取service对象和method对象
    auto it = m_serviceMap.find(service_name);
    if (it == m_serviceMap.end()) { // 没找到
        // std::cout << service_name << " is not exits!" << std::endl;
        LOG_ERR("service_name: %s", service_name.c_str());
        return;
    }

    auto mit = it->second.m_methodMap.find(method_name);
    if (mit == it->second.m_methodMap.end()) { // 没找到
        // std::cout << service_name << " : " << method_name << " is not exist!" << std::endl;
        LOG_ERR("%s: %s is not exist!", service_name.c_str(), method_name.c_str());
        return;
    }

    // 用基类Service抽象获取service对象 new UserService
    google::protobuf::Service *service = it->second.m_service;
    // 用基类MethodDescriptor抽象获取method对象 Login
    const google::protobuf::MethodDescriptor *method = mit->second;

    // 生成rpc方法调用的请求request和response参数
    // service服务对象中的method方法的request参数对象
    google::protobuf::Message *request = service->GetRequestPrototype(method).New();
    if (!request->ParseFromString(args_str)) {
        // 参数解析失败
        // std::cout << "request parse error, content: " << args_str << std::endl;
        LOG_ERR("request parse error, content: %s", args_str.c_str());
        return;
    }
    // service服务对象中的method方法的response参数对象
    google::protobuf::Message *response = service->GetResponsePrototype(method).New();

    // 给下面的method方法的调用, 绑定一个回调函数
    google::protobuf::Closure *done = google::protobuf::NewCallback<KmrpcProvider, 
                                    const muduo::net::TcpConnectionPtr&, google::protobuf::Message*>
                                    (this, &KmrpcProvider::SendRpcResponse, conn, response);


    // 在框架上根据远端rpc请求, 调用当前rpc节点上发布的方法
    // new UserService().Login(controller, request, response, done)
    service->CallMethod(method, nullptr, request, response, done);
}

void KmrpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message* response) {
    std::string response_str;
    if (response->SerializeToString(&response_str)) {
        // 序列化成功后,通过网络把rpc方法执行的结果发送回rpc调用方
        conn->send(response_str);
    } else {
        // std::cout << "serialize response_str error!" << std::endl;
        LOG_ERR("serialize response_str error!");
    }
    conn->shutdown(); // 模拟http短链接服务, 由rpc provider主动断开连接, 为更多rpc连接提供服务
}
