#pragma once

#include <experimental/optional>

#include "tools/Random.h"

#include "ChannelPack.h"
#include "Config.h"

class ManagerChannel {

private:

  std::experimental::optional<ChannelPack> ids;

  emp::Random &local_rng;

  Config & cfg;

  emp::vector<size_t> gen_counter;

  Config::chanid_t drawChannelID() {
    return local_rng.GetUInt64();
  }

public:

  ManagerChannel(
    Config &cfg_,
    emp::Random &local_rng_
  ) : ids(ChannelPack())
  , local_rng(local_rng_)
  , cfg(cfg_)
  , gen_counter(cfg_.NLEV())
  {
    for(size_t i = 0; i < cfg.NLEV(); ++i) {
      ids->push_back(drawChannelID());
    }
  }

  const std::experimental::optional<Config::chanid_t> GetID(size_t lev) {
    return ids ? std::experimental::optional<Config::chanid_t>((*ids)[lev]) : std::experimental::nullopt;
  }

  std::experimental::optional<ChannelPack> GetIDs() { return ids; }

  void ClearIDs() { ids = std::experimental::nullopt; }

  size_t GetGeneration(size_t lev) const { return gen_counter[lev]; }

  const emp::vector<size_t>& GetGenCounter() const { return gen_counter; }

  bool CheckMatch(ManagerChannel &other, size_t lev) {
    return GetID(lev) && other.GetID(lev) ? *GetID(lev) == *other.GetID(lev) : false;
  }

  /* perform a certain level of reproduction */
  void Inherit(
    ChannelPack &parent,
    const emp::vector<size_t> & parent_gen_counter,
    size_t lev
  ) {

    if (!ids) ids = std::experimental::optional<ChannelPack>{ChannelPack(cfg.NLEV())};

    for(size_t i = 0; i < cfg.NLEV(); ++i) {
      if(i < lev) {
        (*ids)[i] = drawChannelID();
        gen_counter[i] = 0;
      } else {
        (*ids)[i] = parent[i];
        gen_counter[i] = parent_gen_counter[i] + 1;
      }
    }
  }

};
