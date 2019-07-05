#include "ClientOperation.h"
#include "TcpSocket.h"
#include "SecKeyShm.h"
#include "RequestCodec.h"
#include "RequestFactory.h"
#include "RespondCodec.h"
#include "RespondFactory.h"

#include <iostream>
#include <cstring>
#include <openssl/hmac.h>
#include <openssl/sha.h>

using namespace std;

//构造函数
ClientOperation::ClientOperation(ClientInfo * info)
{
	//参数检查
	if (NULL == info)
	{
		cout << "构造函数参数非法" << endl;

		return;
	}

	//客户端配置信息
	mInfo = new ClientInfo;
	memcpy(mInfo, info, sizeof(ClientInfo));

	mSocket = new TcpSocket;

	mShm = new SecKeyShm(mInfo->shmKey, mInfo->maxNode);
}

//析构函数
ClientOperation::~ClientOperation()
{
	delete mInfo;
	delete mSocket;
	delete mShm;

	mInfo = NULL;
	mSocket = NULL;
	mShm = NULL;
}

//密钥协商
int ClientOperation::secKeyAgree()
{
	int ret = -1;
	int i = 0;

	char key[32];

	unsigned char md[SHA256_DIGEST_LENGTH];
	unsigned int mdLen = 0;

	RequestMsg requestMsg;


	//1. 组织密钥协商请求报文
	memset(&requestMsg, 0, sizeof(RequestMsg));
	requestMsg.cmdType = RequestCodec::NewOrUpdate;

	strcpy(requestMsg.clientId, mInfo->clientID);
	strcpy(requestMsg.serverId, mInfo->serverID);

	getRandString(sizeof(requestMsg.r1), requestMsg.r1);

	//生成消息认证码
	//1.1 初始化
	HMAC_CTX *ctx = NULL;
	ctx = HMAC_CTX_new();
	if (NULL == ctx)
	{
		cout << "HMAC_CTX_new failed.." << endl;
		return 1;
	}

	//添加密钥信息 
	memset(key, 0, 32);
	sprintf(key, "@%s+%s@", mInfo->serverID, mInfo->clientID);
	ret = HMAC_Init_ex(ctx, key, strlen(key), EVP_sha256(), NULL);
	if (0 == ret)
	{
		cout << "HMAC_Init_ex failed.." << endl;
		return 1;
	}

	//1.2 添加数据
	ret = HMAC_Update(ctx, (unsigned char *)requestMsg.r1, sizeof(requestMsg.r1));
	if (0 == ret)
	{
		cout << "HMAC_Update failed.." << endl;
		return 1;
	}

	//1.3 获取结果
	memset(md, 0, SHA256_DIGEST_LENGTH + 1);
	ret = HMAC_Final(ctx, md, &mdLen);
	if (0 == ret)
	{
		cout << "HMAC_Final failed.." << endl;
		return 1;
	}

	memset(requestMsg.authCode, 0, sizeof(requestMsg.authCode));
	for (i = 0; i < SHA256_DIGEST_LENGTH; i++)
	{
		sprintf((char *)&requestMsg.authCode[i * 2], "%02x", md[i]);
	}

	cout << "HMAC: " << requestMsg.authCode << endl;

	//1.4  释放对象
	HMAC_CTX_free(ctx);


	//2. 创建密钥协商请求报文工厂类对象
	FactoryCodec *factory = new RequestFactory;

	//3. 创建密钥协商请求报文编解码类对象
	Codec *codec = factory->createCodec(&requestMsg);

	//4. 编码请求报文
	char *sendData = NULL;
	int sendLen = 0;
	codec->msgEncode(&sendData, sendLen);

	//5. 发送数据
	mSocket->sendMsg(sendData, sendLen);

	//6. 接收数据
	char *recvData = NULL;
	int recvLen = 0;
	mSocket->recvMsg(&recvData, recvLen);

	delete factory;
	delete codec;
	delete sendData;

	//7. 创建密钥协商响应报文工厂类对象
	factory = new RespondFactory;

	//8. 创建密钥协商响应报文解码对象
	codec = factory->createCodec();

	//9. 解码密钥协商响应报文
	RespondMsg *pRespondMsg = static_cast<RespondMsg*>(codec->msgDecode(recvData, recvLen));

	//10. 判断响应状态
	if (pRespondMsg->rv != 0)
	{
		cout << "秘钥协商响应失败..." << endl;
		return -1;
	}

	//11. 生成密钥 哈希算法模拟
	//11.1 初始化
	SHA512_CTX shaCtx;
	ret = SHA512_Init(&shaCtx);
	if (0 == ret)
	{
		cout << "SHA512_Init failed ..." << endl;
		return -1;
	}

	//11.2 添加数据
	ret = SHA512_Update(&shaCtx, (void*)requestMsg.r1, sizeof(requestMsg.r1));
	if (0 == ret)
	{
		cout << "SHA512_Update failed ..." << endl;
		return -1;
	}

	ret = SHA512_Update(&shaCtx, (void*)pRespondMsg->r2, sizeof(pRespondMsg->r2));
	if (0 == ret)
	{
		cout << "SHA512_Update failed ..." << endl;
		return -1;
	}

	//11.3 计算结果
	unsigned char shaMd[SHA512_DIGEST_LENGTH];
	memset(shaMd, 0, SHA512_DIGEST_LENGTH);
	ret = SHA512_Final(shaMd, &shaCtx);
	if (0 == ret)
	{
		cout << "SHA512_Final failed ..." << endl;
		return -1;
	}

	NodeShmInfo nodeShmInfo;
	memset(&nodeShmInfo, 0, sizeof(NodeShmInfo));

	//11.4 转化为字符串
	for (i = 0; i < SHA512_DIGEST_LENGTH; i++)
	{
		sprintf(&nodeShmInfo.secKey[i * 2], "%02X", shaMd[i]);
	}

	
	//12. 写共享内存
	nodeShmInfo.status = 1;
	nodeShmInfo.seckeyId = pRespondMsg->seckeyid;
	strcpy(nodeShmInfo.clientId, requestMsg.clientId);
	strcpy(nodeShmInfo.serverId, requestMsg.serverId);

	mShm->shmWrite(&nodeShmInfo);

	//13. 释放内存
	delete recvData;

	delete factory;
	delete codec;

	return 0;
}

//密钥校验
int ClientOperation::secKeyCheck()
{
	//1. 组织密钥校验报文  cmdType

	//2. 计算密钥信息的哈希值 存储到r1

	//3. 生成消息认证码

	//4. 创建密钥校验请求报文工厂类对象

	//5. 创建密钥校验请求报文编码对象

	//6. 编码请求报文

	//7 连接到密钥服务端

	//8 发送密钥校验请求报文

	//9 接收密钥校验响应报文

	//10 创建密钥响应报文工厂类对象

	//11. 创建密钥响应报文解码对象

	//12. 解码密钥校验响应报文

	//13. 判断校验结果rv

	//14. 断开连接

	//15. 释放内存

	return 0;
}

//密钥注销
int ClientOperation::secKeyRevoke()
{
	//1. 组织密钥注销报文  cmdType

	//2. 生成消息认证码

	//3. 创建密钥注销请求报文工厂类对象

	//4. 创建密钥注销请求报文编码对象

	//5. 编码请求报文

	//6 连接到密钥服务端

	//7 发送密钥注销请求报文

	//8 接收密钥注销响应报文

	//9 创建密钥响应报文工厂类对象

	//10. 创建密钥响应报文解码对象

	//11. 解码密钥注销响应报文

	//12. 判断注销结果rv

	//13. 清空共享内存 shmWrite();
	//status = 0; //表示密钥无效

	//14. 断开连接

	//15. 释放内存


	return 0;
}

//密钥查看
int ClientOperation::secKeyView()
{
	int ret = -1;

	//从共享内存中读取密钥信息
	NodeShmInfo nodeShmInfo;

	memset(&nodeShmInfo, 0, sizeof(NodeShmInfo));

	//从共享内存中读取密钥信息
	ret = mShm->shmRead(mInfo->clientID, mInfo->serverID, &nodeShmInfo);
	if (-1 == ret)
	{
		cout << "共享内存中不存在对应的网点密钥信息" << endl;
	}
	else
	{
		cout << "status: " << nodeShmInfo.status << endl;
		cout << "seckeyId: " << nodeShmInfo.seckeyId << endl;
		cout << "clientId: " << nodeShmInfo.clientId << endl;
		cout << "serverId: " << nodeShmInfo.serverId << endl;
		cout << "secKey: " << nodeShmInfo.secKey << endl;
	}

	return 0;
}

//获取随机字符串
void ClientOperation::getRandString(int len, char * randBuf)
{
	//字符种类  4类
	//大写字母
	//小写字母
	//数字
	//特殊字符

	//字符种类
	int tag;
	int i = 0;

	//参数检查
	if (len <= 0 || NULL == randBuf)
	{
		cout << "getRandString 参数非法" << endl;
		return;
	}

	//设置随机种子
	srandom(time(NULL));

	memset(randBuf, 0, len);

	for (i = 0; i < len; i++)
	{
		//随机字符种类
		tag = random() % 4;

		switch (tag)
		{
			//大写字母
		case 0:
			randBuf[i] = random() % 26 + 'A';
			break;

			//小写字母
		case 1:
			randBuf[i] = random() % 26 + 'a';
			break;

			//数字
		case 2:
			randBuf[i] = random() % 10 + '0';
			break;

			//特殊字符
		case 3:
			randBuf[i] = "~!@#$%^&*()-+"[random() % 13];
			break;
		}
	}
}
