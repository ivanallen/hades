#pragma once

#include <muduo/base/Logging.h>
#include <muduo/net/TcpConnection.h>
#include <memory>
#include "constants.h"

namespace hades {

class MainServer;
class Client {
public:
    explicit Client(MainServer* owner, const muduo::net::TcpConnectionPtr& conn) : owner_(owner), conn_(conn) {
        using std::placeholders::_1;
        using std::placeholders::_2;
        using std::placeholders::_3;
        LOG_DEBUG << "Client()";
        conn_->setMessageCallback(std::bind(&Client::onMessage, this, _1, _2, _3));
    }

    void onMessage(const muduo::net::TcpConnectionPtr& conn,
            muduo::net::Buffer* buf,
            muduo::Timestamp time);

private:
    int processMultibulkBuffer(muduo::net::Buffer* buf);
    void reset();
private:
    MainServer* owner_ = nullptr;
    muduo::net::TcpConnectionPtr conn_ = nullptr;
    int reqType_ = 0;
    int multiBulkLen_ = 0;
    int bulkLen_ = -1;
    std::vector<std::string> argv_;
};

using ClientPtr = std::shared_ptr<Client>;

} // hades
