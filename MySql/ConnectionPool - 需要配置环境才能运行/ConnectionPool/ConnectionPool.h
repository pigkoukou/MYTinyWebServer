#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include "MysqlConn.h"
using namespace std;
class ConnectionPool
{
public:
    static ConnectionPool* getConnectPool();
    ConnectionPool(const ConnectionPool& obj) = delete;//ɾ���������캯��
    ConnectionPool& operator=(const ConnectionPool& obj) = delete;//ɾ���������������
    shared_ptr<MysqlConn> getConnection();
    ~ConnectionPool();
private:
    ConnectionPool();//����ģʽ�е�����ģʽ
    bool parseJsonFile();//����Json�����ļ����洢����9��������
    void produceConnection();//�������ݿ�����
    void recycleConnection();//�������ݿ�����
    void addConnection();

    string m_ip;
    string m_user;
    string m_passwd;
    string m_dbName;//���ݿ���
    unsigned short m_port;//���ݿ�˿�
    int m_minSize;
    int m_maxSize;
    int m_timeout;//�ȴ���ʱ���ʱ��
    int m_maxIdleTime;//���ݿ�����������ʱ��
    queue<MysqlConn*> m_connectionQ;
    mutex m_mutexQ;//������
    condition_variable m_cond;//��������--������������ģ��
};

