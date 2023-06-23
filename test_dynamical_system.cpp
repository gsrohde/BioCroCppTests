#include <gtest/gtest.h>

#include <iostream>
#include <framework/dynamical_system.h> // for dynamical_system
// #include <framework/state_map.h> // for state_map, state_vector_map
#include <framework/module_factory.h> // for module_factory
// #include <framework/module_creator.h> // for mc_vector
#include <module_library/module_library.h> // for standardBML::module_library
#include <framework/ode_solver_library/ode_solver_factory.h> // for ode_solver_factory
// #include <framework/ode_solver.h> // for ode_solver

using namespace std;

const string ode_solver_name {"boost_euler"};
constexpr auto output_step_size = 1;
constexpr double adaptive_rel_error_tol {1e-4};
constexpr double adaptive_abs_error_tol {1e-4};
constexpr int adaptive_max_steps {200};

const auto system_solver =
    std::unique_ptr<ode_solver>(
        ode_solver_factory::create(
            ode_solver_name,
            output_step_size,
            adaptive_rel_error_tol,
            adaptive_abs_error_tol,
            adaptive_max_steps));

state_map initial_state = { {"position", 0}, {"velocity", 1}};
const state_map invariant_parameters = { {"mass", 10}, {"spring_constant", 0.1}, {"timestep", 1}};
state_vector_map drivers = { {"time",  { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }} };
const mc_vector steady_state_modules(0);
const mc_vector derivative_modules { module_factory<standardBML::module_library>::retrieve("harmonic_oscillator") };

const dynamical_system ds(initial_state,
                          invariant_parameters,
                          drivers,
                          steady_state_modules,
                          derivative_modules);

const auto shared_ds = std::shared_ptr<dynamical_system>(
                     new dynamical_system(
                         initial_state,
                         invariant_parameters,
                         drivers,
                         steady_state_modules,
                         derivative_modules));

TEST(dynamical_systemTest, DISABLED_CopyConstructor) {
    //dynamical_system ds2{ds};
}

TEST(dynamical_systemTest, DISABLED_CopyAssign) {
    dynamical_system ds2(initial_state,
                         invariant_parameters,
                         drivers,
                         steady_state_modules,
                         derivative_modules);
    //ds2 = ds;
}

TEST(dynamical_systemTest, get_ntimes) {
    EXPECT_EQ(ds.get_ntimes(), drivers["time"].size());
}

TEST(dynamical_systemTest, requires_euler_ode_solver) {
    EXPECT_EQ(ds.requires_euler_ode_solver(), false);
}

TEST(dynamical_systemTest, differential_quantities) {
    auto size = initial_state.size();
    auto v = vector<double>(2);
    ds.get_differential_quantities(v);
    string_vector keys{ds.get_differential_quantity_names()};
    for (auto i = 0; i < keys.size(); ++i) {
        EXPECT_DOUBLE_EQ(v[i], initial_state[keys[i]]);
    }
}

TEST(dynamical_systemTest, DISABLED_with_solver) {
    cout << system_solver->generate_integrate_report() << endl;
    auto result = system_solver->integrate(shared_ds);
    cout << "position          velocity" << endl;
    for (auto i = 0; i < result["time"].size(); ++i) {
        cout << result["position"][i] << "     " << result["velocity"][i] << endl;
    }
    cout << system_solver->generate_integrate_report() << endl;

    shared_ds->reset();

    result = system_solver->integrate(shared_ds);
    cout << "position          velocity" << endl;
    for (auto i = 0; i < result["time"].size(); ++i) {
        cout << result["position"][i] << "     " << result["velocity"][i] << endl;
    }
    cout << system_solver->generate_integrate_report() << endl;
}
