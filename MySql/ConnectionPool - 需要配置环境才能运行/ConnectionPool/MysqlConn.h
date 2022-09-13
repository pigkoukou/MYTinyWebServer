#pragma once
#include <iostream>
#include <mysql.h>
#include <chrono>
using namespace std;
using namespace chrono;
class MysqlConn
{
public:
    // 初始化数据库连接
    MysqlConn();
    // 释放数据库连接
    ~MysqlConn();
    // 连接数据库
    bool connect(string user, string passwd, string dbName, string ip, unsigned short port = 3306);
    // 更新数据库: insert, update, delete
    bool update(string sql);
    // 查询数据库
    bool query(string sql);
    // 遍历查询得到的结果集
    bool next();
    // 得到结果集中的字段值
    string value(int index);
    // 事务操作-新建事务设置手动提交
    bool transaction();
    // 提交事务
    bool commit();
    // 事务回滚 
    bool rollback();
    // 刷新起始的空闲时间点
    void refreshAliveTime();
    // 计算连接存活的总时长
    long long getAliveTime();
private:
    void freeResult();
    MYSQL* m_conn = nullptr;//数据库对象
    MYSQL_RES* m_result = nullptr;//结果集
    MYSQL_ROW m_row = nullptr;//指向结果集每一行的二级指针
    steady_clock::time_point m_alivetime;//绝对时钟
};

