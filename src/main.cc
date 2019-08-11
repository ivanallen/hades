#include <muduo/net/TcpServer.h>
#include <muduo/base/Logging.h>
#include <muduo/base/Thread.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>

#include <utility>

#include <stdio.h>
#include <unistd.h>

using namespace muduo;
using namespace muduo::net;

namespace hades {

class MainServer {
public:
    MainServer(EventLoop* loop, const InetAddress& listenAddr)
        : loop_(loop),
        server_(loop, listenAddr, "MainServer") {
        server_.setMessageCallback(
                std::bind(&MainServer::onConnection, this, _1));
        server_.setThreadNum(0);
    }

    void start() {
        server_.start();
    }
    void stop();

private:
    void onConnection(const TcpConnectionPtr& conn) {
        LOG_TRACE << conn->peerAddress().toIpPort() << " -> "
            << conn->localAddress().toIpPort() << " is "
            << (conn->connected() ? "UP" : "DOWN");
        LOG_INFO << conn->getTcpInfoString();
    }

    EventLoop* loop_;
    TcpServer server_;
};

} // namespace hades

int main(int argc, char* argv[]) {
    LOG_INFO << "pid = " << getpid() << ", tid = " << CurrentThread::tid();
    LOG_INFO << "sizeof TcpConnection = " << sizeof(TcpConnection);
    bool ipv6 = argc > 2;
    EventLoop loop;
    InetAddress listenAddr(2000, false, ipv6);
    hades::MainServer server(&loop, listenAddr);

    server.start();

    loop.loop();
}
