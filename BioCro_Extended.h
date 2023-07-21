#ifndef BIOCRO_EXTENDED_H
#define BIOCRO_EXTENDED_H

#include "testBML/src/module_library/module_library.h"

// Commented-out includes are included, directly or indirectly, in the
// previous uncommented-out include.
#include <framework/module_factory.h> // for module_factory
#include <framework/ode_solver_library/ode_solver_factory.h> // for ode_solver_factory
// #include <framework/ode_solver.h> // for ode_solver

namespace BioCro {
    using Test_BioCro_library_module_factory = module_factory<testBML::module_library>;

    using Dynamical_system = std::shared_ptr<dynamical_system>;

    inline Dynamical_system make_dynamical_system
    (
     state_map const& initial_state,
     state_map const& parameters,
     state_vector_map const& drivers,
     mc_vector const& steady_state_modules,
     mc_vector const& differential_modules
     ) {
        return Dynamical_system(new dynamical_system(
            initial_state,
            parameters,
            drivers,
            steady_state_modules,
            differential_modules));
    }

    using Solver = std::unique_ptr<ode_solver>;

    inline Solver make_ode_solver
    (
     std::string const& ode_solver_name,
     double step_size,
     double rel_error_tol,
     double abs_error_tol,
     int max_steps
     ) {
        return Solver
            (ode_solver_factory::create
             (
              ode_solver_name,
              step_size,
              rel_error_tol,
              abs_error_tol,
              max_steps
              )
             );
    }
}

#include "BioCro.h"

#endif
