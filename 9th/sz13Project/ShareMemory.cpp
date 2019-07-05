#include "ShareMemory.h"
#include <cstring>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

//打开共享内存
ShareMemory::ShareMemory(int key)
{
	mShmId = shmget(key, 0, 0);
	if (-1 == mShmId)
	{
		//写日志
	}
}

//打开共享内存
ShareMemory::ShareMemory(char * pathName)
{
	//通过路径名字和项目ID生成一个key
	key_t key = ftok(pathName, 'X');
	if (-1 == key)
	{
		//写日志

		return;
	}

	mShmId = shmget(key, 0, 0);
	if (-1 == mShmId)
	{
		//写日志
	}
}

//创建共享内存
ShareMemory::ShareMemory(int key, int shmSize)
{
	void *addr = NULL;

	//打开共享内存, 如果打开失败就创建
	mShmId = shmget(key, 0, 0);
	if (-1 == mShmId)
	{
		//共享内存不存在就创建
		mShmId = shmget(key, shmSize, IPC_CREAT | 0644);
		if (-1 == mShmId)
		{
			//写日志
		}

		//关联
		addr = mapShm();

		//清零
		memset(addr, 0, shmSize);

		//解除关联
		unMapShm();
	}

}

//创建共享内存
ShareMemory::ShareMemory(char * pathName, int shmSize)
{
	void *addr = NULL;

	key_t key = ftok(pathName, 'X');
	if (-1 == key)
	{
		//写日志

		return;
	}

	//打开共享内存, 如果打开失败就创建
	mShmId = shmget(key, 0, 0);
	if (-1 == mShmId)
	{
		//共享内存不存在就创建
		mShmId = shmget(key, shmSize, IPC_CREAT | 0644);
		if (-1 == mShmId)
		{
			//写日志
		}

		//关联
		addr = mapShm();

		//清零
		memset(addr, 0, shmSize);

		//解除关联
		unMapShm();
	}
}

//析构函数
ShareMemory::~ShareMemory()
{
}

//关联共享内存
void * ShareMemory::mapShm()
{
	//0: 表示默认读写
	mShmAddr = shmat(mShmId, NULL, 0);
	if ((void*)-1 == mShmAddr)
	{
		//写日志
		return NULL;
	}

	return mShmAddr;
}

//取消关联共享内存
int ShareMemory::unMapShm()
{
	int ret = -1;

	ret = shmdt(mShmAddr);
	if (-1 == ret)
	{
		//写日志

		return -1;
	}

	return 0;
}

//删除共享内存
int ShareMemory::delShm()
{
	int ret = -1;

	ret = shmctl(mShmId, IPC_RMID, NULL);
	if (-1 == ret)
	{
		//写日志

		return -1;
	}

	return 0;
}
