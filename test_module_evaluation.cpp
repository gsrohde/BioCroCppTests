#include <gtest/gtest.h>
#include <gmock/gmock.h> // for matchers

#include <iostream>

#include <framework/dynamical_system.h> // for dynamical_system
// #include <framework/state_map.h> // for state_map, state_vector_map
#include <framework/module_factory.h> // for module_factory
// #include <framework/module_creator.h> // for mc_vector
#include <module_library/module_library.h> // for standardBML::module_library
#include <framework/ode_solver_library/ode_solver_factory.h> // for ode_solver_factory
// #include <framework/ode_solver.h> // for ode_solver

#include "Random.h"

using namespace std;

TEST(ModuleEvaluationTest, simple) {

    Rand_double double_gen { -100, 100 };
    Rand_double pos_double_gen { 1e-5, 100 };

   
    module_creator* w = module_factory<standardBML::module_library>::retrieve("harmonic_oscillator");

    double position {double_gen()};
    double velocity {double_gen()};
    double mass {pos_double_gen()};
    double spring_constant {pos_double_gen()};
                            
    // input_quantities should be a state map
    // use it to initialize the quantity list
    state_map quantities = { {"position", position},
                             {"velocity", velocity},
                             {"mass", mass},
                             {"spring_constant", spring_constant},
                             {"timestep", 1} };

    state_map module_output_map;

    // Get the module's outputs and add them to the output list with default
    // values of 0.0. Since derivative modules add their output values to
    // the values in module_output_map, the result only makes sense if each
    // parameter is initialized to 0.
    string_vector module_outputs = w->get_outputs();
    for (string param : module_outputs) {
        module_output_map[param] = 0.0;
    }

    std::unique_ptr<module> module_ptr =
        w->create_module(quantities, &module_output_map);

    module_ptr->run();

    for (auto item : quantities) {
        cout << item.first << ": " << item.second << endl;
    }
    for (string param : module_outputs) {
        cout << param << " derivative: " << module_output_map[param] << endl;
    }

    // dx/dt = v    
    EXPECT_DOUBLE_EQ(module_output_map["position"], velocity);
    // dv/dt = a = -kx/m
    EXPECT_DOUBLE_EQ(module_output_map["velocity"], -spring_constant * position / mass);
}

