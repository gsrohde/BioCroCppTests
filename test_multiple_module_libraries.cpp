// Here we test the use of multiple module libraries at once.
//
// We show that we can use two modules from different module libraries
// that have the same name as long as they are otherwise compatible.
// On the other hand, identical direct modules from different
// libraries will conflict since their outputs overlap.

// Compile with the flag -DVERBOSE=true to get verbose output.
#ifndef VERBOSE
#define VERBOSE false
#endif

#include <gtest/gtest.h>

#include "BioCro_Extended.h"
#include "print_result.h"

using Module_factory = BioCro::Standard_BioCro_library_module_factory;
using Module_factory_2 = BioCro::Test_BioCro_library_module_factory;

class MultipleModuleLibrariesTest : public ::testing::Test {
   protected:
    MultipleModuleLibrariesTest() :bs{get_simulator()} {
    }

    void trial_simulation() {
        auto result = bs.run_simulation();
        if (VERBOSE) print_result(result);

        for (auto i = 0; i < result["time"].size(); ++i) {
            bool eq {result["time"][i] == i};
            if (!eq) {
                cout << "result[\"time\"][" << i << "] (" << result["time"][i]
                     << ") != i (" << i << ")." << endl;
                exit(1);
            }
        }

        exit(0);
    }

 private:
    BioCro::Simulator get_simulator();

    BioCro::State initial_state { {"position", 0}, {"velocity", 1}, {"elapsed_time", 0}, {"TTc", 0} };
    BioCro::Parameter_set parameters
        { {"mass", 10}, {"spring_constant", 0.1}, {"timestep", 1},
          {"lat", 44}, {"longitude", -121}, {"time_zone_offset", -8}, {"year", 2023},
          {"sowing_time", 0}, {"tbase", 10} };
    BioCro::System_drivers drivers
        { {"time", { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 } },
          {"temp", { 5, 8, 10, 15, 20, 20, 25, 30, 32, 40} } };
    BioCro::Module_set steady_state_modules {
        //Module_factory::retrieve("solar_position_michalsky"),
        Module_factory_2::retrieve("solar_position_michalsky")
    };
    BioCro::Module_set derivative_modules {
        Module_factory::retrieve("harmonic_oscillator"),
        Module_factory_2::retrieve("elapsed_time_module"),
        Module_factory::retrieve("thermal_time_linear"),
        Module_factory_2::retrieve("thermal_time_linear")
    };

    // This must be declared last because its initialization depends
    // on the parameters for its construction being initialized first:
    BioCro::Simulator bs;
};

BioCro::Simulator MultipleModuleLibrariesTest::get_simulator() {
    return BioCro::Simulator {
        initial_state,
        parameters,
        drivers,
        steady_state_modules,
        derivative_modules,
        "homemade_euler",
        1,
        0.0001,
        0.0001,
        200
    };
}

TEST_F(MultipleModuleLibrariesTest, CorrectSimulation) {
    ASSERT_EXIT(trial_simulation(),
                ::testing::ExitedWithCode(0),
                ".*")
        << "Either there was a segmentation fault"
        " or the time variable values are incorrect in the result.";
}
