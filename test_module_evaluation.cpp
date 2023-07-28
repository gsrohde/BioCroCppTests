#ifndef VERBOSE
#define VERBOSE false
#endif


#include <gtest/gtest.h>
#include <gmock/gmock.h> // for matchers

#include <iostream>

#include "BioCro.h"

#include "Random.h"

using namespace std;

using Module_factory = BioCro::Standard_BioCro_library_module_factory;

TEST(ModuleEvaluationTest, simple) {

    Rand_double double_gen { -100, 100 };
    Rand_double pos_double_gen { 1e-5, 100 };

    BioCro::Module_creator w = Module_factory::retrieve("harmonic_oscillator");

    double position {double_gen()};
    double velocity {double_gen()};
    double mass {pos_double_gen()};
    double spring_constant {pos_double_gen()};
                            
    // input_quantities should be a state map
    // use it to initialize the quantity list
    BioCro::Variable_settings quantities {
        {"position", position},
        {"velocity", velocity},
        {"mass", mass},
        {"spring_constant", spring_constant}
    };

    BioCro::Variable_settings module_output_map;

    // Get the module's outputs and add them to the output list with default
    // values of 0.0. Since derivative modules add their output values to
    // the values in module_output_map, the result only makes sense if each
    // parameter is initialized to 0.
    auto module_outputs = w->get_outputs();
    for (string param : module_outputs) {
        module_output_map[param] = 0.0;
    }

    auto module_ptr =
        w->create_module(quantities, &module_output_map);

    module_ptr->run();

    if (VERBOSE) {
        for (auto item : quantities) {
            cout << item.first << ": " << item.second << endl;
        }
        for (string param : module_outputs) {
            cout << param << " derivative: "
                 << module_output_map[param] << endl;
        }
    }

    // dx/dt = v    
    EXPECT_DOUBLE_EQ(module_output_map["position"], velocity);
    // dv/dt = a = -kx/m
    EXPECT_DOUBLE_EQ(module_output_map["velocity"],
                     -spring_constant * position / mass);
}

