#include <gtest/gtest.h>
#include <gmock/gmock.h> // for matchers

#include <iostream>

#include "BioCro_Extended.h"

#include "print_result.h"

using namespace std;

using Module_factory = BioCro::Standard_BioCro_library_module_factory;

// Solver parameters
const string ode_solver_name {"boost_euler"};
constexpr double output_step_size {1};
constexpr double adaptive_rel_error_tol {1e-4};
constexpr double adaptive_abs_error_tol {1e-4};
constexpr int adaptive_max_steps {200};

// Dynamical System parameters
constexpr size_t number_of_timepoints{5};
// (We need at least one driver variable, but we don't care what it's called:)
const string driver_variable_name{"some_driver"};

const BioCro::State initial_state { {"position", 0}, {"velocity", 1} };
const BioCro::Parameter_set parameters
    { {"mass", 10}, {"spring_constant", 0.1}, {"timestep", 1}};
const BioCro::Module_set steady_state_modules(0);
const BioCro::Module_set derivative_modules
    { Module_factory::retrieve("harmonic_oscillator") };

// Returns an increasing sequence (a vector of doubles) of the given
// length, starting at 0.
vector<double> sequence(size_t length) {
    auto v = vector<double>(length);
    iota(v.begin(), v.end(), 0);
    return v;
}

class DynamicalSystemTest : public ::testing::Test {
 protected:
    BioCro::System_drivers drivers
        { {driver_variable_name,  sequence(number_of_timepoints)} };

    // The solver
    BioCro::Solver system_solver =
        BioCro::make_ode_solver(
            ode_solver_name,
            output_step_size,
            adaptive_rel_error_tol,
            adaptive_abs_error_tol,
            adaptive_max_steps);

    // The system
    BioCro::Dynamical_system ds = BioCro::make_dynamical_system(
        initial_state,
        parameters,
        drivers,
        steady_state_modules,
        derivative_modules);
};

// Tests

// get_ntimes() should return the number of time points of the
// simulation as determined by the length of the drivers.
TEST_F(DynamicalSystemTest, ntimesIsCorrect) {
    EXPECT_EQ(ds->get_ntimes(), drivers[driver_variable_name].size());
}

// The system we've defined shouldn't require an Euler solver.
TEST_F(DynamicalSystemTest, EulerSolverNotRequired) {
    EXPECT_EQ(ds->requires_euler_ode_solver(), false);
}

// To begin with, the values returned by get_differential_quantities
// should match the values in the initial state.
TEST_F(DynamicalSystemTest, GetDifferentialQuantitiesWorks) {
    auto size = initial_state.size();
    auto v = vector<double>(2);
    ds->get_differential_quantities(v);
    BioCro::Variable_set keys{ds->get_differential_quantity_names()};
    for (auto i = 0; i < keys.size(); ++i) {
        EXPECT_DOUBLE_EQ(v[i], initial_state.at(keys[i]));
    }
}

using ::testing::HasSubstr;
using ::testing::Not;
using ::testing::MatchesRegex;
TEST_F(DynamicalSystemTest, IntegrationReportIsCorrect) {
    EXPECT_EQ(system_solver->generate_integrate_report(),
              "The ode_solver has not been called yet");

    auto result = system_solver->integrate(ds);
    //print_result(result);

    auto integration_report = system_solver->generate_integrate_report();
    EXPECT_THAT(integration_report,
                Not(HasSubstr("The ode_solver has not been called yet")));
    EXPECT_THAT(integration_report,
                MatchesRegex("boost::numeric::odeint::integrate_const required " +
                             std::to_string(number_of_timepoints - 1) +
                             " steps to integrate the system\n.*"));
    // (The boost_euler solver does exactly one step for each time
    // interval, hence number_of_timepoints - 1 is the total number of
    // steps.)
}

TEST_F(DynamicalSystemTest, ResettingWorks) {
    auto result = system_solver->integrate(ds);
    /*
    ds->reset();    ds->get_differential_quantities(v);
    for (auto i = 0; i < differential_quantity_names.size(); ++i) {
        cout << v[i] << " : " << initial_state.at(differential_quantity_names[i]) << endl;
    }
    print_state(BioCro::get_initial_state(result, {"position", "velocity"}));print_result(result);
    //    ds->reset();
    result = system_solver->integrate(ds);
    print_state(BioCro::get_initial_state(result, {"position", "velocity"}));
    */
    print_result(result);
}
