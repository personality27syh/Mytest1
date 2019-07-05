#include "RequestCodec.h"

#include <iostream>

using namespace std;

//构造解码对象
RequestCodec::RequestCodec()
{
	mRequestMsg = new RequestMsg;

	memset(mRequestMsg, 0, sizeof(RequestMsg));
}

//构造编码对象
RequestCodec::RequestCodec(RequestMsg * msg)
{
	mRequestMsg = new RequestMsg;

	memcpy(mRequestMsg, msg, sizeof(RequestMsg));
}

//析构函数
RequestCodec::~RequestCodec()
{
	delete mRequestMsg;
}

//请求报文编码
int RequestCodec::msgEncode(char ** outData, int & len)
{
	int ret = -1;

	//参数出错判断
	if (NULL == outData)
	{
		cout << "invalid parameter ..." << endl;
		return -1;
	}

	//1. 编码cmdType
	ret = writeHeadNode(mRequestMsg->cmdType);
	if (0 != ret)
	{
		cout << "writeHeadNode failed.." << endl;
		return -1;
	}

	//2. 编码clientId
	ret = writeNextNode(mRequestMsg->clientId, strlen(mRequestMsg->clientId));
	if (0 != ret)
	{
		cout << "writeNextNode failed.." << endl;
		return -1;
	}

	//3. 编码authCode
	ret = writeNextNode(mRequestMsg->authCode, strlen(mRequestMsg->authCode));
	if (0 != ret)
	{
		cout << "writeNextNode failed.." << endl;
		return -1;
	}

	//4. 编码serverId
	ret = writeNextNode(mRequestMsg->serverId, strlen(mRequestMsg->serverId));
	if (0 != ret)
	{
		cout << "writeNextNode failed.." << endl;
		return -1;
	}

	//5. 编码r1
	ret = writeNextNode(mRequestMsg->r1, strlen(mRequestMsg->r1));
	if (0 != ret)
	{
		cout << "writeNextNode failed.." << endl;
		return -1;
	}

	//6. 编码结构体
	ret = packSequence(outData, len);
	if (0 != ret)
	{
		cout << "packSequence failed.." << endl;
		return -1;
	}

	//7. 传出

	//8. 释放内存
	freeSequence(NULL);

	return 0;
}

//请求报文解码
void * RequestCodec::msgDecode(char * inData, int inLen)
{
	int ret = -1;

	//1. 解码结构体
	ret = unpackSequence(inData, inLen);
	if (0 != ret)
	{
		cout << "unpackSequence failed ..." << endl;

		return NULL;
	}

	//2. 解码cmdType
	ret = readHeadNode(mRequestMsg->cmdType);
	if (0 != ret)
	{
		cout << "readHeadNode failed ..." << endl;

		return NULL;
	}


	//3. 解码clientId
	ret = readNextNode(mRequestMsg->clientId);
	if (0 != ret)
	{
		cout << "readHeadNode failed ..." << endl;

		return NULL;
	}

	//4. 解码authCode
	ret = readNextNode(mRequestMsg->authCode);
	if (0 != ret)
	{
		cout << "readHeadNode failed ..." << endl;

		return NULL;
	}

	//5. 解码serverId
	ret = readNextNode(mRequestMsg->serverId);
	if (0 != ret)
	{
		cout << "readHeadNode failed ..." << endl;

		return NULL;
	}

	//6. 解码r1
	ret = readNextNode(mRequestMsg->r1);
	if (0 != ret)
	{
		cout << "readHeadNode failed ..." << endl;

		return NULL;
	}

	//7. 传出

	//8. 释放内存
	freeSequence(NULL);
	
	return mRequestMsg;
}

//释放请求报文结构体内存
int RequestCodec::MsgMemFree(void ** point)
{
	RequestMsg *pMsg = static_cast<RequestMsg*>(*point);

	delete pMsg;

	*point = NULL;

	return 0;
}
