
###
 # @Author: Clark
 # @Email: haixuanwoTxh@gmail.com
 # @Date: 2024-04-13 11:45:42
 # @LastEditors: Clark
 # @LastEditTime: 2024-04-13 11:45:40
 # @Description: file content
###

make clean
make
if [ $? -ne 0 ]; then
    echo "make failed"
    exit 1
fi

echo 1 | sudo -S ./main_client 0 192.168.2.59 6666
