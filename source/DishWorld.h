#pragma once

#include <utility>

#include "base/vector.h"
#include "Evolve/World.h"
#include "tools/Random.h"
#include "tools/hash_utils.h"

#include "Cardi.h"
#include "CellFrame.h"
#include "Config.h"
#include "Genome.h"
#include "Manager.h"
#include "Mutator.h"

class DishWorld : public emp::World<Genome> {

private:

  Config &cfg;

  emp::vector<emp::Ptr<emp::Random>> global_rngs;
  emp::vector<emp::Ptr<emp::Random>> local_rngs;
  emp::vector<emp::Ptr<Config::hardware_t>> cpus;

  Mutator mut;
  emp::Ptr<Manager> man;

public:
  DishWorld(Config &cfg_, size_t uid_offset=0)
  : cfg(cfg_), mut(cfg_) {
    SetPopStruct_Grid(cfg.GRID_W(), cfg.GRID_H(), true);

    for(size_t i = 0; i < GetSize(); ++i) {
      local_rngs.push_back(
        emp::NewPtr<emp::Random>(emp::szudzik_hash(cfg.SEED(),i+uid_offset))
      );
      global_rngs.push_back(emp::NewPtr<emp::Random>(cfg.SEED()));
    }

    man = new Manager(local_rngs, global_rngs, cfg);

    for(size_t i = 0; i < GetSize(); ++i) {
      cpus.push_back(emp::NewPtr<Config::hardware_t>());
      cpus[i]->PushTrait(emp::NewPtr<CellFrame>(
        local_rngs[i],
        cfg,
        *man,
        i
      ));
    }

    for(size_t i = 0; i < GetSize(); ++i) {
      Genome g(*local_rngs[i], cfg);
      InjectAt(g, emp::WorldPosition(i,1));
    }

    OnOffspringReady(
      [this](Genome& g, size_t pos){
        return mut.ApplyMutations(g.program,*local_rngs[pos]);
      }
    );

    OnOrgDeath([this](size_t pos){
      // emp_assert(cpus[pos]);
      // cpus[pos]->Reset();
    });

    OnPlacement([this](size_t pos){
      cpus[pos]->Reset();
      cpus[pos]->SetProgram(GetOrg(pos).program);
    });

    OnUpdate([this](size_t upd){
      Pre();
      Mid();
      Post();
    });

  }

  void Pre() {
    for(size_t i = 0; i < GetSize(); ++i) {
      if (IsOccupied(i)) {
        for(size_t l = 0; l < cfg.NLEV(); ++l) {
          man->Wave(i,l).CalcNext();
          man->Wave(i,l).HarvestResource();
        }
      }
    }
  }

  void Mid() {
    for(size_t i = 0; i < GetSize(); ++i) {
      if (IsOccupied(i)) cpus[i]->Process(cfg.HARDWARE_STEPS());
    }
  }

  void Post() {
    for(size_t i = 0; i < GetSize(); ++i) {
      auto optional_tup = man->Priority(i).QueryPendingGenome();
      if(optional_tup) {
        auto [ g, chanpack, rep_lev, par_pos ] = *(optional_tup);
        AddOrgAt(g, i, par_pos);
        man->Channel(i).Inherit(chanpack,rep_lev);
      }
      if (IsOccupied(i)) {
        man->Priority(i).Reset();
        man->Stockpile(i).ResolveExternalContributions();
        for(size_t l = 0; l < cfg.NLEV(); ++l) man->Wave(i,l).ResolveNext();
      }
    }
  }

};