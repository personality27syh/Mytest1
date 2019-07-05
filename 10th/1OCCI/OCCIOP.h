#pragma once
#include <string>
#include <occi.h>
using namespace std;
using namespace oracle::occi;


class NodeShmInfo
{
public:
	int status;
	int seckeyID;
	char clientID[12];
	char serverID[12];
	char seckey[128];
};

class OCCIOP
{
public:
	OCCIOP();
	~OCCIOP();

	// 初始化环境连接数据库
	bool connectDB(string user, string passwd, string connstr);
	// 得到keyID
	int getKeyID();
	bool updataKeyID(int keyID);
	bool writeSecKey(NodeShmInfo *pNode);
	void closeDB();

private:
	// 获取当前时间, 并格式化为字符串
	string getCurTime();

private:
	Environment* m_env;
	Connection* m_conn;
	
};

