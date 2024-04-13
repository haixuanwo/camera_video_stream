<!--
 * @Author: Clark
 * @Email: haixuanwoTxh@gmail.com
 * @Date: 2024-04-09 15:08:26
 * @LastEditors: Clark
 * @LastEditTime: 2024-04-13 14:23:17
 * @Description: file content
-->

# 环境准备
- uvc camera连接到client端
- client与server端在同一网络

# client
cd client;./run.sh

- 脚本中执行 make：make clean;make
- 脚本中执行：sudo ./main_client 0 127.0.0.1 6666
第一个参数为/dev/video0
第二个参数为server端ip地址
第三个参数为server端端口号

# server
cd server;./run.sh

- 脚本中执行：make clean;make
- 脚本中执行：./main_server 6666
参数为server端端口号

