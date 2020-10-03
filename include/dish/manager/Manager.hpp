#pragma once
#ifndef DISH_MANAGER_MANAGER_HPP_INCLUDE
#define DISH_MANAGER_MANAGER_HPP_INCLUDE

#include <algorithm>

#include "../../../third-party/Empirical/source/tools/Random.h"

#include "../config/Config.hpp"
#include "../world/DishWorld.hpp"
#include "ManagerApoptosis.hpp"
#include "ManagerChannel.hpp"
#include "ManagerConnection.hpp"
#include "ManagerHeir.hpp"
#include "ManagerFamily.hpp"
#include "ManagerInbox.hpp"
#include "ManagerPriority.hpp"
#include "ManagerSharing.hpp"
#include "ManagerStockpile.hpp"
#include "ManagerWave.hpp"

namespace dish {

class DishWorld;

class Manager {

private:
  DishWorld &dw;
  emp::vector<emp::Ptr<ManagerApoptosis>> mas;
  emp::vector<emp::Ptr<ManagerChannel>> mcs;
  emp::vector<emp::Ptr<ManagerConnection>> mcos;
  emp::vector<emp::Ptr<ManagerFamily>> mfs;
  emp::vector<emp::Ptr<ManagerHeir>> mhs;
  emp::vector<emp::Ptr<ManagerInbox>> mis;
  emp::vector<emp::Ptr<ManagerPriority>> mps;
  emp::vector<emp::Ptr<ManagerSharing>> mshs;
  emp::vector<emp::Ptr<ManagerStockpile>> mss;
  emp::vector<emp::vector<emp::vector<emp::Ptr<ManagerWave>>>> mws;


public:

  Manager() = delete;

  Manager(
    DishWorld &dw_,
    emp::vector<emp::Ptr<emp::Random>> &local_rngs,
    emp::vector<emp::Ptr<emp::Random>> &global_rngs,
    const Config &cfg
  );

  ~Manager();

  ManagerApoptosis& Apoptosis(size_t pos);

  ManagerChannel& Channel(size_t pos);

  const ManagerChannel& Channel(const size_t pos) const;

  ManagerConnection& Connection(size_t pos);

  ManagerFamily& Family(size_t pos);

  ManagerHeir& Heir(size_t pos);

  ManagerInbox& Inbox(size_t pos);

  ManagerPriority& Priority(size_t pos);

  const ManagerPriority& Priority (size_t pos) const;

  ManagerSharing& Sharing(size_t pos);

  ManagerStockpile& Stockpile(size_t pos);

  ManagerWave& Wave(size_t rep, size_t pos, size_t lev);

  DishWorld &DW();

};

} // namespace dish

#endif // #ifndef DISH_MANAGER_MANAGER_HPP_INCLUDE