#ifndef SAFE_SIMULATORS_H
#define SAFE_SIMULATORS_H

// Commented-out includes are included, directly or indirectly, in the
// previous uncommented-out include.
#include <framework/ode_solver_library/ode_solver_factory.h>
// #include <framework/ode_solver.h>
// #include <framework/dynamical_system.h> // for dynamical_system
// #include <framework/state_map.h> // for state_map, state_vector_map
// #include <framework/constants.h> // for math_constants::pi
#include <framework/module_factory.h> // for module_factory
// #include <framework/module_creator.h> // for mc_vector
#include <module_library/module_library.h> // for standardBML::module_library

namespace BioCro {

class Idempotent_simulator
{
   public:
    Idempotent_simulator(
        // parameters passed to dynamical_system constructor
        state_map const& initial_values,
        state_map const& parameters,
        state_vector_map const& drivers,
        mc_vector const& direct_mcs,
        mc_vector const& differential_mcs,
        // parameters passed to ode_solver_factory::create
        std::string ode_solver_name,
        double output_step_size,
        double adaptive_rel_error_tol,
        double adaptive_abs_error_tol,
        int adaptive_max_steps)
    {
        // Create the system
        sys = std::shared_ptr<dynamical_system>(
            new dynamical_system(initial_values, parameters,
                                 drivers, direct_mcs,
                                 differential_mcs));

        // Create the ode_solver that will be used to solve the system
        system_solver =
            std::unique_ptr<ode_solver>(
                ode_solver_factory::create(
                    ode_solver_name,
                    output_step_size,
                    adaptive_rel_error_tol,
                    adaptive_abs_error_tol,
                    adaptive_max_steps));
    }

    state_vector_map run_simulation()
    {
        sys->reset();
        return system_solver->integrate(sys);
    }

   private:
    std::shared_ptr<dynamical_system> sys;
    std::unique_ptr<ode_solver> system_solver;
};

}

#endif
