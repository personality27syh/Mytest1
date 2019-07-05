#!/bin/bash


#shell中变量是弱类型

var=1
echo $var
echo ${var}

#如果字符串中没有空格 制表符 可以不用双引号和单引号
var=hello
echo $var
echo ${var}

var="hello itcast"
echo $var
echo ${var}

var=3.1415926
echo $var
echo ${var}

var=aaa
echo "var = ${var}bb"
echo "var = $varbb"


#退出进程
exit 0


