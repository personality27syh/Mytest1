#pragma once

//共享内存基类
class ShareMemory
{
public:
	//打开共享内存
	ShareMemory(int key);
	//打开共享内存
	ShareMemory(char *pathName);
	//创建共享内存
	ShareMemory(int key, int shmSize);
	//创建共享内存
	ShareMemory(char *pathName, int shmSize);

	//析构函数
	virtual ~ShareMemory();

	//关联共享内存
	void *mapShm();

	//解除关联
	int unMapShm();

	//删除共享内存
	int delShm();

private:
	int mShmId;
	void *mShmAddr;
};

