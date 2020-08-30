//
// Created by nbaiot@126.com on 2020/8/29.
//

#ifndef RTCGATEWAY_WEBSOCKET_LISTENER_H
#define RTCGATEWAY_WEBSOCKET_LISTENER_H

#include <vector>
#include <memory>
#include <thread>
#include <functional>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/beast/core/error.hpp>
#include <boost/asio/executor_work_guard.hpp>

namespace nbaiot::rtc {

class WebsocketSession;

class WebsocketListener : public std::enable_shared_from_this<WebsocketListener> {

public:
  using OnNewConnectionCallback = std::function<void(std::weak_ptr<WebsocketSession> session)>;

  using OnErrorCallback = std::function<void(boost::system::error_code, const std::string&)>;

  explicit WebsocketListener(int threadCount);

  bool Init(const std::string& ip, uint16_t port);

  void Run();

  void SetOnNewConnectionCallback(OnNewConnectionCallback callback);

  void SetErrorCallback(OnErrorCallback callback);

private:

  void DoAccept();

  void OnAccept(boost::beast::error_code ec, boost::asio::ip::tcp::socket socket);

private:
  bool init_{false};
  std::vector<std::unique_ptr<std::thread>> io_threads_;
  boost::asio::io_context ioc_;
  boost::asio::executor_work_guard<boost::asio::io_context::executor_type> guard_{ioc_.get_executor()};
  boost::asio::ip::tcp::acceptor acceptor_{ioc_};

  OnNewConnectionCallback new_connection_callback_;
  OnErrorCallback error_callback_;

};

}


#endif //RTCGATEWAY_WEBSOCKET_LISTENER_H
