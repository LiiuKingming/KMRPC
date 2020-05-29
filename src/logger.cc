#include "logger.h"
#include <time.h>
#include <iostream>

// ��ȡ��־�ĵ���
Logger& Logger::GetInstance() {
    static Logger logger;
    return logger;
}

Logger::Logger() {
    std::thread writeLogTask([&](){
        for (;;) {
            // ��ȡ��ǰ������, ȡ��־��Ϣ, д����Ӧ����־�ļ��� a+
            time_t now = time(nullptr);
            tm *nowtm = localtime(&now);

            char file_name[128];
            sprintf(file_name, "%d-%d-%d-log.txt", nowtm->tm_year + 1900, 
                                                    nowtm->tm_mon + 1,
                                                    nowtm->tm_mday);
            FILE *pf = fopen(file_name, "a+");
            if (pf == nullptr) {
                std::cout << "logger file : " << file_name << "open error!" << std::endl;
                exit(EXIT_FAILURE);
            }

            std::string msg = m_lckQue.Pop();

            char time_buf[128] = {0};
            sprintf(time_buf, "%d:%d:%d => [%s]",
                    nowtm->tm_hour,
                    nowtm->tm_min, 
                    nowtm->tm_sec,
                    (m_loglevel == INFO ? "info" : "error"));
            msg.insert(0, time_buf);
            msg.append("\n");

            fputs(msg.c_str(), pf);
            fclose(pf);
        }
    });
    // �����߳�
    writeLogTask.detach();
}

// ������־����
void Logger::SetLogLevel(LogLevel level) {
    m_loglevel = level;
}

// д��־, ����־��Ϣд��lockqueue����������
void Logger::Log(std::string msg) {
    m_lckQue.Push(msg);
}