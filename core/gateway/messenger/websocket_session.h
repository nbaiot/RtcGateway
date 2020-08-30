//
// Created by nbaiot@126.com on 2020/8/29.
//

#ifndef RTCGATEWAY_WEBSOCKET_SESSION_H
#define RTCGATEWAY_WEBSOCKET_SESSION_H

#include <deque>
#include <memory>
#include <functional>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/core/flat_buffer.hpp>

namespace nbaiot::rtc {

class WebsocketSession : public std::enable_shared_from_this<WebsocketSession>{
public:
  using OnDisconnectCallback = std::function<void()>;
  using OnReceiveMsgCallback = std::function<void(const std::string&)>;

  explicit WebsocketSession(boost::asio::ip::tcp::socket&& socket);

  ~WebsocketSession();

  void Run();

  void Close();

  void Send(std::shared_ptr<std::string const> const& data);

  void SetDisconnectCallback(OnDisconnectCallback callback);

  void SetReceiveMsgCallback(OnReceiveMsgCallback callback);


private:
  void OnRun();

  void OnAccept(boost::beast::error_code ec);

  void DoRead();

  void OnRead(boost::beast::error_code ec, std::size_t bytes_transferred);

  void OnClose();

  void OnClosed(boost::system::error_code ec);

  void OnSend(const std::shared_ptr<const std::string> &data);

  void OnWrite(boost::beast::error_code ec, std::size_t bytes_transferred);

private:
  boost::beast::flat_buffer buffer_;
  boost::beast::websocket::stream<boost::beast::tcp_stream> ws_;
  std::deque<std::shared_ptr<std::string const>> send_queue_;

  OnDisconnectCallback disconnect_callback_;
  OnReceiveMsgCallback receive_msg_callback_;
};

}


#endif //RTCGATEWAY_WEBSOCKET_SESSION_H
