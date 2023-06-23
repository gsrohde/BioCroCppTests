#include <gtest/gtest.h>

#include <framework/biocro_simulation.h>
#include <framework/module_factory.h>
#include <module_library/module_library.h>
#include "print_result.h"

class biocro_simulationTest : public ::testing::Test {

    state_map initial_state = { {"TTc", 0} };
    state_map invariant_parameters = { {"sowing_time", 0}, {"tbase", 5}, {"temp", 11}, {"timestep", 1} };
    state_vector_map drivers = { {"time",  { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }} };
    mc_vector steady_state_modules;
    mc_vector derivative_modules { module_factory<standardBML::module_library>::retrieve("thermal_time_linear") };

   protected:
    biocro_simulationTest()
        :
        bs {
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
        } {}

    biocro_simulation bs;

    double get_initial_time() {
        return drivers["time"][0];
    }

    state_map get_initial_state() {
        return initial_state;
    }
};

// Check that the initial state at the initial time is the same in the
// result each time the simulation is run.
//
// Comments: In my view, once a biocro_simulation object is
// constructed, calls to run_simulation should be idempotent: the
// result should be the same each time the method is called.

TEST_F(biocro_simulationTest, repeat_run) {
    const auto result = bs.run_simulation();
    const auto result2 = bs.run_simulation();

    const double initial_time {get_initial_time()};

    double initial_TTc_value {get_initial_state()["TTc"]};

    for (int i = 0; i < result.at("time").size(); ++i) {
        if (result.at("time")[i] == initial_time) {
            EXPECT_DOUBLE_EQ(result2.at("TTc")[i], initial_TTc_value);
        }
    }
}
