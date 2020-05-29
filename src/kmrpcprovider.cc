#include "kmrpcprovider.h"
#include "kmrpcapplication.h"
#include "rpcheader.pb.h"
#include "logger.h"
#include "zookeeperutil.h"

void KmrpcProvider::NotifyService(google::protobuf::Service *service) {
    ServerceInfo service_info;

    // ��ȡ��������������Ϣ ָ��
    const google::protobuf::ServiceDescriptor *pServiceDesc = service->GetDescriptor();
    // ��ȡ������������
    std::string service_name = pServiceDesc->name();
    // ��ȡ�������ķ�������
    int methodCnt = pServiceDesc->method_count();

    // std::cout << "service_name: " << service_name << std::endl;
    LOG_INFO("service_name: %s", service_name.c_str());

    for (int i = 0; i < methodCnt; ++i) {
        // ��ȡ�������ָ���±���񷽷�������(��������)
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
    // ��ȡ�����ļ�rpcserver����Ϣ
    std::string ip = KmrpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    uint16_t port = atoi(KmrpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());
    muduo::net::InetAddress address(ip, port);

    // ����TcpServer����
    muduo::net::TcpServer server(&m_eventLoop, address, "KmrpcProvider");

    // �����ӻص� ��Ϣ��д�ص�����, ������������ҵ�����
    server.setConnectionCallback(std::bind(&KmrpcProvider::OnConnection, this, std::placeholders::_1));
    server.setMessageCallback(std::bind(&KmrpcProvider::OneMessage, this, 
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    // �����߳̿�����
    server.setThreadNum(4);

    // �ѵ�ǰrpc�ڵ���Ҫ�����ķ���ȫ��ע�ᵽzk��, ��rpc client���Դ�zk���ַ���
    // session timeout 30s ---> zkclient����I/O�̻߳�ÿ��1/3 * timeoutʱ�䷢��ping��Ϣ(����)
    ZkClient zkCli;
    zkCli.Start();
    // service_name Ϊ�����Խڵ�, method_nameΪ��ʱ�Խڵ�
    for (auto &sp : m_serviceMap) {
        // /service_name->/UserServiceRpc
        std::string service_path = "/" + sp.first;
        zkCli.Create(service_path.c_str(), nullptr, 0);
        for (auto &mp : sp.second.m_methodMap) {
            // /service_name/method_name->UserServiceRpc/Login �洢��ǰ���rpc����ڵ�������ip��port
            std::string method_path = service_path + "/" + mp.first;
            char method_path_data[128] = {0};
            sprintf(method_path_data, "%s:%d", ip.c_str(), port);
            // ZOO_EPHEMERAL��ʾznode��һ����ʱ�Խڵ�
            zkCli.Create(method_path.c_str(), method_path_data, strlen(method_path_data), ZOO_EPHEMERAL);
        }
    }



    //rpc�����׼������, ��ӡ��Ϣ
    std::cout << "KmrpcProvider start service at ip: " << ip << "port: " << port <<std::endl;
    
    // �����������
    server.start();
    m_eventLoop.loop();
}

// �µ�socket���ӻص�
void KmrpcProvider::OnConnection(const muduo::net::TcpConnectionPtr &conn) {
    if (!conn->connected()) {
        // ��rpc client�����ӶϿ���
        conn->shutdown();
    }
}
/*
�ڿ���ڲ���RpcProvider��RpcConsumerЭ�̺�֮��ͨ���õ�protobuf��������
service_name method_name args    ����proto��message���ͣ���������ͷ�����л��ͷ����л�
                                 service_name method_name args_size
16UserServiceLoginzhang san123456   

header_size(4���ֽ�) + header_str + args_str
*/
// �ѽ��������û��Ķ�д�¼��ص�,���Զ����һ��rpc�����������, ��ôOneMessage�ͻ���Ӧ
void KmrpcProvider::OneMessage(const muduo::net::TcpConnectionPtr &conn, 
                            muduo::net::Buffer *buf, 
                            muduo::Timestamp) {
    // �����Ͻ��յ�Զ��rpc����������ַ��� ����Login args
    std::string recv_buf = buf->retrieveAllAsString();
    
    //���ַ����л�ȡǰ4���ֽڵ�����,��header_size
    uint32_t header_size = 0;
    recv_buf.copy((char*)&header_size, 4, 0);

    //����header_size��ȡ����ͷ��ԭʼ�ַ���, �����л�����, �õ�rpc�������ϸ��Ϣ
    std::string rpc_header_str = recv_buf.substr(4, header_size);
    kmrpc::RpcHeader rpcHeader;
    std::string service_name;
    std::string method_name;
    uint32_t args_size;

    if (rpcHeader.ParseFromString(rpc_header_str)) {
        // ����ͷ�����л��ɹ�
        service_name = rpcHeader.service_name();
        method_name = rpcHeader.method_name();
        args_size = rpcHeader.args_size();
    } else {
        // ����ͷ�����л�ʧ��
        // std::cout << "rpc_header_str: " << rpc_header_str << std::endl;
        LOG_ERR("rpc_header_str: %s", rpc_header_str.c_str());
        return;
    }

    // ��ȡrpc�����������ַ�������
    std::string args_str = recv_buf.substr(4 + header_size, args_size);

    // ��ӡ������Ϣ
    std::cout << "============================================" << std::endl;
    std::cout << "header_size: " << header_size << std::endl; 
    std::cout << "rpc_header_str: " << rpc_header_str << std::endl; 
    std::cout << "service_name: " << service_name << std::endl; 
    std::cout << "method_name: " << method_name << std::endl; 
    std::cout << "args_str: " << args_str << std::endl; 
    std::cout << "============================================" << std::endl;

/*
    struct ServerceInfo {
        google::protobuf::Service *m_service; // ����������
        std::unordered_map<std::string, const google::protobuf::MethodDescriptor*> m_methodMap;// �洢���񷽷�
    };
    // �洢�ɹ�ע��ķ�����������񷽷���������Ϣ <name, info>
    std::unordered_map<std::string, ServerceInfo> m_serviceMap;
*/
    // ��ȡservice�����method����
    auto it = m_serviceMap.find(service_name);
    if (it == m_serviceMap.end()) { // û�ҵ�
        // std::cout << service_name << " is not exits!" << std::endl;
        LOG_ERR("service_name: %s", service_name.c_str());
        return;
    }

    auto mit = it->second.m_methodMap.find(method_name);
    if (mit == it->second.m_methodMap.end()) { // û�ҵ�
        // std::cout << service_name << " : " << method_name << " is not exist!" << std::endl;
        LOG_ERR("%s: %s is not exist!", service_name.c_str(), method_name.c_str());
        return;
    }

    // �û���Service�����ȡservice���� new UserService
    google::protobuf::Service *service = it->second.m_service;
    // �û���MethodDescriptor�����ȡmethod���� Login
    const google::protobuf::MethodDescriptor *method = mit->second;

    // ����rpc�������õ�����request��response����
    // service��������е�method������request��������
    google::protobuf::Message *request = service->GetRequestPrototype(method).New();
    if (!request->ParseFromString(args_str)) {
        // ��������ʧ��
        // std::cout << "request parse error, content: " << args_str << std::endl;
        LOG_ERR("request parse error, content: %s", args_str.c_str());
        return;
    }
    // service��������е�method������response��������
    google::protobuf::Message *response = service->GetResponsePrototype(method).New();

    // �������method�����ĵ���, ��һ���ص�����
    google::protobuf::Closure *done = google::protobuf::NewCallback<KmrpcProvider, 
                                    const muduo::net::TcpConnectionPtr&, google::protobuf::Message*>
                                    (this, &KmrpcProvider::SendRpcResponse, conn, response);


    // �ڿ���ϸ���Զ��rpc����, ���õ�ǰrpc�ڵ��Ϸ����ķ���
    // new UserService().Login(controller, request, response, done)
    service->CallMethod(method, nullptr, request, response, done);
}

void KmrpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message* response) {
    std::string response_str;
    if (response->SerializeToString(&response_str)) {
        // ���л��ɹ���,ͨ�������rpc����ִ�еĽ�����ͻ�rpc���÷�
        conn->send(response_str);
    } else {
        // std::cout << "serialize response_str error!" << std::endl;
        LOG_ERR("serialize response_str error!");
    }
    conn->shutdown(); // ģ��http�����ӷ���, ��rpc provider�����Ͽ�����, Ϊ����rpc�����ṩ����
}
