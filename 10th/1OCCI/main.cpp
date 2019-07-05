#include <iostream>
#include <string>
#include <string.h>
#include "OCCIOP.h"
using namespace std;

int main()
{

	OCCIOP op;
	op.connectDB("SECMNG", "SECMNG", "192.168.14.45:1521/orcl");
	int keyid = op.getKeyID();
	cout << "keyid: " << keyid << endl;

	NodeShmInfo shmInfo;
	shmInfo.status = 1;
	shmInfo.seckeyID = keyid;	// 从数据中读出的
	strcpy(shmInfo.clientID, "1111");
	strcpy(shmInfo.seckey, "afkdljdklasjfdksajfskdj");
	strcpy(shmInfo.serverID, "0001");
#if 1
	bool bl = op.writeSecKey(&shmInfo);
	if (!bl)
	{
		cout << "写秘钥失败..." << endl;
	}
	cout << "写密钥到数据库成功..." << endl;

#endif
	op.updataKeyID(keyid + 1);

	op.closeDB();

	return 0;
}
