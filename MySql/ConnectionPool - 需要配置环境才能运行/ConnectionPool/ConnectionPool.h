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
    ConnectionPool(const ConnectionPool& obj) = delete;//删除拷贝构造函数
    ConnectionPool& operator=(const ConnectionPool& obj) = delete;//删除复制运算符重载
    shared_ptr<MysqlConn> getConnection();
    ~ConnectionPool();
private:
    ConnectionPool();//单例模式中的懒汉模式
    bool parseJsonFile();//解析Json配置文件（存储下面9个参数）
    void produceConnection();//生产数据库连接
    void recycleConnection();//销毁数据库连接
    void addConnection();

    string m_ip;
    string m_user;
    string m_passwd;
    string m_dbName;//数据库名
    unsigned short m_port;//数据库端口
    int m_minSize;
    int m_maxSize;
    int m_timeout;//等待超时最大时长
    int m_maxIdleTime;//数据库连接最大空闲时常
    queue<MysqlConn*> m_connectionQ;
    mutex m_mutexQ;//互斥锁
    condition_variable m_cond;//条件变量--生产者消费者模型
};

