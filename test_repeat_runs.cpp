#include <gtest/gtest.h>

#include <framework/biocro_simulation.h>
#include <framework/module_factory.h>
#include <module_library/module_library.h>
#include "print_result.h"

class BiocroSimulationTest : public ::testing::Test {

    state_map initial_state = { {"TTc", 0} };
    state_map invariant_parameters = { {"sowing_time", 0}, {"tbase", 5}, {"temp", 11}, {"timestep", 1} };
    state_vector_map drivers = { {"time",  { 0, 1, 2, 3, 4, 5 }} };
    mc_vector steady_state_modules;
    mc_vector derivative_modules { module_factory<standardBML::module_library>::retrieve("thermal_time_linear") };

   protected:
    BiocroSimulationTest()
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
};

// "run_simulation()" should be idempotent.  Alternatively, an
// exception should be thrown if the user attempts to run it more than
// once.
//
// This test checks that each time the simulation is run, the result
// is the same.  Currently this fails unless there are no differential
// modules.  This is because the state of the differential quantities
// is not reset at the completion of a run so that on subsequent runs,
// the initial state of these quantities will not correspond to the
// initial state given to the biocro_simulation constructor.

TEST_F(BiocroSimulationTest, DISABLED_runSimulationIsIdempotent) {
    const auto first_result = bs.run_simulation();
    const auto second_result = bs.run_simulation();

    //print_result(first_result);
    //print_result(second_result);

    for (auto item : first_result) {
        string quantity_name {item.first};
        size_t duration {item.second.size()};
        for (size_t i {0}; i < duration; ++i) {
            ASSERT_DOUBLE_EQ(first_result.at(quantity_name)[i], second_result.at(quantity_name)[i]);
        }
    }
}
