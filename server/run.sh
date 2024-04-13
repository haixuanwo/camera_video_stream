
###
 # @Author: Clark
 # @Email: haixuanwoTxh@gmail.com
 # @Date: 2024-04-08 18:19:18
 # @LastEditors: Clark
 # @LastEditTime: 2024-04-09 11:44:00
 # @Description: file content
###

export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:`pwd`/lib/opencv

make clean
make
if [ $? -ne 0 ]; then
    echo "make failed"
    exit 1
fi

./main_server 6666
