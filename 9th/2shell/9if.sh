#!/bin/bash

#-gt 大于  greater than
#-ge 大于或者等于
#-le 小于或者等于
#-lt 小于
#-eq 等于
#-ne 不等于


#等待用户输入一个数字
read -p "please input a number: " n


if [ $n -gt 100 ]
then
    echo "n > 100"
else
    echo "n <= 100"

fi


exit 0
