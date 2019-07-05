#!/bin/bash

#获取当前用户
USER=$(whoami)

#获取对应进程PID
PID=$(ps -u $USER | grep keyMngServer | awk '{print $1}')

#echo "PID = $PID"


#判断密钥协商服务端是否启动
if [ ! -z $PID ]
then
    kill -10 $PID
else
    echo "密钥协商服务端没有启动"
fi

exit 0



