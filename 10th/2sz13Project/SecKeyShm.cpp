#include "SecKeyShm.h"
#include <cstring>

#include <iostream>

using namespace std;

//打开共享内存
SecKeyShm::SecKeyShm(int key):ShareMemory(key)
{
	mMaxNode = 10;
}

//创建共享内存
SecKeyShm::SecKeyShm(int key, int maxNode):ShareMemory(key, maxNode * sizeof(NodeShmInfo)),mMaxNode(maxNode)
{

}

//打开共享内存
SecKeyShm::SecKeyShm(char * pathName):ShareMemory(pathName)
{
	mMaxNode = 10;
}

//创建共享内存
SecKeyShm::SecKeyShm(char * pathName, int maxNode):ShareMemory(pathName, maxNode * sizeof(NodeShmInfo)), mMaxNode(maxNode)
{

}

SecKeyShm::~SecKeyShm()
{

}

//写共享内存
//如果共享内存中存在对应网点密钥信息, 就直接覆盖
//如果共享内存不存在对应网点密钥信息, 找一个空白的区域写网点密钥信息
int SecKeyShm::shmWrite(NodeShmInfo * pNodeInfo)
{
	int i = 0;
	void *addr = NULL;

	NodeShmInfo *tmp = NULL;
	NodeShmInfo emptyNode;

	//参数检查
	if (NULL == pNodeInfo)
	{
		cout << "parameter is invalid" << endl;
		return -1;
	}

	//关联共享内存
	addr = mapShm();

	//tmp = static_cast<NodeShmInfo*>(addr);

	//遍历共享内存, 查看共享内存中是否有对应的网点密钥信息
	for (i = 0; i < mMaxNode; i++)
	{
		tmp = static_cast<NodeShmInfo*>(addr) + i;
		if ((strcmp(tmp->clientId, pNodeInfo->clientId) == 0)
			&& (strcmp(tmp->serverId, pNodeInfo->serverId) == 0))
		{
			cout << "共享内存中存在对应的网点密钥信息, 现在就覆盖原来的信息" << endl;
			memcpy(tmp, pNodeInfo, sizeof(NodeShmInfo));
			break;
		}
	}

	//遍历共享内存, 寻找一个空白区域, 将网点密钥信息写入共享内存
	if (i == mMaxNode)
	{
		//清零操作
		memset(&emptyNode, 0, sizeof(NodeShmInfo));
		for (i = 0; i < mMaxNode; i++)
		{
			tmp = static_cast<NodeShmInfo*>(addr) + i;
			if (memcmp(tmp, &emptyNode, sizeof(NodeShmInfo)) == 0)
			{
				cout << "找到了空白的区域, 将网点密钥信息写入共享内存" << endl;
				memcpy(tmp, pNodeInfo, sizeof(NodeShmInfo));
				break;
			}
		}

		//共享内存中没有空白区域
		if (i == mMaxNode)
		{
			cout << "请联系管理员...." << endl;
		}
	}

	//取消关联
	unMapShm();

	return 0;
}

//从共享内存中读取网点密钥信息
int SecKeyShm::shmRead(const char * clientId, const char * serverId, NodeShmInfo * pNodeInfo)
{
	int i = 0;
	void *addr = NULL;

	NodeShmInfo *tmp = NULL;


	//参数检查
	if (NULL == clientId || NULL == serverId || NULL == pNodeInfo)
	{
		cout << "some parameters are NULL" << endl;
		return -1;
	}

	//关联共享内存
	addr = mapShm();

	//遍历共享内存, 是否存在对应网点密钥信息
	for (i = 0; i < mMaxNode; i++)
	{
		tmp = static_cast<NodeShmInfo*>(addr) + i;
		if ((strcmp(tmp->clientId, clientId) == 0) && (strcmp(tmp->serverId, serverId) == 0))
		{
			cout << "找到对应的网点密钥信息" << endl;
			memcpy(pNodeInfo, tmp, sizeof(NodeShmInfo));
			break;
		}
	}

	if (i == mMaxNode)
	{
		cout << "共享内存中不存在对应的网点密钥信息" << endl;
		return -1;
	}

	//取消关联共享内存
	unMapShm();


	return 0;
}
