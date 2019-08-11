#include "client.h"

namespace hades {

void Client::onMessage(const muduo::net::TcpConnectionPtr& conn,
        muduo::net::Buffer* buf,
        muduo::Timestamp time) {
    while (buf->readableBytes() > 0) {
        if (!reqType_) {
            if (*buf->peek() == '*') {
                reqType_ = constants::REQ_MULTIBULK;
            } else {
                reqType_ = constants::REQ_INLINE;
            }
        }

        if (reqType_ == constants::REQ_INLINE) {
        } else if (reqType_ == constants::REQ_MULTIBULK) {
            LOG_DEBUG << "process multibulk";
            if (processMultibulkBuffer(buf) != constants::OK) break;
        } else {
        }
        for (auto& arg : argv_) {
            LOG_DEBUG << arg;
        }

        conn->send("*1$-1\r\n");
        reset();
    }
}

int Client::processMultibulkBuffer(muduo::net::Buffer* buf) {
    if (multiBulkLen_ == 0) {
        const char* crlf = buf->findCRLF();
        if (!crlf) {
            return constants::ERR;
        } 

        buf->retrieve(1);
        int len = static_cast<int>(crlf - buf->peek());
        std::string multiBulkLen(buf->peek(), len);
        LOG_DEBUG << "multiBulkLen:" << multiBulkLen;
        int ll = std::stoi(multiBulkLen);
        buf->retrieveUntil(crlf + 2);

        if (ll <= 0) {
            return constants::OK;
        }

        multiBulkLen_ = ll;
    }

    while (multiBulkLen_) {
        if (bulkLen_ == -1) {
            if (buf->readableBytes() <= 0) {
                return constants::ERR;
            }
            if (*buf->peek() != '$') {
                // TODO:协议错误
                return constants::ERR;
            }
            const char* crlf = buf->findCRLF();
            if (!crlf) {
                return constants::ERR;
            }

            buf->retrieve(1);
            int len = static_cast<int>(crlf - buf->peek());
            std::string bulkLen(buf->peek(), len);
            LOG_DEBUG << "bulkLen:" << bulkLen;
            int ll = std::stoi(bulkLen);
            buf->retrieveUntil(crlf + 2);

            if (ll <= 0) {
                return constants::OK;
            }

            bulkLen_ = ll;
        }

        if (buf->readableBytes() < bulkLen_) {
            return constants::ERR;
        }

        const char* crlf = buf->findCRLF();
        if (!crlf) {
            // TODO: 协议错误
            return constants::ERR;
        }
        int len = static_cast<int>(crlf - buf->peek());
        if (bulkLen_ != len) {
            // TODO: 协议错误
            return constants::ERR;
        }

        argv_.emplace_back(buf->peek(), len);
        buf->retrieveUntil(crlf + 2);

        bulkLen_ = -1;
        --multiBulkLen_;
    }
    return constants::OK;
}

void Client::reset() {
    reqType_ = 0;
    multiBulkLen_ = 0;
    bulkLen_ = -1;
    argv_.clear();
}

} //namespace hades
