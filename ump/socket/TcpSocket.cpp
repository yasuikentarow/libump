/*
Copyright 2016 fuzz, Inc. All rights reserved. 
   http://www.fuzz.co.jp

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of fuzz, Inc. nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL FUZZ, INC. BE LIABLE FOR ANY 
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF 
THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/***********************************************************************//**
	@file
***************************************************************************/
#include "ump/socket/TcpSocket.hpp"

namespace ump {
namespace socket {
/***********************************************************************//**
	@brief デフォルトコンストラクタ
***************************************************************************/
TcpSocket::TcpSocket()
  : TcpSocket(INVALID_FD)
{
}
/***********************************************************************//**
	@brief コンストラクタ
	@param[in] fd ファイルディクリプタ
***************************************************************************/
TcpSocket::TcpSocket(int fd)
  : super(), 
    fd_(fd), 
    port_(0)
{}
/***********************************************************************//**
	@brief デストラクタ.
***************************************************************************/
TcpSocket::~TcpSocket() {
  close();
}
/***********************************************************************//**
	@brief ソケットが開いているか調べる
	@return	ソケットが開いているとき真
***************************************************************************/
bool TcpSocket::isOpen() const {
  return (fd_ >= 0);
}
/***********************************************************************//**
	@copydoc Socket::connect
***************************************************************************/
bool TcpSocket::onConnect(const std::string& host, int port) {
  struct sockaddr_in address;
  address.sin_family = AF_INET;
  struct hostent* entry = gethostbyname(host.c_str());
  if(!entry) {
    super::setError("no such host");
    return false;
  }
  memcpy(&address.sin_addr, entry->h_addr_list[0], entry->h_length);
  address.sin_port = htons(port);
  open();
  int result = 
    ::connect(fd_, (struct sockaddr *)&address, sizeof(address));
  if(result < 0) {
    onClose();
    super::setError("can't connect");
    return false;
  }
  port_ = port;
  return true;
}
/***********************************************************************//**
	@copydoc Socket::listen
***************************************************************************/
bool TcpSocket::onListen(int port) {
  struct sockaddr_in address;
  memset(&address, 0, sizeof(address));
  address.sin_port = htons(port);
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = htonl(INADDR_ANY);
  open();
  {
    const int on = 1;
    ::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    ::setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on));
  }
  int result = bind(fd_, reinterpret_cast<struct sockaddr *>(&address), 
                    sizeof(address));
  if(result != 0) {
    setError();
    onClose();
    return false;
  }
  if(port == 0) {
    socklen_t size = sizeof(address);
    if(getsockname(fd_, reinterpret_cast<struct sockaddr*>(&address), 
                   &size) == 0) {
      port_ = ntohs(address.sin_port);
    }
    else {
      setError();
      onClose();
      return false;
    }
  }
  else {
    port_ = port;
  }
  ::listen(fd_, 1);
  return true;
}
/***********************************************************************//**
	@copydoc Socket::onAccept
***************************************************************************/
std::shared_ptr<Socket> TcpSocket::onAccept(int timeout) {
  {
    struct pollfd fds;
    fds.fd = fd_;
    fds.events = POLLIN;
    if(poll(&fds, 1, timeout) <= 0) {
      return nullptr;
    }
  }
  struct sockaddr_in address;
  socklen_t len = sizeof(address);
  int fd = ::accept(fd_, (struct sockaddr*)&address, &len);
  if(fd < 0) {
    return nullptr;
  }
  return std::make_shared<TcpSocket>(fd);
}
/***********************************************************************//**
	@brief データを送信する
	@param buff 送信するデータ
	@param size 送信するサイズ
	@return 送信に成功したとき真
***************************************************************************/
bool TcpSocket::onSend(const char* buff, size_t size) {
  while(size > 0) {
    auto sendSize = ::send(fd_, buff, size, 0);
    if(sendSize > 0) {
      size -= sendSize;
      buff += sendSize;
    }
    else {
      onClose();
      return false;
    }
  }
  return true;
}
/***********************************************************************//**
	@copydoc Socket::recv
***************************************************************************/
bool TcpSocket::onRecv(char* buff, size_t size) {
  while(size > 0) {
    auto recvSize = ::recv(fd_, buff, size, 0);
    if(recvSize > 0) {
      size -= recvSize;
      buff += recvSize;
    }
    else {
      onClose();
      return false;
    }
  }
  return true;
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void TcpSocket::onClose() {
  ::close(fd_);
  fd_ = INVALID_FD;
  port_ = 0;
}
/***********************************************************************//**
	@brief ソケットを開く
***************************************************************************/
void TcpSocket::open() {
  assert(!isOpen());
  fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void TcpSocket::setError() {
  super::setError(strerror(errno));
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}