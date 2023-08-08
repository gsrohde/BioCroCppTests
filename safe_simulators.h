#ifndef SAFE_SIMULATORS_H
#define SAFE_SIMULATORS_H

#include "BioCro_Extended.h"

namespace BioCro {

class Idempotent_simulator
{
   public:
    Idempotent_simulator(
        // parameters passed to dynamical_system constructor
        BioCro::State const& initial_values,
        BioCro::Parameter_set const& parameters,
        BioCro::System_drivers const& drivers,
        BioCro::Module_set const& direct_mcs,
        BioCro::Module_set const& differential_mcs,
        // parameters passed to ode_solver_factory::create
        std::string ode_solver_name,
        double output_step_size,
        double adaptive_rel_error_tol,
        double adaptive_abs_error_tol,
        int adaptive_max_steps)
    {
        // Create the system
        sys = make_dynamical_system(initial_values, parameters,
                                 drivers, direct_mcs,
                                 differential_mcs);

        // Create the ode_solver that will be used to solve the system
        system_solver =
            make_ode_solver(
                    ode_solver_name,
                    output_step_size,
                    adaptive_rel_error_tol,
                    adaptive_abs_error_tol,
                    adaptive_max_steps);
    }

    BioCro::Simulation_result run_simulation()
    {
        sys->reset();
        return system_solver->integrate(sys);
    }

   private:
    BioCro::Dynamical_system sys;
    BioCro::Solver system_solver;
};

}

#endif