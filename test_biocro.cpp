#include <gtest/gtest.h>

#include <framework/biocro_simulation.h>
#include <framework/module_factory.h>
#include <module_library/module_library.h>
#include "print_result.h"

biocro_simulation get_simulation() {

    state_map initial_state = { {"position", 0}, {"velocity", 1}};
    state_map invariant_parameters = { {"mass", 10}, {"spring_constant", 0.1}, {"timestep", 1}};
    state_vector_map drivers = { {"time",  { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }} };
    mc_vector steady_state_modules(0);
    mc_vector derivative_modules { module_factory<standardBML::module_library>::retrieve("harmonic_oscillator") };

    return biocro_simulation {
        initial_state,
        invariant_parameters,
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

class biocro_simulationTest : public ::testing::Test {
   protected:
    biocro_simulationTest() :bs{get_simulation()} {
    }
    biocro_simulation bs;

    void trial_simulation() {
        auto result = bs.run_simulation();
        print_result(result);

        for (auto i = 0; i < result["time"].size(); ++i) {
            bool eq {result["time"][i] == i};
            if (!eq) {
                cout << "result[\"time\"][" << i << "] (" << result["time"][i] << ") != i (" << i << ")." << endl;
                exit(1);
            }
        }

        //exit(0);
    }
};

TEST_F(biocro_simulationTest, DISABLED_CorrectSimulation) {

    ASSERT_EXIT(trial_simulation(),
                ::testing::ExitedWithCode(0),
                ".*") << "Either there was a segmentation fault or the time variable values are incorrect in the result.";
}

TEST_F(biocro_simulationTest, repeat_run) {
    cout << "RUN 1" << endl;
    cout << bs.generate_report() << endl;
    trial_simulation();
    cout << "RUN 2" << endl;
    cout << bs.generate_report() << endl;
    trial_simulation();
    cout << "DONE" << endl;
    cout << bs.generate_report() << endl;
}
