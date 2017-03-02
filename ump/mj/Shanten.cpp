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
#include "ump/mj/Shanten.hpp"

namespace ump {
namespace mj {
/***********************************************************************//**
	@brief コンストラクタ
***************************************************************************/
Shanten::Shanten()
  : mentsuMax_(0), 
    shanten_(std::numeric_limits<int>::max())
{}
/***********************************************************************//**
	@brief デストラクタ
***************************************************************************/
Shanten::~Shanten() {
}
/***********************************************************************//**
	@brief シャンテンを計算する
	@param[in] hais 牌の配列
	@param[in] isMenzen 面前のとき真
	@return シャンテン数
***************************************************************************/
int Shanten::update(const HaiArray& hais, bool isMenzen) {
  super::set(hais);
  mentsuMax_ = hais.size() / 3;
  shanten_ = std::numeric_limits<int>::max();
  richi_.clear();
  if(isMenzen) {
    checkKokushi();
    checkChitoi();
  }
  eachHead();
  eachMentsu();
  if(getShanten() < 0) {
    richi_ = getHais();
  }
  richi_.unique();
  return getShanten();
}
/***********************************************************************//**
	@brief シャンテン数を取得する
	@return シャンテン数
***************************************************************************/
int Shanten::getShanten() const {
  return shanten_;
}
/***********************************************************************//**
	@brief リーチをかけられる牌を取得する
	@return リーチをかけられる牌の配列
***************************************************************************/
const HaiArray& Shanten::getRichi() const {
  return richi_;
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void Shanten::onHead() {
  eachMentsu();
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void Shanten::onMentsu() {
  eachTatsu();
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void Shanten::onTatsu() {
  //dump();
  auto mentsuNum = countMentsu();
  auto tatsuNum = countTatsu();
  if(mentsuNum + tatsuNum > mentsuMax_) {
    tatsuNum = mentsuMax_ - mentsuNum;
  }
  int shanten = int(mentsuMax_ - mentsuNum) * 2 - int(tatsuNum);
  if(hasHead()) {
    shanten--;
  }
  updateShanten(shanten);
  if(shanten == 0) {
    if(countHai() == 0 && countTatsu() == 1) {
      richi_.append(getTatsu(0));
    }
    else {
      richi_.append(getHais());
    }
  }
}
/***********************************************************************//**
	@brief シャンテン数を更新する
***************************************************************************/
void Shanten::updateShanten(int shanten) {
  if(shanten < shanten_) {
    shanten_ = shanten;
  }
}
/***********************************************************************//**
	@brief 国士無双のチェック
***************************************************************************/
void Shanten::checkKokushi() {
  HaiArray elem;
  for(size_t i = 0; i < HaiArray::YAOCHU.size(); i++) {
    auto hai = getHais().removeSame(HaiArray::YAOCHU[i]);
    if(hai) {
      elem.append(hai);
    }
  }
  int shanten = 13 - int(elem.size());
  if(getHais().isIncludeYaochu()) {
    shanten--;
  }
  updateShanten(shanten);
  if(shanten == 0) {
    richi_.append(getHais());
  }
  super::pop(elem);
}
/***********************************************************************//**
	@brief 七対子のチェック
***************************************************************************/
void Shanten::checkChitoi() {
  HaiArray uniq = getHais().getUnique();
  HaiArray elem;
  int shanten = (int(getHais().size()) - 1) / 2;
  for(size_t i = 0; i < uniq.size(); i++) {
    const Hai* hai = uniq[i];
    if(getHais().countSame(hai) >= 2) {
      for(int j = 0; j < 2; j++) {
        elem.append(getHais().removeSame(hai));
      }
      shanten--;
    }
  }
  if(uniq.size() < 7) {
    shanten += 7 - uniq.size();
  }
  updateShanten(shanten);
  if(shanten == 0) {
    richi_.append(getHais());
  }
  super::pop(elem);
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void Shanten::dump() const {
  std::cout << "(" << getHead().toString() << ")";
  for(size_t i = 0, n = countMentsu(); i < n; i++) {
    std::cout << "(" << getMentsu(i).toString() << ")";
  }
  for(size_t i = 0, n = countTatsu(); i < n; i++) {
    std::cout << "(" << getTatsu(i).toString() << ")";
  }
  std::cout << getHais().toString() << std::endl;
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}