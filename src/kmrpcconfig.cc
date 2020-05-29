#include "kmrpcconfig.h"
#include <iostream>
#include <string>

// �������
void KmrpcConfig::LoadConfigFile(const char *config_file) {
    FILE *pf = fopen(config_file, "r");
    if (pf == nullptr) {
        std::cout << config_file << "is note exist!" << std::endl;
        exit(EXIT_FAILURE);
    }

    // 1.ע�� 2.��ȷ��������= 3.ͷβ����ո�
    while(!feof(pf)) {
        char buf[512] = {0};
        fgets(buf, 512, pf);
        
        // ȥ���ַ���ǰ�����Ŀո�
        std::string read_buf(buf);
        Trim(read_buf);

        // �ж�#ע��
        if (read_buf[0] == '#' || read_buf.empty()) {
            continue;
        }

        // ����������
        int idx = read_buf.find('=');
        if (idx == -1) {
            // ���ò��Ϸ�
            continue;
        }

        std::string key;
        std::string value;
        // ����substr�ڶ�������Ϊ����
        key = read_buf.substr(0, idx);
        Trim(value);

        int endidx = read_buf.find('\n', idx);
        value = read_buf.substr(idx + 1, endidx - idx - 1);
        Trim(value);
        m_configMap.insert({key, value});
    }
    fclose(pf);
}

// ��ѯ������Ϣ
std::string KmrpcConfig::Load(const std::string &key) {
    auto it = m_configMap.find(key);
    if (it == m_configMap.end()) {
        return "";
    }
    return it->second;
}

// ȥ���ַ���ǰ��Ŀո�
void KmrpcConfig::Trim(std::string &src_buf) {
    int idx = src_buf.find_first_not_of (' ');
    if (idx != -1) {
        // ˵���ַ���ǰ���пո�
        src_buf = src_buf.substr(idx, src_buf.size() - idx);
    }
    // ȥ���ַ����������Ŀո�
    idx = src_buf.find_last_not_of (' ');
    if (idx != -1) {
        // ˵���ַ��������пո�
        src_buf = src_buf.substr(0, idx + 1);
    }
}