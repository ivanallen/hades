#pragma once
#include <muduo/net/TcpConnection.h>

namespace hades {

class MainServer;
class Client {
public:
    explicit Client(MainServer* owner, const muduo::net::TcpConnectionPtr& conn) : owner_(owner), conn_(conn) {
        using std::placeholders::_1;
        using std::placeholders::_2;
        using std::placeholders::_3;
        conn_->setMessageCallback(std::bind(&Client::onMessage, this, _1, _2, _3));
    }

    void onMessage(const muduo::net::TcpConnectionPtr& conn,
            muduo::net::Buffer* buf,
            muduo::Timestamp time);

private:
    MainServer* owner_;
    muduo::net::TcpConnectionPtr conn_;
};

} // hades
