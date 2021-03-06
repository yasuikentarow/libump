﻿/*
Copyright 2017 fuzz, Inc. All rights reserved. 
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
#include "ump/server/JobAgari.hpp"
#include "ump/server/JobKakan.hpp"
#include "ump/server/JobTsumo.hpp"
#include "ump/server/Game.hpp"
#include "ump/server/Player.hpp"

namespace ump {
namespace server {
/***********************************************************************//**
	@brief コンストラクタ
***************************************************************************/
JobKakan::JobKakan(Game& game, const mj::Hai* hai)
  : super(game), 
    hai_(hai)
{
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void JobKakan::onBegin() {
  auto& game = getGame();
  auto playerNum = game.countPlayer();
  for(size_t i = 1; i < playerNum; i++) {
    auto index = (game.getTurn() + i) % playerNum;
    auto player = game.getPlayer(index);
    if(player->isConnect() && player->canRon(hai_, true)) {
      auto command = game.createCommand(Command::TYPE_NAKI_Q);
      command.append(hai_->toString());
      command.append(Command::TYPE_RON);
      game.sendCommand(player, command);
      players_.push_back(player);
    }
  }
}
/***********************************************************************//**
	@brief 
***************************************************************************/
Job* JobKakan::onUpdate() {
  auto& game = getGame();
  if(!players_.empty()) {
    auto player = players_.front();
    auto reply = player->getReply();
    if(reply.isExist()) {
      if(reply.getType() == Command::TYPE_RON) {
        return new JobAgari(game, player->getSeat());
      }
      else {
        players_.erase(players_.begin());
      }
    }
    return this;
  }
  game.appendDora();
  return new JobTsumo(game, true);
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
