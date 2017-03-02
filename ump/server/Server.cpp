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
#include "ump/Command.hpp"
#include "ump/Version.hpp"
#include "ump/server/Config.hpp"
#include "ump/server/Game.hpp"
#include "ump/server/Player.hpp"
#include "ump/server/Server.hpp"
#include "ump/socket/Socket.hpp"

namespace ump {
namespace server {
/***********************************************************************//**
	@brief デフォルトコンストラクタ
	@param[in] socket ソケット
	@param[in] port ポート番号
***************************************************************************/
Server::Server(const std::shared_ptr<socket::Socket>& socket, 
               int port)
  : socket_(socket), 
    port_(port), 
    config_(std::make_shared<Config>())
{
  while(!socket_->listen(port_)) {
    std::cerr << socket_->getError() << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  printf("open port %d\n", port);
}
/***********************************************************************//**
	@brief デストラクタ
***************************************************************************/
Server::~Server() {
  stop();
  join();
}
/***********************************************************************//**
	@brief 実行
***************************************************************************/
void Server::start() {
  assert(!thread_);
  thread_.reset(new std::thread(std::ref(*this)));
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void Server::stop() {
  auto games(games_);
  for(auto& game : games) {
    game->stop();
  }
  socket_->close();
}
/***********************************************************************//**
	@brief スレッド終了を待つ
***************************************************************************/
void Server::join() {
  if(thread_) {
    thread_->join();
    thread_.reset();
  }
}
/***********************************************************************//**
	@brief ゲームが終了した
	@param[in] game 終了したゲーム
***************************************************************************/
void Server::onEndGame(Game* game) {
  auto found = std::find_if(games_.begin(), games_.end(), 
                            [game](const std::shared_ptr<Game>& iter) {
                              return iter.get() == game;
                            });
  assert(found != games_.end());
  games_.erase(found);
}
/***********************************************************************//**
	@copydoc Receiver::onReceive
***************************************************************************/
void Server::onReceive(std::shared_ptr<Player> player, 
                       const Command& command) {
  if(command.getType() == Command::TYPE_HELLO) {
    if(!game_) {
      game_ = createGame();
    }
    if(command.hasOption("name")) {
      player->setName(command.getOption("name"));
    }
    game_->appendPlayer(player);
    if(game_->countPlayer() == game_->getConfig().getPlayerNum()) {
      game_->start();
      games_.push_back(game_);
      game_.reset();
    }
  }
}
/***********************************************************************//**
	@brief 
***************************************************************************/
std::shared_ptr<Game> Server::createGame() {
  return std::make_shared<Game>(getConfig(), *this);
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void Server::operator()() {
  while(socket_->isOpen()) {
    if(auto socket = socket_->accept(getTimeout())) {
      auto player = std::make_shared<Player>(socket);
      player->send(Command(Command::TYPE_HELLO).
                   setOption("ump", Version::Get().toString()), 
                   this);
      player->start();
    }
  }
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}