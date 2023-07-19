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

    using Solver = std::unique_ptr<ode_solver>;

    inline std::unique_ptr<ode_solver> make_ode_solver
    (
     std::string const& ode_solver_name,
     double step_size,
     double rel_error_tol,
     double abs_error_tol,
     int max_steps
     ) {
        return std::unique_ptr<ode_solver>
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
