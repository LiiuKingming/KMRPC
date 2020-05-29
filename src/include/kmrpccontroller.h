#pragma once
#include <google/protobuf/service.h>
#include <string>

class KmrpcController : public google::protobuf::RpcController {
public:
    KmrpcController();
    void Reset();
    bool Failed() const;
    std::string ErrorText() const;
    void SetFailed(const std::string& reason);

    // δʵ�־��幦��
    void StartCancel();
    bool IsCanceled() const;
    void NotifyOnCancel(google::protobuf::Closure* callback);
private:
    bool m_failed; // RPC����ִ�й����е�״̬
    std::string m_errText; // RPC����ִ�й����еĴ�����Ϣ
};