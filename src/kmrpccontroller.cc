#include "kmrpccontroller.h"

KmrpcController::KmrpcController() {
    m_failed = false;
    m_errText = "";
}

void KmrpcController::Reset() {
    m_failed = false;
    m_errText = "";
}

bool KmrpcController::Failed() const {
    return m_failed;
}

std::string KmrpcController::ErrorText() const {
    return m_errText;
}

void KmrpcController::SetFailed(const std::string& reason) {
    m_failed = true;
    m_errText = reason;
}

// 未实现具体功能
void KmrpcController::StartCancel() {}
bool KmrpcController::IsCanceled() const {return false;}
void KmrpcController::NotifyOnCancel(google::protobuf::Closure* callback) {}