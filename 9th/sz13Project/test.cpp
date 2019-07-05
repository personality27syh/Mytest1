#define _CRT_SECURE_NO_WARNINGS

#include "RequestFactory.h"
#include "RespondFactory.h"

#include "ServerOperation.h"
#include <iostream>
#include <cstring>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <signal.h>



using namespace std;


void fun(int signo)
{
    cout << " 捕捉到信号: " << signo << endl;

    ServerOperation::flag = 1;

}


//创建一个守护进程
int createDaemon(void)
{
    int ret = -1;

    pid_t pid = -1;

    //1. 创建子进程 父进程退出
    pid = fork();
    if (-1 == pid)
    {
        cout << "fork failed.." << endl;
        return -1;
    }
    else if (pid > 0)
    {
        //父进程退出
        exit(0); 
    }

    //2. 创建会话
    ret = setsid();
    if (-1 == ret)
    {
        cout << "setsid failed.." << endl;
        return -1;
    }


    //3. 更改工作目录

    //4. 修改权限掩码
    umask(0);

    //5. 关闭文件描述符

    //6. 周期性执行任务

    return 0;
}



int main()
{
	int ret = -1;

    struct sigaction act;
	
	//1. 服务端配置信息
	ServerInfo info;
	memset(&info, 0, sizeof(ServerInfo));

	//服务端ID
	strcpy(info.serverID, "0001");
	//数据库用户名
	strcpy(info.dbUser, "SECMNG");
	//数据库密码
	strcpy(info.dbPasswd, "SECMNG");
	//数据库实例
	strcpy(info.dbSID, "orcl");
	//端口
	info.sPort = 10086;
	//最大网点数
	info.maxNode = 10;
	//共享内存key
	info.shmkey = 0x11;


    //注册信号
    memset(&act, 0, sizeof act);
    act.sa_handler = fun;
    act.sa_flags = 0;
    ret = sigaction(SIGUSR1, &act, NULL);
    if (-1 == ret)
    {
        cout << "sigaction failed.." << endl;
        return 1;
    }


	//2. 创建密钥协商服务端对象
	ServerOperation server(&info);

    //成为守护进程
    createDaemon();

	//3. 启动服务端
	server.startWork();

	cout << "hello itcast" << endl;

	return 0;
}

