//
// Created by nbaiot@126.com on 2020/8/29.
//

#include "websocket_listener.h"

#include <glog/logging.h>
#include <boost/asio/strand.hpp>
#include <boost/beast/core/bind_handler.hpp>

#include "websocket_session.h"

namespace nbaiot::rtc {

WebsocketListener::WebsocketListener(int threadCount) {
  for (int i = 0; i < threadCount; ++i) {
    io_threads_.push_back(std::make_unique<std::thread>([this]() {
      ioc_.run();
    }));
  }
}

void WebsocketListener::Run() {
  if (init_) {
    DoAccept();
  }
}

void WebsocketListener::SetOnNewConnectionCallback(WebsocketListener::OnNewConnectionCallback callback) {
  new_connection_callback_ = std::move(callback);
}

void WebsocketListener::SetErrorCallback(WebsocketListener::OnErrorCallback callback) {
  error_callback_ = std::move(callback);
}

bool WebsocketListener::Init(const std::string& ip, uint16_t port) {
  init_ = false;
  boost::asio::ip::tcp::endpoint ep(boost::asio::ip::make_address(ip), port);
  boost::beast::error_code ec;
  acceptor_.open(ep.protocol(), ec);
  if (ec) {
    LOG(ERROR) << "WebsocketListener open error:" << ec.message();
    return false;
  }

  acceptor_.set_option(boost::asio::socket_base::reuse_address(true), ec);
  if (ec) {
    LOG(ERROR) << "WebsocketListener set option error:" << ec.message();
    return false;
  }

  acceptor_.bind(ep, ec);
  if (ec) {
    LOG(ERROR) << "WebsocketListener bind error:" << ec.message();
    return false;
  }

  acceptor_.listen(boost::asio::socket_base::max_listen_connections, ec);
  if (ec) {
    LOG(ERROR) << "WebsocketListener listen error:" << ec.message();
    return false;
  }

  init_ = true;
  return true;
}

void WebsocketListener::DoAccept() {
  acceptor_.async_accept(
      boost::asio::make_strand(ioc_),
      boost::beast::bind_front_handler(
          &WebsocketListener::OnAccept,
          shared_from_this()));
}

void WebsocketListener::OnAccept(boost::beast::error_code ec, boost::asio::ip::tcp::socket socket) {
  if (ec) {
    LOG(ERROR) << "WebsocketListener accept error:" << ec.message();
  } else {
    auto session = std::make_shared<WebsocketSession>(std::move(socket));
    session->Run();
    if (new_connection_callback_) {
      new_connection_callback_(session);
    }
  }

  DoAccept();
}
}