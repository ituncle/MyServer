//
// Created by root on 5/27/16.
//

#ifndef JOBSERVER_YFQPROTO_H
#define JOBSERVER_YFQPROTO_H

#include "protocol/Pareser.h"
#include "utils/crc32.h"

#include "network/muduo/base/BlockingQueue.h"

namespace ft {
namespace proto {
///
/// @code
/// +-------------------+-------------------+------------------+
/// |    start flag     |   message head    |  message body    |
/// |  (always be 0xAA) |  (little endian)  |   (protobuf)     |
/// +-------------------+-------------------+------------------+
/// |  only one byte    |           message length             |
/// @endcode


/**
 * enum for message id
 */
enum MessageType {
  Invalid = 0x00000000,
  CommonAns = 0x00000010,
  HeartBeat = 0x00000001
};

/**
 * 头标记
 */
static const unsigned char START_FLAG = 0xaa;


using msglen_type = unsigned int;   // type of message len
using msgid_type = unsigned int;   // type of message type
using crc_type = unsigned int;      // type of crc32-value
/**
 * message head
 */
struct MsgHead {
  static const size_t MsgHeadLen = 12;    // 消息头占的字节数

  crc_type crc_;          // 消息的crc32校验码(对：消息长度 + 消息ID + 消息体求crc校验)
  msglen_type msgLen_;    // 消息长度（从消息头第一个字节，到消息体最后一个字节的字节数）
  msgid_type msgid_;      // 消息类型

  MsgHead() { }

  MsgHead(const char *buf, size_t len) {
    assert(len >= MsgHeadLen);

    memcpy(this, buf, MsgHeadLen);
  }
};

struct Request;

class YfqParser final : public Parser<Request> {
public:
  YfqParser()
  : Parser(),
    requests_(std::make_shared<muduo::BlockingQueue<std::shared_ptr<Request>>>()) {}

//  YfqParser(std::string& name)
//  : Proto(name),
//    requests_(std::make_shared<muduo::BlockingQueue<std::shared_ptr<Request>>>()) {}

  /**
   * 根据消息体和消息类型，按照协议组包成数据流
   */
  static std::string packet(const std::string &msgbody, MessageType type) {
    MsgHead head;
    head.msgLen_ = MsgHead::MsgHeadLen + msgbody.length();
    head.msgid_ = static_cast<msgid_type>(type);

    const size_t totalLen = sizeof(START_FLAG) + head.msgLen_;
    std::unique_ptr<char> data(new char[totalLen]);

    char *pdata = data.get();
    pdata[0] = START_FLAG;
    memcpy(pdata + sizeof(START_FLAG), &head, sizeof(head));
    memcpy(pdata + sizeof(START_FLAG) + MsgHead::MsgHeadLen, msgbody.c_str(), msgbody.length());

    head.crc_ = utils::CRC32::instance()->calc(pdata + sizeof(START_FLAG) + sizeof(crc_type),
                                               head.msgLen_ - sizeof(crc_type));
    memcpy(pdata + sizeof(START_FLAG), &head.crc_, sizeof(crc_type));

    std::string ret;
    ret.reserve(totalLen);
    ret.append(pdata, totalLen);
    return ret;
  }
  std::shared_ptr<muduo::Queue<std::shared_ptr<Request>>> getRequestQueue() override {return requests_;}
  void parse(const std::weak_ptr<muduo::net::TcpConnection> &conn,
             muduo::net::Buffer *buf,
             const muduo::Timestamp &time) override {
    size_t parsed = 0;  // 已解析过的字节数
    auto remain = buf->readableBytes(); // 还未解析的字节数
    auto *begin = buf->peek();  // 指向第一个未解析的字节

    do {
      /**
       * 找头标记
       */
      for (; remain > 0; --remain, ++parsed) {
        if (START_FLAG == static_cast<unsigned char>(begin[parsed])) break; // 找到疑是一个消息包的开始标记
      }
      if (remain == 0) {     // 没找到头标志，丢弃所有的字节
        LogInfo << "can't find START_FLAG(0xAA) in recvbuf, abandon all bytes";
        break;
      }

      /**
       * 检查剩余字节是否足够一个消息头
       */
      if (remain < sizeof(START_FLAG) + MsgHead::MsgHeadLen) {
        LogInfo << "no enough bytes for msghead, remain = " << remain;
        break;   // no enough bytes for msghead
      }

      /**
       * 跳过START_FLAG
       */
      parsed += sizeof(START_FLAG);
      remain -= sizeof(START_FLAG);

      /**
       * 解析消息头
       */
      MsgHead head(begin + parsed, remain);

      /**
       * 确保消息长度>=MsgHead::MsgHeadLen
       */
      if (head.msgLen_ < MsgHead::MsgHeadLen) {
        LogInfo << "invalid msglen: " << head.msgLen_;
        continue;
      }

      /**
       * 检查剩余字节是否足够消息头 + 消息体
       */
      if (remain < head.msgLen_) {
        const size_t maxMsgLen = 128 * 1024;
        if (head.msgLen_ > maxMsgLen) {
          LogWarn << "Please notice: the length of request(type=" << head.msgid_ << ") is bigger than " << maxMsgLen;
        }
        // 回退
        parsed -= sizeof(START_FLAG);
        remain += sizeof(START_FLAG);
        LogInfo << "no enough bytes for msghead+msgbody, remain = " << remain << ", head.msgLen_ = " << head.msgLen_;
        break;
      }

      /**
       * CRC32校验
       */
      // 计算CRC
      crc_type crc = utils::CRC32::instance()->calc(begin + parsed + sizeof(crc_type), head.msgLen_ - sizeof(crc_type));
      // 验证CRC
      if (crc != head.crc_) { // 校验失败
        LogDebug << "failed to check the crc32, abandon one byte, msgId=" <<
        head.msgid_ << ", msgLen=" << head.msgLen_ << ", crc_received=" << head.crc_ << ", crc_expected = " <<
        crc;
        // 接着找下一个START_FLAG
        continue;
      }

      /**
       * 解析消息体
       */
      if (!parseMsgBody(conn, begin + parsed + MsgHead::MsgHeadLen, head.msgLen_ - MsgHead::MsgHeadLen,
                        static_cast<MessageType>(head.msgid_), time)) {
        LogDebug << "failed to parse msgbody, msgId=" << head.msgid_ <<
        ", msgLen=" << head.msgLen_ << ", crc=" << head.crc_;
//                        continue;
      }
      /**
  * 丢弃这一个消息包
  */
      parsed += head.msgLen_;
      remain -= head.msgLen_;
    } while (remain > 0);

    buf->retrieve(parsed);  // 删除已解析的字节
  }

private:
  bool parseMsgBody(const std::weak_ptr<muduo::net::TcpConnection> &conn, const char *body,
                    size_t len, MessageType type, const muduo::Timestamp &time) {
    // append to the request-queue
    auto request = std::make_shared<Request>(std::string(body, len), type, conn, time);
    requests_->put(request);
    return true;
  }

private:
  std::shared_ptr<muduo::BlockingQueue<std::shared_ptr<Request>>> requests_;
};

/**
 * request received from remote
 */
struct Request {
  using value_type = std::string;

  // pointer to the content of this request
  value_type rquest_;
  // the kind of this request
  MessageType reqtype_ = Invalid;

  // the progress of the request
  YfqProtocol::CommonAns_Progress progress = YfqProtocol::CommonAns::Pending;

  // the connection from which received the request.
  // NOTE: the request will be invalid while the %conn be null
  const std::weak_ptr<muduo::net::TcpConnection> conn;

  // the timestamp when we received this request
  muduo::Timestamp time_;

  Request() {

  }

  // constructor
  Request(const value_type &request, MessageType type,
          const std::weak_ptr<muduo::net::TcpConnection> &from, const muduo::Timestamp &time)
  : rquest_(request), reqtype_(type), conn(from), time_(time) {
  }


  /**
   * @param response a
   */
  void respond(const std::string &msgbody, MessageType type) {
    std::string data = YfqParser::packet(msgbody, type);

    std::string msg = muduo::printBytes(data.c_str(), 0, data.length());
    LogInfo << "respond to remote " << data.length() << " bytes: " << msg;

    auto pconn = conn.lock();
    if (pconn) {
      LogTrace << "respond to remote " << data.length() << " bytes";
      pconn->send(data.c_str(), data.length());
    } else {
      LogError << "failed to remote due to the connection is null";
    }
  }
};

}
}

#endif //JOBSERVER_YFQPROTO_H
