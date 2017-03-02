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
#include "AgariTest.hpp"
/***********************************************************************//**
	@brief 
***************************************************************************/
AgariTest::AgariTest()
  : config_(std::make_shared<ump::mj::Config>()), 
    game_(std::make_shared<ump::mj::Game>(config_)), 
    player_(std::make_shared<ump::mj::Player>()), 
    parser_(*player_)
{
  game_->appendPlayer(player_);
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void AgariTest::onRun() {
  const struct {
    const char* hais;
    int tsumoHan;
    int ronHan;
  } TABLES[] = {
    { "6m6m6m2p3p4p5p6p6p6p7p2s3s4s", 2, 1 }, 
    /* 七対子 */
    { "2m2m9m9m3p3p1s1s7s7s1z1z5z5z", 3, 2 }
  };
  for(auto& table : TABLES) {
    {
      auto& agari = parse(table.hais, false);
      TEST_MESSAGE(agari.getHan() == table.tsumoHan, table.hais);
    }
    {
      auto& agari = parse(table.hais, true);
      TEST_MESSAGE(agari.getHan() == table.ronHan, table.hais);
    }
  }
}
/***********************************************************************//**
	@brief 
***************************************************************************/
const ump::mj::Agari& AgariTest::parse(const char* str, bool isRon) {
  ump::mj::HaiArray hais(str);
  auto hai = hais.removeSame(hais.back());
  player_->reset();
  player_->drawHaipai(hais);
  player_->resetFirst();
  if(isRon) {
    return parser_.parse(hai);
  }
  player_->tsumo(hai);
  return parser_.parse(nullptr);
}
/***********************************************************************//**
	$Id$
***************************************************************************/