﻿/*
Copyright 2018 fuzz, Inc. All rights reserved. 
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
#include "ump/Command.hpp"
#include "ump/mj/Config.hpp"
#include "ump/mj/Game.hpp"
#include "ump/mj/Player.hpp"

namespace ump {
namespace mj {
/***********************************************************************//**
        @brief デフォルトコンストラクタ
***************************************************************************/
Game::Game()
  : random_(new std::default_random_engine(std::random_device()())), 
    oya_(0), 
    round_(0), 
    renchan_(0), 
    kyotaku_(0), 
    turn_(0), 
    lastSutehai_(nullptr)
{}
/***********************************************************************//**
        @brief デストラクタ
***************************************************************************/
Game::~Game() {
  clearPlayer();
}
/***********************************************************************//**
	@brief プレイヤーを追加する
	@param[in] seat 席
	@param[in] player 追加するプレイヤー
***************************************************************************/
void Game::setPlayer(size_t seat, std::shared_ptr<Player> player) {
  player->setGame(shared_from_this());
  player->setSeat(seat);
  if(players_.size() <= seat) {
    players_.resize(seat + 1);
  }
  players_[seat] = player;
}
/***********************************************************************//**
	@brief プレイヤーを削除する
	@param[in] player 削除するプレイヤー
***************************************************************************/
void Game::removePlayer(std::shared_ptr<Player> player) {
  assert(getPlayer(player->getSeat()) == player);
  player->setGame(nullptr);
  players_.at(player->getSeat()).reset();
  onRemovePlayer(player);
}
/***********************************************************************//**
	@brief プレイヤーの人数を取得する
	@return プレイヤー数
***************************************************************************/
size_t Game::countPlayer() const {
  return players_.size();
}
/***********************************************************************//**
	@brief プレイヤーを取得する
	@param[in] seat 席
	@return プレイヤー
***************************************************************************/
std::shared_ptr<Player> Game::getPlayer(size_t seat) const {
  return (seat < players_.size()) ? players_.at(seat) : nullptr;
}
/***********************************************************************//**
	@brief プレイヤーを取得する
	@param[in] seat 席を表す文字列
	@return プレイヤー
***************************************************************************/
std::shared_ptr<Player> Game::getPlayer(const char* seat) const {
  return getPlayer(Command::StringToSeat(seat));
}
/***********************************************************************//**
	@brief 着順を取得する
	@return 着順のプレイヤーのベクター
***************************************************************************/
Players Game::getRanking() const {
  auto players(getPlayers());
  std::stable_sort(players.begin(), players.end(), 
                   [](const std::shared_ptr<Player>& a, 
                      const std::shared_ptr<Player>& b) {
                     return a->getPoint() > b->getPoint();
                   });
  return players;
}
/***********************************************************************//**
	@brief 供託を加算する
	@param[in] value 加算する値
	@return 自分自身
***************************************************************************/
void Game::addKyotaku(const BigNum& value) {
  kyotaku_ += value;
}
/***********************************************************************//**
	@brief 供託をリセットする
	@return 供託されていた点数
***************************************************************************/
BigNum Game::resetKyotaku() {
  auto kyotaku = getKyotaku();
  kyotaku_ = 0;
  return kyotaku;
}
/***********************************************************************//**
	@brief 手番を変更する
	@param[in] turn 手番
***************************************************************************/
void Game::setTurn(size_t turn) {
  assert(turn < countPlayer());
  turn_ = turn;
}
/***********************************************************************//**
	@brief 手番を親に戻す
***************************************************************************/
void Game::resetTurn() {
  setTurn(getOya());
}
/***********************************************************************//**
	@brief 手番を進める
	@return 進んだ後の手番
***************************************************************************/
size_t Game::nextTurn() {
  setTurn((getTurn() + 1) % countPlayer());
  return getTurn();
}
/***********************************************************************//**
	@brief 牌を捨てる
	@param[in] player プレイヤー
	@param[in] sutehai 捨て牌
	@return 河の捨て牌
***************************************************************************/
Sutehai* Game::sutehai(Player& player, const Sutehai& sutehai) {
  lastSutehai_ = player.sutehai(sutehai);
  return lastSutehai_;
}
/***********************************************************************//**
        @brief 場風を取得する
	@return 場風
***************************************************************************/
const Hai* Game::getBakaze() const {
  return Hai::Get("1z")->rotate(int(getRound()));
}
/***********************************************************************//**
	@brief ドラ表示牌を追加する
	@param[in] hai ドラ表示牌
***************************************************************************/
void Game::appendDora(const Hai* hai) {
  doras_.push_back(hai);
  onShowHai(hai);
}
/***********************************************************************//**
	@brief 局を開始する
***************************************************************************/
void Game::beginKyoku() {
  for(auto& player : getPlayers()) {
    player->reset();
  }
  doras_.clear();
  resetTurn();
}
/***********************************************************************//**
	@brief 局を終了する
***************************************************************************/
void Game::endKyoku() {
  auto player = getPlayer(getOya());
  if(player->isAgari() || isRyukyoku()) {
    setRenchan(getRenchan() + 1);
    flag_.set(FLAG_RENCHAN);
  }
  else {
    setRenchan(0);
    flag_.reset(FLAG_RENCHAN);
  }
  if(!player->isAgari() && !player->isSayTenpai()) {
    auto oya = getOya() + 1;
    if(oya < countPlayer()) {
      setOya(oya);
    }
    else {
      setOya(0);
      setRound(getRound() + 1);
    }
  }
}
/***********************************************************************//**
	@brief 和了があったか調べる
	@return 和了があったとき真
***************************************************************************/
bool Game::isAgari() const {
  for(auto player : players_) {
    if(player->isAgari()) {
      return true;
    }
  }
  return false;
}
/***********************************************************************//**
	@brief 流局したか調べる
        @return 流局したとき真
***************************************************************************/
bool Game::isRyukyoku() const {
  return !isAgari();
}
/***********************************************************************//**
	@brief 残りのツモ数を取得する
	@return 残りのツモ数
***************************************************************************/
size_t Game::getRest() const {
  return std::numeric_limits<size_t>::max();
}
/***********************************************************************//**
	@brief リーチ可能か調べる
	@return リーチ可能なとき真
***************************************************************************/
bool Game::canRichi() const {
  return getRest() >= countPlayer();
}
/***********************************************************************//**
	@brief 
***************************************************************************/
bool Game::canPon() const {
  return getRest() >= 1;
}
/***********************************************************************//**
	@brief 
***************************************************************************/
bool Game::canChi() const {
  return getRest() >= 1;
}
/***********************************************************************//**
	@brief 終了時の処理
	@param[in] players 着順
***************************************************************************/
void Game::onEndGame(const Players& players) {
  setEnd(true);
}
/***********************************************************************//**
	@brief すべてのプレイヤーをクリアする
***************************************************************************/
void Game::clearPlayer() {
  for(size_t i = 0, n = players_.size(); i < n; i++) {
    if(auto player = players_.at(i)) {
      removePlayer(player);
    }
  }
  players_.clear();
}
/***********************************************************************//**
	@brief 親を変更する
	@param[in] oya 親
***************************************************************************/
void Game::setOya(size_t oya) {
  assert(oya < countPlayer());
  oya_ = oya;
}
/***********************************************************************//**
        $Id$
***************************************************************************/
}
}
