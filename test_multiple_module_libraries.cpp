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
#include <gmock/gmock.h>

#include "BioCro_Extended.h"
#include "print_result.h"

using ::testing::MatchesRegex;

using Module_factory = BioCro::Standard_BioCro_library_module_factory;
using Module_factory_2 = BioCro::Test_BioCro_library_module_factory;

class MultipleModuleLibrariesTest : public ::testing::Test {
   protected:
    MultipleModuleLibrariesTest() :bs{get_simulator()} {
    }

    void trial_simulation() {
        bs = get_simulator();
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

    // These are defaults.  Individual tests can alter them.
    BioCro::State initial_state { {"TTc", 0} };
    BioCro::Parameter_set parameters
        { {"timestep", 1} };
    BioCro::System_drivers drivers
        { {"time", { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 } },
          {"temp", { 5, 8, 10, 15, 20, 20, 25, 30, 32, 40} } };
    BioCro::Module_set direct_modules {};
    BioCro::Module_set differential_modules {};

private:
    BioCro::Simulator get_simulator();

    // This must be declared last because its initialization depends
    // on the parameters for its construction being initialized first:
    BioCro::Simulator bs;
};

BioCro::Simulator MultipleModuleLibrariesTest::get_simulator() {
    return BioCro::Simulator {
        initial_state,
        parameters,
        drivers,
        direct_modules,
        differential_modules,
        "homemade_euler",
        1,
        0.0001,
        0.0001,
        200
    };
}

TEST_F(MultipleModuleLibrariesTest, CorrectSimulation) {
    parameters.insert({{"sowing_time", 0}, {"tbase", 10} });
    std::initializer_list<BioCro::Module_creator> additional_modules {
        Module_factory::retrieve("thermal_time_linear"),
        Module_factory_2::retrieve("thermal_time_linear")
    };
    differential_modules.insert(differential_modules.end(), additional_modules);
    ASSERT_EXIT(trial_simulation(),
                ::testing::ExitedWithCode(0),
                ".*");
}

// Show that direct modules having outputs in common conflict, even if
// they are from different module libraries.
TEST_F(MultipleModuleLibrariesTest, ConflictingModules) {
    parameters.insert({{"lat", 44}, {"longitude", -121},
                       {"time_zone_offset", -8}, {"year", 2023} });
    
    direct_modules = {Module_factory::retrieve("solar_position_michalsky"),
                      Module_factory_2::retrieve("solar_position_michalsky")};

    EXPECT_THROW(trial_simulation(), std::logic_error);

    // The solar_position_michalsky module is defined identically in
    // standardBML and testBML (except for the namespace, of course).
    // In particular, they the same set of outputs, resulting in a
    // "quantities defined more than once" error.
    std::string message_match_string = "Thrown by dynamical_system::"
        "dynamical_system: the supplied inputs cannot form a valid dynamical "
        "system.*"
        "The following quantities were defined more than once in the inputs:.*";

    // Test that the expected exception is thrown.
    EXPECT_THROW({
        try
        {
            trial_simulation();
        }
        catch( const std::logic_error& e )
        {
            // Test that the exeception has the correct message.
            EXPECT_THAT(e.what(), MatchesRegex(message_match_string));
            throw;
        }
    }, std::logic_error);

}

