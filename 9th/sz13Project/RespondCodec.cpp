#include "RespondCodec.h"

#include <iostream>

using namespace std;


//创建解码对象
RespondCodec::RespondCodec()
{
	pMsg = new RespondMsg;

	memset(pMsg, 0, sizeof(RespondMsg));
}

//创建一个编码对象
RespondCodec::RespondCodec(RespondMsg * msg)
{
	pMsg = new RespondMsg;


	memcpy(pMsg, msg, sizeof(RespondMsg));
}

//析构函数
RespondCodec::~RespondCodec()
{
	delete pMsg;
}

//编码
int RespondCodec::msgEncode(char ** outData, int & len)
{
	int ret = -1;

	//1. 参数检查
	if (NULL == outData)
	{
		cout << "msgEncode failed..." << endl;
		return -1;
	}

	//2. 编码rv
	ret = writeHeadNode(pMsg->rv);
	if (0 != ret)
	{
		cout << "writeHeadNode failed...." << endl;
		return -1;
	}

	//3. 编码clientID
	ret = writeNextNode(pMsg->clientId, strlen(pMsg->clientId));
	if (0 != ret)
	{
		cout << "writeNextNode failed...." << endl;
		return -1;
	}

	//4. 编码serverId
	ret = writeNextNode(pMsg->serverId, strlen(pMsg->serverId));
	if (0 != ret)
	{
		cout << "writeNextNode failed...." << endl;
		return -1;
	}

	//5. 编码r2
	ret = writeNextNode(pMsg->r2, strlen(pMsg->r2));
	if (0 != ret)
	{
		cout << "writeNextNode failed...." << endl;
		return -1;
	}

	//6. 编码secKeyId
	ret = writeNextNode(pMsg->seckeyid);
	if (0 != ret)
	{
		cout << "writeNextNode failed...." << endl;
		return -1;
	}


	//7. 编码结构体
	ret = packSequence(outData, len);
	if (0 != ret)
	{
		cout << "writeNextNode failed...." << endl;
		return -1;
	}

	//8. 传出

	//9. 释放内存
	freeSequence(NULL);

	return 0;
}

//解码
void * RespondCodec::msgDecode(char * inData, int inLen)
{
	int ret = -1;

	//1. 参数检查
	if (NULL == inData || inLen <= 0)
	{
		cout << "invalid parameter...." << endl;
		return NULL;
	}

	//2. 解码结构体
	ret = unpackSequence(inData, inLen);
	if (0 != ret)
	{
		cout << "unpackSequence failed..." << endl;
		return NULL;
	}

	//3. 解码rv
	ret = readHeadNode(pMsg->rv);
	if (0 != ret)
	{
		cout << "readHeadNode failed..." << endl;
		return NULL;
	}

	//4. 解码clientId
	ret = readNextNode(pMsg->clientId);
	if (0 != ret)
	{
		cout << "readNextNode failed..." << endl;
		return NULL;
	}


	//5. 解码serverId
	ret = readNextNode(pMsg->serverId);
	if (0 != ret)
	{
		cout << "readNextNode failed..." << endl;
		return NULL;
	}


	//6. 解码r2
	ret = readNextNode(pMsg->r2);
	if (0 != ret)
	{
		cout << "readNextNode failed..." << endl;
		return NULL;
	}

	//7. 解码seckeyid
	ret = readNextNode(pMsg->seckeyid);
	if (0 != ret)
	{
		cout << "readNextNode failed..." << endl;
		return NULL;
	}

	//8. 传出

	//9. 释放内存
	freeSequence(NULL);

	return pMsg;
}

//释放内存
int RespondCodec::msgMemFree(void ** data)
{
	//参数检查
	if (NULL == data || NULL == *data)
	{
		cout << "invalid parameter ..." << endl;
		return -1;
	}

	//释放内存
	delete (RespondMsg*)*data;
	*data = NULL;

	return 0;
}

