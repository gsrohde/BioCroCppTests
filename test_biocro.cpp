#include <gtest/gtest.h>

#include "BioCro.h"
#include "print_result.h"

using Module_factory = BioCro::Standard_BioCro_library_module_factory;

BioCro::Simulator get_simulation() {

    BioCro::State initial_state = { {"position", 0}, {"velocity", 1}};
    BioCro::Parameter_set parameters =
        { {"mass", 10}, {"spring_constant", 0.1}, {"timestep", 1}};
    BioCro::System_drivers drivers =
        { {"time",  { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }} };
    BioCro::Module_set steady_state_modules(0);
    BioCro::Module_set derivative_modules {
        Module_factory::retrieve("harmonic_oscillator")
    };

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

class BioCroSimulationTest : public ::testing::Test {
   protected:
    BioCroSimulationTest() :bs{get_simulation()} {
    }
    BioCro::Simulator bs;

    void trial_simulation() {
        auto result = bs.run_simulation();
        //print_result(result);

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
};

TEST_F(BioCroSimulationTest, CorrectSimulation) {

    ASSERT_EXIT(trial_simulation(),
                ::testing::ExitedWithCode(0),
                ".*")
        << "Either there was a segmentation fault"
        " or the time variable values are incorrect in the result.";
}
