#include <muduo/net/TcpServer.h>
#include <muduo/base/Logging.h>
#include <muduo/base/Thread.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>

#include <utility>
#include <unistd.h>
#include "client.h"

using namespace muduo;
using namespace muduo::net;

namespace hades {

class MainServer {
public:
    MainServer(EventLoop* loop, const InetAddress& listenAddr)
        : loop_(loop),
        server_(loop, listenAddr, "MainServer") {
        server_.setConnectionCallback(
                std::bind(&MainServer::onConnection, this, _1));
        server_.setThreadNum(0);
    }

    void start() {
        server_.start();
    }
    void stop();

private:
    void onConnection(const TcpConnectionPtr& conn) {
        LOG_INFO << conn->peerAddress().toIpPort() << " -> "
            << conn->localAddress().toIpPort() << " is "
            << (conn->connected() ? "UP" : "DOWN");
        LOG_INFO << conn->getTcpInfoString();
        ClientPtr client(new Client(this, conn));
        LOG_INFO << "connection name:" << conn->name();
        clients_[conn->name()] = client;
    }

    EventLoop* loop_;
    TcpServer server_;
    std::unordered_map<string, ClientPtr> clients_;
};

} // namespace hades

int main(int argc, char* argv[]) {
    LOG_INFO << "pid = " << getpid() << ", tid = " << CurrentThread::tid();
    EventLoop loop;
    InetAddress listenAddr(2000, false, false);
    hades::MainServer server(&loop, listenAddr);

    server.start();

    loop.loop();
}
