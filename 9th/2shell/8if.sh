#!/bin/bash

#[] 是一个命令
#[ 右边必须要有空格
#] 左边必须要有空额
#$1 第一个传入参数
if [ -d $1 ]
then
    echo "$1 is directory"
else
    echo "$1 is other file type"
fi

exit 0

