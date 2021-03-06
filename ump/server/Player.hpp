﻿/*
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
#pragma once

#include "ump/Command.hpp"
#include "ump/Logger.hpp"
#include "ump/mj/Player.hpp"
#include "ump/thread/Atomic.hpp"
#include "ump/thread/SocketThread.hpp"

namespace ump {
namespace server {
/***********************************************************************//**
	@brief プレイヤー
***************************************************************************/
class Player
  : public mj::Player, 
    public SocketThread::Listener, 
    public std::enable_shared_from_this<Player>
{
  typedef mj::Player super;

 private:
  enum {
    FLAG_FURITEN, 
    FLAG_MAX
  };

 private:
  std::weak_ptr<Server> server_;
  Command command_;
  thread::Atomic<Command> reply_;
  std::mutex mutex_;
  std::bitset<FLAG_MAX> flag_;
  std::vector<Command> gameLog_;
  std::vector<Command> kyokuLog_;
  std::unique_ptr<SocketThread> socket_;

 public:
  Player(std::shared_ptr<Server> server, 
         std::shared_ptr<Socket> socket);
  ~Player();

  void setGame(std::shared_ptr<mj::Game> game) override;
  std::shared_ptr<Game> getGame() const;

  bool isConnect() const;
  bool sendCommand(const Command& command);
  void closeSocket();

  UMP_GETTER(Command, command_);

  Command getReply() const {
    return reply_.get();
  }

  void reset() override;

  void onDiscarded(const Player& player, const mj::Hai* hai);

  bool canRichi() const;
  bool canRon(const mj::Hai* hai, bool chankan);
  void setFuriten(bool value);

  void swapSocket(Player& player);
  void onReconnect();

  void start();
  void stop();

 protected:
  mj::Sutehai* sutehai(const mj::Sutehai& sutehai) override;

  /**
   * コマンドを送信したときの処理
   * @param[in] command コマンド
   */
  virtual void onSendCommand(const Command& command) {}

  void onRecvCommand(const Command& command) override;
  void onDisconnectSocket() override;

  void onOpenMentsu(std::shared_ptr<const mj::Mentsu> mentsu) override;

  void log(Logger::Level level, const std::string& message) const;

 private:
  std::shared_ptr<Server> getServer() const;

  UMP_BIT_GETTER(Furiten, flag_, FLAG_FURITEN);
  void updateFuriten();
  void updateFuriten(const mj::Hai* hai);
};
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
