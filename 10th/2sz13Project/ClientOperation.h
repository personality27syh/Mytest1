#pragma once
#include "TcpSocket.h"
#include "SecKeyShm.h"

//密钥协商客户端配置信息
class ClientInfo
{
public:
	char clientID[12];			// 客户端ID
	char serverID[12];			// 服务器ID
	char authCode[65];			// 消息认证码
	char serverIP[32];			// 服务器IP
	unsigned int serverPort;	// 服务器端口
	int maxNode;				// 共享内存节点个数  客户端默认1
	int shmKey;					// 共享内存的Key
};

//密钥协商客户端业务类
class ClientOperation
{
public:
	ClientOperation(ClientInfo *info);
	~ClientOperation();

	// 秘钥协商
	int secKeyAgree();
	// 秘钥校验
	int secKeyCheck();
	// 秘钥注销
	int secKeyRevoke();
	// 秘钥查看
	int secKeyView();

private:
	void getRandString(int len, char* randBuf);

private:
	ClientInfo *mInfo;
	TcpSocket *mSocket;
	SecKeyShm* mShm;
};

