#!/bin/bash

#单引号: 硬引用 不解析$变量 全部当做字符串
#双引号: 软引用 解析$变量 




echo "var = hhahahah"
echo 'var = hahhahah'


var=12345
echo "var = $var"
echo 'var = $var'


exit 0

