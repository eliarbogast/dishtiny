// This is the main function for the NATIVE version of this project.

#include "conduit/include/uitsl/mpi/MpiGuard.hpp"
#include "conduit/include/uitsl/parallel/ThreadTeam.hpp"

#include "dish2/config/make_arg_specs.hpp"
#include "dish2/config/setup.hpp"
#include "dish2/spec/Spec.hpp"
#include "dish2/run/thread_job.hpp"
#include "dish2/world/ProcWorld.hpp"

const uitsl::MpiGuard mpi_guard;

using Spec = dish2::Spec;

int main(int argc, char* argv[]) {

  dish2::setup( emp::ArgManager{ argc, argv, dish2::make_arg_specs() } );

  dish2::ProcWorld<Spec> proc_world;

  uitsl::ThreadTeam team;

  // launch threads to run simulation
  for( size_t thread{}; thread < dish2::cfg.N_THREADS(); ++thread ) team.Add(
    [thread, &proc_world](){ dish2::thread_job<Spec>(
      thread, proc_world.MakeThreadWorld( thread )
    ); }
  );

  // wait for threads to complete
  team.Join();

  return 0;

}
