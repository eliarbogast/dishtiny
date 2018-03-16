#pragma once

#include <iostream>

#include "base/vector.h"
#include "base/Ptr.h"
#include "tools/Random.h"
#include "tools/random_utils.h"
#include "Evolve/World.h"
#include "base/assert.h"

#include "ChannelManager.h"
#include "CustomConfig.h"
#include "DishtinyConfig.h"
#include "Organism.h"
#include "ResourceManager.h"
#include "SignalManager.h"
#include "StoreManager.h"

class Simulation {
private:
  const size_t nupdate;

  emp::Random rand;

  emp::World<Organism> world;

  GridSpec spec;

  ChannelManager channel;
  ResourceManager resource;
  SignalManager signal;
  StoreManager store;

  const size_t NLEV;
  const size_t GRID_A;
  const double REP_THRESH;

  emp::vector<size_t> shuffler;
  emp::vector<size_t> neighborshuffler;

  size_t birth_loc;

public:
  Simulation(
    int _nupdate,
    int seed,
    DishtinyConfig& dconfig,
    CustomConfig& cconfig)
  : nupdate(_nupdate)
  , rand(seed)
  , world(rand)
  , spec(dconfig)
  , channel(dconfig, spec, &rand)
  , resource(dconfig, cconfig, spec, &rand)
  , signal(dconfig, spec)
  , store(dconfig, cconfig, spec)
  , NLEV(dconfig.NLEV())
  , GRID_A(dconfig.GRID_H()*dconfig.GRID_W())
  , REP_THRESH(dconfig.REP_THRESH())
  , shuffler(emp::GetPermutation(rand, GRID_A))
  , neighborshuffler(emp::GetPermutation(rand, 4))
  {
    // configure world object
    SetupWorld(dconfig, cconfig);
  }

  /*
   * Take a single Update step. Return false if nupdate exceeded, else true.
   */
  inline bool Step() {
    std::cout << "update " << world.GetUpdate() << std::endl;

    signal.Propagate(channel, world);

    resource.TryReseed(world.GetUpdate(), signal, channel, world);

    resource.LayResource(world.GetUpdate());

    store.Harvest(signal, resource, channel, world);

    store.PayStateCost(signal, resource, channel, world);

    store.SettlePools(channel);

    store.KillDebtors(world);

    // individual reproduction, paid for by own stockpile
    emp::Shuffle(rand, shuffler);
    for (size_t i = 0; i < GRID_A; ++i) {
      store.TransactStockpile(
        shuffler[i],
        TryReproduceCell(shuffler[i], store.CheckStockpile(shuffler[i]))
      );
    }

    // pool reproduction, just like individual reproduction but paid for by
    // channel resource pools
    for (size_t lev = 0; lev < NLEV; ++lev) {

      // we need to use a copy of the channel list
      // because reproduction occurs during iteration
      emp::vector<int> temp;
      channel.CopyChannelList(lev, temp);

      for (size_t memb = 0; memb < temp.size(); ++memb) {
        int ch = temp[memb];
        if (channel.ChannelExists(lev, ch)) {
          ReproducePool(lev, ch);
        }
      }

    }

    world.Update();

    return (world.GetUpdate() < nupdate);
  }

  /*
   * Force the simulation to complete len steps at a time.
   */
  inline bool Steps(size_t len) {

    bool res = false;
    for (size_t i = 0; i < len; ++i) res = Step();

    return res;

  }

  /*
   * Run the simulation until nupdate exceeded.
   */
  inline void Run() {
    while (Step());
  }

private:
  /*
   * Actually reproduce a cell.
   * NOTE: this does function does not calculate the cost of reproduction or
   * levy that cost. These responsibilies are left to the caller.
   */
  inline void ReproduceCell(size_t parent, size_t dest, size_t off_level, double endowment) {

    Organism *child = new Organism(world.GetOrg(parent));
    child->DoMutations(rand);

    // takes care of killing trampled cell
    // birth_loc is hooked into DoBirth function through Lambda in this scope
    birth_loc = dest;
    world.DoBirth(*child, parent);

    // copy over channels on levels at and above off_level
    channel.Spawn(parent, dest, off_level);

    // give endowment
    store.MultilevelTransaction(world.GetOrg(dest), dest, channel, endowment);

  }

  /*
   * Check endowment requirements and channel size caps to determine the
   * hierarchical level on which a cell wishes to reproduce. During
   * reproduction, channels on levels at and above off_level will be copied
   * to the daugher cell. To reproduce at a parcticular level, avail_resource
   * must be sufficient to pay the endowment and reproduction cost AND the
   * organism's cap on channel population size must not be exceeded for that
   * level and all levels above.
   * If these critieria are met for no off_level, then one past the last valid
   * off_level is returned.
   */
  inline size_t pick_off_level(size_t cell, double avail_resource) {

    emp_assert(world.IsOccupied(cell));

    const Organism& org = world.GetOrg(cell);

    // check endowments and caps
    for (size_t tlev = 0; tlev < NLEV + 1; ++tlev) {
      bool res = true;

      // check cap
      for (size_t i = tlev; i < NLEV; ++i) {
        res &= (
            channel.GetCensusCount(i, channel.GetChannel(i, cell))
            < org.GetOffChCap(i)
          );
      }

      // check endowment
      res &= (avail_resource >= org.GetEndowment(tlev) + REP_THRESH);

      // choose lowest viable level for reproduction
      if (res) {
        return tlev;
      }
    }

    // on failure, return one past maximum valid reproduction level
    // where viable reproduction levels 0-NLEV inclusive are valid
    return NLEV+1;
  }

  /*
   * Pick a neighboring cell to place offspring in. Return GRID_A (one past
   * maximum valid cell) on failure.
   */
  inline size_t pick_off_dest(size_t cell) {
    bool viable[4];
    for (size_t n = 0; n < 4; ++n) {
      viable[n] = true;
    }

    const size_t x = spec.GetX(cell);
    const size_t y = spec.GetY(cell);

    const Organism& org = world.GetOrg(cell);

    // at each level, decide if want to exclude cells that match own channel
    for (size_t lev = 0; lev < NLEV; ++lev) {
      if (org.GetAvoidOver(lev) > rand.GetDouble()) {
        int cur_ch = channel.GetChannel(lev, cell);

        viable[0] &= (channel.GetChannel(lev,x,y+1) != cur_ch);
        viable[1] &= (channel.GetChannel(lev,x,y-1) != cur_ch);
        viable[2] &= (channel.GetChannel(lev,x+1,y) != cur_ch);
        viable[3] &= (channel.GetChannel(lev,x-1,y) != cur_ch);
      }
    }

    // neighborshuffler is {0,1,2,3}
    emp::Shuffle(rand, neighborshuffler);

    // proceed through neighbors in random order,
    // returning first index that is valid
    for (size_t n = 0; n < 4; ++n) {
      if(viable[neighborshuffler[n]]) {
        switch(neighborshuffler[n]) {
          case 0 :
            return spec.GetID(x,y+1);
          case 1 :
            return spec.GetID(x,y-1);
          case 2 :
            return spec.GetID(x+1,y);
          case 3 :
            return spec.GetID(x-1,y);;
        }
      }

    }

    // on failure, return one past maximum valid cell index
    return GRID_A;
  }

  /*
   * Return 0.0 for failure or cost of reproduction (incl. endowment) if
   * reproduction succeeds. Take care of reproduction on success.
   */
  inline double TryReproduceCell(size_t cell, double avail_resource) {
    // is the cell alive?
    if (!world.IsOccupied(cell)) {
      return 0.0;
    }

    // did the cell pick a viable neighboring cell to place its offspring in?
    size_t off_dest = pick_off_dest(cell);
    if (off_dest == GRID_A) {
      return 0.0;
    }

    // did the cell pick a hierarchical level to reproduce on?
    // (this is where endowment requirements and channel size caps are checked)
    size_t off_level = pick_off_level(cell, avail_resource);
    // 0 thru NLEV are valid responses, NLEV + 1 returned on fail
    if (off_level == NLEV + 1) {
      return 0.0;
    }

    // from this point onwards, we're cleared for reproduction
    double endow = world.GetOrg(cell).GetEndowment(off_level);

    ReproduceCell(cell, off_dest, off_level, endow);

    return -REP_THRESH - endow;
  }


  /*
   * In the ChannelManager's Census, organisms in each channel are organized
   * from close to channel clump center to far from channel clump center. If
   * pool resource exceeds REP_THRESH, give each cell a chance to reproduce in
   * order from center outwards. Once someone reproduces, if there's still
   * enough resource then go back through the (re-sorted) list until nobody
   * reproduces.
   */
  inline void ReproducePool(size_t lev, int ch) {

    double res;
    do {

      res = 0.0;
      for (
        size_t memb = 0;
        (memb < channel.GetCensusCount(lev, ch) &&
        res == 0.0 &&
        store.CheckPool(lev,ch) >= REP_THRESH);
        ++memb
        ) {

          res = TryReproduceCell(
            channel.GetCensusCell(lev,ch,memb),
            store.CheckPool(lev,ch)
            );
          store.TransactPool(lev, ch, res);

      }
    } while (store.CheckPool(lev, ch) >= REP_THRESH && channel.GetCensusCount(lev,ch) > 0 && res != 0.0);

  }

  /*
   * Configure World object.
   */
  inline void SetupWorld(DishtinyConfig& dconfig, CustomConfig& cconfig) {
    // this also sets the world to asynchronous mode
    world.SetGrid(dconfig.GRID_W(), dconfig.GRID_H());

    world.OnOrgDeath( [this](size_t pos) {
      // set channels to DEAD
      // if org was last of particular channel, remove that channel's res pool
      channel.Kill(
        pos,
        [this](size_t lev, int ch){ store.ErasePool(lev, ch); }
      );
      // erase organism's resource stockpile
      store.EraseStockpile(pos);
    } );

    world.SetAddBirthFun(
      [this](emp::Ptr<Organism> new_org, size_t parent_id) {
        // kill old organism if necessary, place new organism
        world.AddOrgAt(new_org, birth_loc);
        return birth_loc;
    } );

    // populate the world
    for (size_t cell = 0; cell < GRID_A; ++cell) {
      Organism *org = new Organism(&rand, dconfig, &cconfig);
      world.InjectAt(*org, cell);
    }

  }

};
