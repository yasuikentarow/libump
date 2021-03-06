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
#include "ump/mj/Game.hpp"
#include "ump/mj/Yama.hpp"
#include "ump/thread/Thread.hpp"

namespace ump {
namespace server {
/***********************************************************************//**
	@brief ゲーム(半荘)
***************************************************************************/
class Game
  : public mj::Game, 
    public Thread, 
    public Logger
{
  typedef mj::Game super;

 private:
  std::weak_ptr<Server> server_;
  mj::Yama yama_;
  std::stack<std::unique_ptr<Job>> jobs_;
  std::mutex mutex_;
  int kanNum_;
  int serial_ = 0;

 public:
  Game(std::shared_ptr<const Config> config, 
       std::shared_ptr<Server> server);
  ~Game() override;

  std::shared_ptr<const Config> getConfig() const;

  std::shared_ptr<Server> getServer() const {
    return server_.lock();
  }

  void appendPlayer(std::shared_ptr<Player> player);
  bool canStart() const;
  bool isStart() const;
  virtual void start();
  virtual void stop();

  void beginJob(Job* job);

  std::shared_ptr<Player> getPlayer(size_t index) const;
  std::shared_ptr<Player> getTurnPlayer() const;

  void beginKyoku();
  void appendDora();
  void uraDora();
  mj::HaiArray drawHaipai(size_t num);
  const mj::Hai* drawHai();
  void onDiscarded(const Player& player, const mj::Hai* hai);

  void onOpenMentsu(std::shared_ptr<Player> player, 
                    std::shared_ptr<const mj::Mentsu> mentsu);

  bool canKan() const;

  Command createCommand(Command::Type type);

  void setTurn(size_t seat) override;
  size_t getRest() const override;

  bool isLastKyoku() const;

  void operator()() override;

  void sendAll(const Command& command, bool isLog = true);
  virtual bool sendCommand(std::shared_ptr<Player> player, 
                           const Command& command, 
                           bool isLog = false);

  virtual void onRecvCommand(std::shared_ptr<Player> player, 
                             const Command& command);

  virtual void onDisconnectPlayer(std::shared_ptr<Player> player);

 private:
  std::shared_ptr<Game> getThis();

  size_t findSeat() const;

  bool updateJob(const std::chrono::milliseconds& deltaTime);
  void stopAllJob();

  void dump() const;

  void appendDora(Command::Type type);
  const mj::Hai* getDora(const mj::Hai* hai) const;
};
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
