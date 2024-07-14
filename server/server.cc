
#include "../common/codec.h"
#include "muduo/base/Logging.h"
#include "muduo/base/Mutex.h"
#include "muduo/net/EventLoop.h"
#include "muduo/net/TcpServer.h"

#include "cameraapp.h"
#include "../common/common.h"

#include <set>
#include <stdio.h>
#include <unistd.h>
#include <thread>

using namespace muduo;
using namespace muduo::net;

class ChatServer : noncopyable
{
 public:
  ChatServer(EventLoop* loop,
             const InetAddress& listenAddr)
  : server_(loop, listenAddr, "ChatServer"),
    codec_(std::bind(&ChatServer::onStringMessage, this, _1, _2, _3))
  {
    server_.setConnectionCallback(
        std::bind(&ChatServer::onConnection, this, _1));
    server_.setMessageCallback(
        std::bind(&LengthHeaderCodec::onMessage, &codec_, _1, _2, _3));

    sendThread = std::make_shared<std::thread>(std::bind(&ChatServer::sendThreadFunc, this));
  }

  void start()
  {
    server_.start();
  }

 private:
  void onConnection(const TcpConnectionPtr& conn)
  {
    LOG_INFO << conn->peerAddress().toIpPort() << " -> "
             << conn->localAddress().toIpPort() << " is "
             << (conn->connected() ? "UP" : "DOWN");

    if (conn->connected())
    {
      connections_.insert(conn);
    }
    else
    {
      connections_.erase(conn);
    }
  }

  void onStringMessage(const TcpConnectionPtr&,
                       const string& message,
                       Timestamp)
  {
    // for (ConnectionList::iterator it = connections_.begin();
    //     it != connections_.end();
    //     ++it)
    // {
    //     //   codec_.send(get_pointer(*it), message);
    //     LOG_INFO << "recv size: " << message.size() << " to " << (*it)->peerAddress().toIpPort();
    // }

    // displayImage->push(reinterpret_cast<const uint8_t*>(message.data()), message.size());
  }

    static void sendThreadFunc(void *arg)
    {
        ChatServer* server = static_cast<ChatServer*>(arg);

        if (cam_init(0, WIDTH, HEIGHT) < 0)
        {
            LOG_INFO << "cam init fail";
            return;
        }
        camera_streamon();

        int frameLen = -1;
        char *buf = new char[WIDTH*HEIGHT*3/2];
        while (1)
        {
            if (camera_capture(buf, &frameLen) < 0)
            {
                std::this_thread::yield();
                continue;
            }

            for (ConnectionList::iterator it = server->connections_.begin();
                it != server->connections_.end();
                ++it)
            {
                server->codec_.send(get_pointer(*it), std::string(buf, frameLen));
                // LOG_INFO << "send size: " << frameLen << " to " << (*it)->peerAddress().toIpPort();
            }
        }
    }

  typedef std::set<TcpConnectionPtr> ConnectionList;
  TcpServer server_;
  LengthHeaderCodec codec_;
  ConnectionList connections_;
  std::shared_ptr<std::thread> sendThread;
};

int main(int argc, char* argv[])
{
  LOG_INFO << "pid = " << getpid();
  if (argc > 1)
  {
    EventLoop loop;
    uint16_t port = static_cast<uint16_t>(atoi(argv[1]));
    InetAddress serverAddr(port);
    ChatServer server(&loop, serverAddr);
    server.start();
    loop.loop();
  }
  else
  {
    printf("Usage: %s port\n", argv[0]);
  }
}
