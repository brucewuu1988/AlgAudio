/*
This file is part of AlgAudio.

AlgAudio, Copyright (C) 2015 CeTA - Audiovisual Technology Center

AlgAudio is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

AlgAudio is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with AlgAudio.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "Signal.hpp"
#include <iostream>
#include <algorithm>

namespace AlgAudio{

int SignalBase::subscription_id_counter = 1;

Subscription::Subscription(Subscription&& other)
  : id(std::move(other.id)),
    target(std::move(other.target)) {
  other.id = 0;
  other.target = nullptr;
  if(target) target->SubscriptionAddressChanged(&other,this);
}

Subscription& Subscription::operator=(Subscription&& other) {
  //std::swap(id, other.id);
  //std::swap(target, other.target);
  if(!IsEmpty()) Release();
  id = std::move(other.id);
  target = std::move(other.target);
  other.id = 0;
  other.target = nullptr;
  if(target) target->SubscriptionAddressChanged(&other,this);
  return *this;
}

void Subscription::Release(){
  if(!IsEmpty()){
    //std::cout << "Releasing subscribtion " << id << " targetting " << target << std::endl;
    if(target) target->RemoveSubscriptionByID(id, this);
    target = nullptr;
    id = 0;
    //std::cout << "Relased." << std::endl;
  }
}

SignalBase::SignalBase(){
  //std::cout << "Created signal " <<  this << std::endl;
}

SignalBase::~SignalBase(){
  //std::cout << "Destroying signal " <<  this << std::endl;
  for(auto& p : subscriptions) p->target = nullptr;
}


void SignalBase::SubscriptionAddressChanged(Subscription* old, Subscription* n){
  std::replace (subscriptions.begin(), subscriptions.end(), old, n);
}


} // namespace AlgAudio
