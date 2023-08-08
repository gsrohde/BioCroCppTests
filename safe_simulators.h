#ifndef SAFE_SIMULATORS_H
#define SAFE_SIMULATORS_H

#include "BioCro_Extended.h"

namespace BioCro {

class Safe_simulator
{
   public:
    Safe_simulator(
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

    virtual BioCro::Simulation_result run_simulation() = 0;

   protected:
    BioCro::Dynamical_system sys;
    BioCro::Solver system_solver;
};
    
class Idempotent_simulator : public Safe_simulator
{
    using Safe_simulator::Safe_simulator; // inherit constructor

   public:
    BioCro::Simulation_result run_simulation()
    {
        sys->reset();
        return system_solver->integrate(sys);
    }
};

class Single_use_simulator : public Safe_simulator
{
    using Safe_simulator::Safe_simulator; // inherit constructor

   public:
    BioCro::Simulation_result run_simulation()
    {
        if (has_been_run) {
            throw std::runtime_error("A Single_use_simulator can only be run once.");
        }
        has_been_run = true;
        return system_solver->integrate(sys);
    }

   private:
    bool has_been_run {false};
};

}

#endif
