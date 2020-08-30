//
// Created by nbaiot@126.com on 2020/8/29.
//

#include "websocket_session.h"

#include <glog/logging.h>
#include <boost/asio/dispatch.hpp>
#include <boost/beast/core/bind_handler.hpp>
#include <boost/beast/core/buffers_to_string.hpp>

namespace nbaiot::rtc {

WebsocketSession::WebsocketSession(boost::asio::ip::tcp::socket&& socket) : ws_(std::move(socket)) {

}

WebsocketSession::~WebsocketSession() {
  /// 链接断开，则对象销毁，反之则一直存在
  if (disconnect_callback_) {
    disconnect_callback_();
  }
#if DEBUG_SIGNALING
  LOG(INFO) << "~~~~~~~~~~~~~~~~~WebsocketSession";
#endif
}

void WebsocketSession::Run() {
  boost::asio::dispatch(ws_.get_executor(), boost::beast::bind_front_handler(&WebsocketSession::OnRun,
                                                                             shared_from_this()));
}

void WebsocketSession::OnRun() {
  ws_.set_option(boost::beast::websocket::stream_base::timeout::suggested(boost::beast::role_type::server));

  ws_.set_option(boost::beast::websocket::stream_base::decorator(
      [](boost::beast::websocket::response_type& res) {
        res.set(boost::beast::http::field::server,
                "nbaiot/rtc-gateway");
      }));

  ws_.async_accept(boost::beast::bind_front_handler(&WebsocketSession::OnAccept, shared_from_this()));
}

void WebsocketSession::OnAccept(boost::beast::error_code ec) {
  if (ec) {
    LOG(ERROR) << ">>>>>>>>>> WebsocketSession OnAccept error:" << ec.message();
    return;
  }

  DoRead();
}

void WebsocketSession::DoRead() {
  ws_.async_read(buffer_, boost::beast::bind_front_handler(&WebsocketSession::OnRead, shared_from_this()));
}

void WebsocketSession::OnRead(boost::beast::error_code ec, std::size_t bytes_transferred) {
  boost::ignore_unused(bytes_transferred);

  if (ec) {
    LOG(ERROR) << ">>>>>>>>>> WebsocketSession read error:" << ec.message();
    return;
  }

  if (receive_msg_callback_) {
    try {
      receive_msg_callback_(boost::beast::buffers_to_string(buffer_.data()));
    } catch (...) {
    }
  }

  buffer_.consume(buffer_.size());

  DoRead();
}


void WebsocketSession::Close() {
  boost::asio::dispatch(ws_.get_executor(), boost::beast::bind_front_handler(&WebsocketSession::OnClose,
                                                                             shared_from_this()));
}

void WebsocketSession::OnClose() {
  if (ws_.is_open()) {
    ws_.async_close(boost::beast::websocket::close_code::normal,
                    boost::beast::bind_front_handler(&WebsocketSession::OnClosed, shared_from_this()));
  }
}

void WebsocketSession::OnClosed(boost::system::error_code ec) {
  LOG(INFO) << ">>>>>>>>>>>>>> WebsocketSession closed";
}


void WebsocketSession::Send(std::shared_ptr<std::string const> const& data) {
  if (ws_.is_open()) {
    boost::asio::dispatch(ws_.get_executor(),
        boost::beast::bind_front_handler(&WebsocketSession::OnSend, shared_from_this(), data));
  }
}

void WebsocketSession::OnSend(const std::shared_ptr<const std::string>& data) {
  send_queue_.push_back(data);

  if (send_queue_.size() > 1)
    return;

  ws_.async_write(boost::asio::buffer(*send_queue_.front()),
                  boost::beast::bind_front_handler(&WebsocketSession::OnWrite,
                      shared_from_this()));
}

void WebsocketSession::OnWrite(boost::beast::error_code ec, std::size_t bytes_transferred) {
  if (ec) {
    /// TODO: handle error
    LOG(ERROR) << ">>>>>>>>>> WebsocketSession write error:" << ec.message();
  }

  send_queue_.pop_front();

  if (!send_queue_.empty()) {
    ws_.async_write(boost::asio::buffer(*send_queue_.front()),
                    boost::beast::bind_front_handler(&WebsocketSession::OnWrite, shared_from_this()));
  }
}

void WebsocketSession::SetDisconnectCallback(WebsocketSession::OnDisconnectCallback callback) {
  disconnect_callback_ = std::move(callback);
}

void WebsocketSession::SetReceiveMsgCallback(WebsocketSession::OnReceiveMsgCallback callback) {
  receive_msg_callback_ = std::move(callback);
}



}