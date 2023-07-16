#include <gtest/gtest.h>

#include "BioCro.h"
#include "print_result.h"

using Module_factory = BioCro::Standard_BioCro_library_module_factory;

class BiocroSimulationTest : public ::testing::Test {

    BioCro::State initial_state { {"TTc", 0} };
    BioCro::Parameter_set parameters { {"sowing_time", 0},
                                       {"tbase", 5},
                                       {"temp", 11},
                                       {"timestep", 1} };
    BioCro::System_drivers drivers { {"time",  { 0, 1, 2, 3, 4, 5 }} };
    BioCro::Module_set steady_state_modules;
    BioCro::Module_set derivative_modules
        { Module_factory::retrieve("thermal_time_linear") };

   protected:
    BiocroSimulationTest()
        :
        bs {
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
        } {}

    BioCro::Simulator bs;
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
// initial state given to the BioCro::Simulator constructor.

TEST_F(BiocroSimulationTest, DISABLED_runSimulationIsIdempotent) {
    const auto first_result = bs.run_simulation();
    const auto second_result = bs.run_simulation();

    //print_result(first_result);
    //print_result(second_result);

    for (auto item : first_result) {
        string quantity_name {item.first};
        size_t duration {item.second.size()};
        for (size_t i {0}; i < duration; ++i) {
            ASSERT_DOUBLE_EQ(first_result.at(quantity_name)[i],
                             second_result.at(quantity_name)[i]);
        }
    }
}
