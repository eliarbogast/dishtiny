#pragma once

#include "base/vector.h"
#include "tools/Random.h"

#include "CustomConfig.h"
#include "DishtinyConfig.h"

class Organism {
private:

  // pointer for quick and efficient reproduction
  // (shared between all organisms)
  emp::Ptr<emp::Random> rand;

  // genetic information...
  // ... how much resource should I endow an offspring when I reproduce
  // at a particular hierarchical level
  emp::vector<double> endowments;
  // ... what proportions of resource should I place into channel pools of
  // different hierarchical levels and what proportion should I place into my
  // own stockpile? (values here are always normalized to sum to 1)
  emp::vector<double> res_pools;
  // ... with what probability should I avoid reproducing over cells
  // that share my channel on a particular level?
  // (1 = always avoid this , 0 = don't care )
  emp::vector<double> avoid_overs;
  // ... how big can the population of same-channel cells on a particular level
  // get before I decide to reproduce on a higher hierarchical level?
  emp::vector<double> off_ch_caps;

  // pointer for quick and efficient reproduction
  // (shared between all organisms)
  emp::Ptr<CustomConfig> cconfig;

public:
  Organism(
    emp::Ptr<emp::Random> _r,
    DishtinyConfig& dconfig,
    emp::Ptr<CustomConfig> _cconfig)
  : rand(_r)
  , endowments(dconfig.NLEV()+1)
  , res_pools(dconfig.NLEV()+1)
  , avoid_overs(dconfig.NLEV())
  , off_ch_caps(dconfig.NLEV())
  , cconfig(_cconfig)
  {
    // initialize genetic information
    for (size_t lev = 0; lev < dconfig.NLEV(); ++lev) {
      off_ch_caps[lev] = init_off_ch_cap(lev);
      endowments[lev] = init_endowment(lev);
      res_pools[lev] = init_res_pool(lev);
      avoid_overs[lev] = init_avoid_over(lev);
    }

    // these genotypic characteristics have NLEV+1 values...
    // ... for endowments when changing none, some, or all channel values
    endowments[dconfig.NLEV()] = init_endowment(dconfig.NLEV());
    // ... to share between own stockpile and resource pools over all levels
    res_pools[dconfig.NLEV()] = init_res_pool(dconfig.NLEV());

    // balance res_pools so values sum to 1
    bal_res_pools();

  }

  /*
   * Make an exact copy of a parent Organism.
   */
  Organism(const Organism& par)
  : rand(par.rand)
  , endowments(par.endowments.size())
  , res_pools(par.res_pools.size())
  , avoid_overs(par.avoid_overs.size())
  , off_ch_caps(par.off_ch_caps.size())
  , cconfig(par.cconfig)
  {
    // initialize genetic information from parent
    for (size_t lev = 0; lev < endowments.size(); ++lev) {
      endowments[lev] = par.endowments[lev];
    }

    for (size_t lev = 0; lev < res_pools.size(); ++lev) {
      res_pools[lev] = par.res_pools[lev];
    }

    for (size_t lev = 0; lev < avoid_overs.size(); ++lev) {
      avoid_overs[lev] = par.avoid_overs[lev];
    }

    for (size_t lev = 0; lev < off_ch_caps.size(); ++lev) {
      off_ch_caps[lev] = par.off_ch_caps[lev];
    }

  }

  /*
   * Equality determined by identity (via comparison of object addresses).
   */
  inline bool operator==(const Organism& other) const {
    return (this == &other);
  }

  /*
   * Equality determined by identity (via comparison of object addresses).
   */
  bool operator!=(const Organism& other) const {
    return !(operator==(other));
  }

  /*
   * Potentially mutate genetic information of this Organism. (Whether mutation
   * is actually performed is determined by RNG results.)
   */
  inline void DoMutations(emp::Random& x) {
    // mutate genetic information
    for (size_t lev = 0; lev < endowments.size(); ++lev) {
      endowments[lev] = mut_endowment(endowments[lev], lev);
    }

    for (size_t lev = 0; lev < res_pools.size(); ++lev) {
      res_pools[lev] = mut_res_pool(res_pools[lev], lev);
    }

    for (size_t lev = 0; lev < avoid_overs.size(); ++lev) {
      avoid_overs[lev] = mut_avoid_over(avoid_overs[lev], lev);
    }

    for (size_t lev = 0; lev < off_ch_caps.size(); ++lev) {
      off_ch_caps[lev] = mut_off_ch_cap(off_ch_caps[lev], lev);
    }

    bal_res_pools();

  }

  /*
   * No cleanup required --- random generator, config pointers are shared
   * between all organisms.
   */
  ~Organism() { ; }

  /*
   * Accessor function.
   */
  inline double GetEndowment(size_t lev) const {
    return endowments[lev];
  }

  /*
   * Accessor function.
   */
  inline size_t GetEndowmentDepth() const {
    return endowments.size();
  }

  /*
   * Accessor function.
   */
  inline double GetResPool(size_t lev) const {
    return res_pools[lev];
  }

  /*
   * Accessor function.
   */
  inline size_t GetResPoolDepth() const {
    return res_pools.size();
  }

  /*
   * Accessor function.
   */
  inline double GetAvoidOver(size_t lev) const {
    return avoid_overs[lev];
  }

  /*
   * Accessor function.
   */
  inline size_t GetAvoidOverDepth() const {
    return avoid_overs.size();
  }

  /*
   * Accessor function.
   */
  inline double GetOffChCap(size_t lev) const {
    return off_ch_caps[lev];
  }

  /*
   * Accessor function.
   */
  inline size_t GetOffChCapDepth() const {
    return off_ch_caps.size();
  }

private:
  /*
   * Make sure res_pools elements sum to 1.
   */
  inline void bal_res_pools() {
    // we can assume res_pools values are non-negative
    double s = 0.0;
    for (size_t i = 0; i < res_pools.size(); ++i) {
      s += res_pools[i];
    }
    if (s > 0) {
      for (size_t i = 0; i < res_pools.size(); ++i) {
        res_pools[i] /= s;
      }
    } else {
      for (size_t i = 0; i < res_pools.size(); ++i) {
        res_pools[i] = 1.0 / res_pools.size();
      }
    }
  }

  /*
   * Initialization function.
   */
  inline double init_off_ch_cap(size_t lev) {
    return rand->GetDouble(48.0);
  }

  /*
   * Initialization function.
   */
  inline double init_endowment(size_t lev) {
    return rand->GetDouble(3.0);
  }

  /*
   * Initialization function.
   */
  inline double init_res_pool(size_t lev) {
    return rand->GetDouble(1.0);
  }

  /*
   * Initialization function.
   */
  inline double init_avoid_over(size_t lev) {
    return std::max(std::min(rand->GetDouble(-0.5, 1.5), 1.0), 0.0);
  }

  /*
   * Draw double from random generator. If it says mutate value, return mutated
   * value. Else, return original value.
   */
  inline double mut_off_ch_cap(double cur, size_t lev) {
    double rval = cur;
    if (rand->GetDouble() < cconfig->PM_OFF_CH_CAP[lev]) {
      rval = std::max(cur + rand->GetRandNormal(0.0, 24.0), 0.0);
    }
    return rval;
  }

  /*
   * Draw double from random generator. If it says mutate value, return mutated
   * value. Else, return original value.
   */
  inline double mut_endowment(double cur, size_t lev) {
    double rval = cur;
    if (rand->GetDouble() < cconfig->PM_ENDOWMENT[lev]) {
      rval = std::max(cur + rand->GetRandNormal(0.0, 10.0), 0.0);
    }
    return rval;
  }

  /*
   * Draw double from random generator. If it says mutate value, return mutated
   * value. Else, return original value.
   */
  inline double mut_res_pool(double cur, size_t lev) {
    double rval = cur;
    if (rand->GetDouble() < cconfig->PM_RES_POOL[lev]) {
      rval = std::max(cur + rand->GetRandNormal(0.0, 0.2), 0.0);
    }
    return rval;
  }

  /*
   * Draw double from random generator. If it says mutate value, return mutated
   * value. Else, return original value.
   */
  inline double mut_avoid_over(double cur, size_t lev) {
    double rval = cur;
    if (rand->GetDouble() < cconfig->PM_AVOID_OVER[lev]) {
      rval = init_avoid_over(lev);
    }
    return rval;
  }

};
