#include "ServerOperation.h"

#include "SecKeyShm.h"


#include <iostream>
#include <pthread.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>


using namespace std;

int ServerOperation::flag = 0;

//线程传递参数类型
typedef struct _pthread_info_t {
	TcpSocket *tcpSocket;
	ServerOperation *s;
}pthread_info_t;

//线程处理函数 与客户端进行数据交互
void *myroute(void *arg)
{
	int ret = -1;
	int i = 0;

	//接收报文相关参数
	char *recvData = NULL;
	int recvLen = 0;

	FactoryCodec *factory = NULL;
	Codec *codec = NULL;
	RequestMsg *pRequestMsg = NULL;


	char *sendData = NULL;
	int sendLen = 0;

	pthread_info_t *pInfo = static_cast<pthread_info_t*>(arg);

	do {


		//1. 接收密钥协商请求报文
		pInfo->tcpSocket->recvMsg(&recvData, recvLen);  //1. free

		//2. 创建密钥协商请求报文工厂类对象
		factory = new RequestFactory;

		//3. 创建密钥协商请求报文解码对象
		codec = factory->createCodec();

		//4. 解码密钥协商请求报文
		pRequestMsg = static_cast<RequestMsg *>(codec->msgDecode(recvData, recvLen));

		//5. 验证消息认证码

		//5.1 创建一个对象
		HMAC_CTX *ctx = NULL;
		ctx = HMAC_CTX_new();
		if (NULL == ctx)
		{
			cout << "HMAC_CTX_new failed.." << endl;
			break;
		}

		//5.2 初始化 添加密钥
		char key[32];
		memset(key, 0, 32);
		sprintf(key, "@%s+%s@", pRequestMsg->serverId, pRequestMsg->clientId);
		ret = HMAC_Init_ex(ctx, (void*)key, strlen(key), EVP_sha256(), NULL);
		if (0 == ret)
		{
			cout << "HMAC_Init_ex failed.. " << endl;
			break;
		}

		//5.3 添加数据
		ret = HMAC_Update(ctx, (unsigned char *)pRequestMsg->r1, strlen(pRequestMsg->r1));
		if (0 == ret)
		{
			cout << "HMAC_Update failed.. " << endl;
			break;
		}

		//5.4 计算结果
		unsigned char md[SHA256_DIGEST_LENGTH];
		unsigned int mdLen = 0;

		memset(md, 0, SHA256_DIGEST_LENGTH);
		ret = HMAC_Final(ctx, md, &mdLen);
		if (0 == ret)
		{
			cout << "HMAC_Final failed.. " << endl;
			break;
		}

		//5.5 转换成字符串
		unsigned char buf[SHA256_DIGEST_LENGTH * 2 + 1];
		for (i = 0; i < SHA256_DIGEST_LENGTH; i++)
		{
			sprintf((char*)&buf[i * 2], "%02X", md[i]);
		}

		cout << "验证消息认证码: " << buf << endl;

		//5.6 释放CTX
		HMAC_CTX_free(ctx);

		//5.7 比较消息认证码
		if (strcmp(pRequestMsg->authCode, (char *)buf) == 0)
		{
			cout << "消息认证码验证ok" << endl;
		}
		else
		{
			cout << "消息认证码不一致" << endl;
			break;
		}



		//6. 根据请求的类型做对应响应
		switch (pRequestMsg->cmdType)
		{
			//密钥协商
		case RequestCodec::NewOrUpdate:
			pInfo->s->secKeyAgree(pRequestMsg, &sendData, sendLen); //2.free
			break;

			//密钥校验
		case RequestCodec::Check:
			pInfo->s->secKeyCheck(pRequestMsg, &sendData, sendLen); //2.free
			break;

			//密钥注销
		case RequestCodec::Revoke:
			pInfo->s->secKeyRevoke(pRequestMsg, &sendData, sendLen); //2.free
			break;

		default:
			break;
		}

		//7. 判断对应的业务是否成功
		if (0 != ret)
		{
			cout << "做对应的业务失败..." << endl;
			break;
		}

		//8. 发送密钥协商响应报文
		pInfo->tcpSocket->sendMsg(sendData, sendLen);

		//9. 断开连接
		pInfo->tcpSocket->disConnect();


	}while (0);

	//10. 释放内存
	if (NULL != pInfo)
	{
		delete pInfo;
	}

	if (NULL != recvData)
	{
		delete recvData;
	}

	if (NULL != factory)
	{
		delete factory;
	}

	if (NULL != codec)
	{
		delete codec;
	}

	if (NULL != sendData)
	{
		delete sendData;
	}

	pthread_exit(NULL);
}

//构造函数
ServerOperation::ServerOperation(ServerInfo * info)
{
	//参数检查
	if (NULL == info)
	{
		cout << "ServerOperation parameter invalid." << endl;
		return;
	}

	//服务端配置信息
	mInfo = new ServerInfo;
	memset(mInfo, 0, sizeof(ServerInfo));
	memcpy(mInfo, info, sizeof(ServerInfo));

	mShm = new SecKeyShm(mInfo->shmkey, mInfo->maxNode);

	mServer = new TcpServer;

}

//析构函数
ServerOperation::~ServerOperation()
{
	delete mInfo;
	delete mShm;
	delete mServer;

	mInfo = NULL;
	mShm = NULL;
	mServer = NULL;
}

//启动服务器
void ServerOperation::startWork()
{
	int ret = -1;

	pthread_t tid = -1;

	pthread_info_t *pInfo = NULL;

	//1. 设置监听
	mServer->setListen(mInfo->sPort);
	cout << "密钥协商服务端处于监听状态..." << endl;

	//2. 循环接受客户端连接 创建线程
	while (1)
	{
        if (1 == flag)
        {
            cout << "密钥协商服务端友好退出.." << endl;
            break; 
        }

		mClient = mServer->acceptConn(3);
		if (NULL == mClient)
		{
			cout << "3 秒超时..." << endl;
			continue;
		}

		pInfo = new pthread_info_t;
		memset(pInfo, 0, sizeof(pthread_info_t));
		pInfo->tcpSocket = mClient;
		pInfo->s = this;

		//创建线程
		pthread_create(&tid, NULL, myroute, (void*)pInfo);

		//设置线程为分离状态
		pthread_detach(tid);

	}

	//3. 关闭服务端
	mServer->closefd();
}

int ServerOperation::secKeyAgree(RequestMsg * reqmsg, char ** outData, int & outLen)
{
	int ret = -1;

	RespondMsg respondMsg;

	NodeShmInfo nodeShmInfo;

	unsigned char md[SHA512_DIGEST_LENGTH];

	SHA512_CTX ctx;

	if (NULL == reqmsg || NULL == outData)
	{
		cout << "secKeyAgree invalid parameter.." << endl;
		return -1;
	}

	//1. 组织密钥协商响应报文
	memset(&respondMsg, 0, sizeof(RespondMsg));
	respondMsg.rv = 0;
	strcpy(respondMsg.clientId, reqmsg->clientId);
	strcpy(respondMsg.serverId, reqmsg->serverId);

	respondMsg.seckeyid = 1;

	//生成随机序列
	getRandString(sizeof(respondMsg.r2), respondMsg.r2);


	//2. 创建密钥协商响应报文工厂类对象
	FactoryCodec *factory = new RespondFactory;

	//3. 创建密钥协商响应报文编码对象
	Codec *codec = factory->createCodec(&respondMsg);

	//4. 编码传出
	codec->msgEncode(outData, outLen);


	memset(&nodeShmInfo, 0, sizeof(NodeShmInfo));
	//5. 生成密钥 SHA512
	//5.1 初始化
	ret = SHA512_Init(&ctx);
	if (0 == ret)
	{
		cout << "SHA512_Init failed.." << endl;
		return -1;
	}

	//5.2 添加数据
	ret = SHA512_Update(&ctx, (void*)reqmsg->r1, strlen(reqmsg->r1));
	if (0 == ret)
	{
		cout << "SHA512_Update failed.." << endl;
		return -1;
	}

	ret = SHA512_Update(&ctx, (void*)respondMsg.r2, strlen(respondMsg.r2));
	if (0 == ret)
	{
		cout << "SHA512_Update failed.." << endl;
		return -1;
	}

	//5.3 计算结果
	memset(md, 0, SHA512_DIGEST_LENGTH);
	ret = SHA512_Final(md, &ctx);
	if (0 == ret)
	{
		cout << "SHA512_Final failed.." << endl;
		return -1;
	}

	//5.4 转化字符串
	for (int i = 0; i < SHA512_DIGEST_LENGTH; i++)
	{
		sprintf((char*)&nodeShmInfo.secKey[i * 2], "%02X", md[i]);
	}

	//6. 写共享内存
	mShm->shmWrite(&nodeShmInfo);

	//7. 写数据库

	//8. 释放内存

	delete factory;
	delete codec;
	factory = NULL;
	codec = NULL;


	return 0;
}

int ServerOperation::secKeyCheck(RequestMsg * reqmsg, char ** outData, int & outLen)
{
	//1. 组织密钥校验响应报文

	//2. 创建密钥响应报文工厂类对象

	//3. 创建密钥响应报文编码对象

	//4. 编码密钥校验响应报文 传出

	//5. 释放内存

	return 0;
}

int ServerOperation::secKeyRevoke(RequestMsg * reqmsg, char ** outData, int & outLen)
{
	//1. 组织密钥注销响应报文

	//2. 创建密钥响应报文工厂类对象

	//3. 创建密钥响应报文编码对象

	//4. 编码密钥注销响应报文 传出

	//5. 将状态设置为0 写共享内存

	//6. 释放内存

	return 0;
}

int ServerOperation::secKeyView(void)
{
	return 0;
}

//随机字符串
void ServerOperation::getRandString(int len, char * randBuf)
{
	int i = 0;
	int tag;

	//参数检查
	if (len <= 0 || NULL == randBuf)
	{
		cout << "getrandString parameter invalid..." << endl;

		return;
	}

	//设置随机种子
	srandom(time(NULL));

	//清零buf
	memset(randBuf, 0, len);

	for (i = 0; i < len; i++)
	{
		//随机字符种类
		tag = random() % 4;
		switch (tag)
		{
			//大写字母
		case 0:
			randBuf[i] = 'A' + random() % 26;
			break;

			//小写字母
		case 1:
			randBuf[i] = 'a' + random() % 26;
			break;

			//数字
		case 2:
			randBuf[i] = '0' + random() % 10;
			break;

			//特殊字符
		case 3:
			randBuf[i] = "~!@#$%^&*()-+"[random() % 13];
			break;
		}
	}
}
