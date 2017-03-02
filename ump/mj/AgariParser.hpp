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

#include "ump/mj/Agari.hpp"
#include "ump/mj/Divider.hpp"

namespace ump {
namespace mj {
/***********************************************************************//**
	@brief 和了パーサー
***************************************************************************/
class AgariParser
  : public Divider
{
  typedef Divider super;

 private:
  enum {
    FLAG_RON, 
    FLAG_MAX
  };

 private:
  const Player& player_;
  Agari agari_;
  const Hai* agariHai_;
  std::bitset<FLAG_MAX> flag_;

 public:
  AgariParser(const Player& player);
  ~AgariParser() override = default;

  const Agari& parse(const Hai* ron);

 protected:
  void onHead() override;
  void onMentsu() override;

 private:
  const Player& getPlayer() const {
    return player_;
  }

  bool isRon() const;
  void update();
  bool isKokushi() const;
  void checkChitoi();
  void checkAgariHai(Agari& agari);
};
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}