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
#pragma once

#include "ump/Logger.hpp"
#include "ump/mj/Game.hpp"
#include "ump/mj/Yama.hpp"

namespace ump {
namespace server {
/***********************************************************************//**
	@brief ゲーム(半荘)
***************************************************************************/
class Game
  : public mj::Game, 
    public Logger
{
  typedef mj::Game super;

 private:
  enum {
    FLAG_STOP, 
    FLAG_MAX
  };

  class Thread;

 private:
  Server& server_;
  std::unique_ptr<std::thread> thread_;
  mj::Yama yama_;
  std::stack<Job*> jobs_;
  std::bitset<FLAG_MAX> flag_;

 public:
  Game(std::shared_ptr<const Config> config, Server& server);
  ~Game() override;

  const Config& getConfig() const;

  Server& getServer() const {
    return server_;
  }

  void start();
  void stop();
  bool isStop() const {
    return flag_.test(FLAG_STOP);
  }

  void beginJob(Job* job);

  std::shared_ptr<Player> getPlayer(size_t index) const;
  std::shared_ptr<Player> getTurnPlayer() const;

  void beginKyoku();
  void appendDora();
  void uraDora();
  mj::HaiArray drawHaipai(size_t num);
  const mj::Hai* tsumo();

  size_t getRest() const override;

  void sendAll(const Command& command) const;

  void operator()();

  virtual void onRecvCommand(std::shared_ptr<Player> player, 
                             const Command& command);

 private:
  void updateJob(const std::chrono::milliseconds& deltaTime);
  
  void dump() const;

  std::string createId() const;
  std::string getLogPath(const std::string& id) const;
};
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}