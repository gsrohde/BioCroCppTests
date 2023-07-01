#ifndef VERBOSE
#define VERBOSE false
#endif

#include <gtest/gtest.h>

#include <cmath>

#include <framework/biocro_simulation.h>
#include <framework/module_factory.h>
#include <module_library/module_library.h>

#include "print_result.h"

using namespace std;
using namespace math_constants;

/*
 * The oscillation should obey the formula
 *
 *     x(t) = A sin(ωt + φ),
 *
 * where x(t) if the position at time t.  We can use
 *
 *     x(0) = A sin(φ) = initial_state["position"]
 *
 * and
 *
 *     v(0) = Aω cos(φ) = initial_state["velocity"]
 *
 * together with the force equations
 *
 *     F(t) = m a(t) = m (-ω² x(t))
 *
 * and
 *
 *     F(t) = -k x(t)
 *
 * (where k is the spring constant) to solve for the parameters A,
 * ω, and φ.
 */
class HarmonicOscillator_Test : public ::testing::Test {
   protected:
    HarmonicOscillator_Test() {
        double mass = invariant_parameters["mass"];
        double spring_constant = invariant_parameters["spring_constant"];
        invariant_parameters["period"] = 2*pi*sqrt(mass/spring_constant);
    }

    void set_duration(int n) {
        vector<double> times;
        for (auto i = 0; i < n; ++i) {
            times.push_back(i);
        }
        drivers = { { "time", times } };
    }

    int duration() const {
        return drivers.at("time").size();
    }

    double omega() const {
        double m = invariant_parameters.at("mass");
        double k = invariant_parameters.at("spring_constant");
        return sqrt(k/m);
    }
        
    double period() const {
        return 2 * pi / omega();
    }

    // φ (phi)
    double phase() const {
        return atan2(omega() * initial_state.at("position"), initial_state.at("velocity"));
    }

    // A
    double amplitude() const {
        double x_0 {initial_state.at("position")};

        double v_0 {initial_state.at("velocity")};

        if (abs(sin(phase())) > abs(omega() * cos(phase()))) {
            return x_0 / sin(phase());
        } else {
            return v_0 / (omega() * cos(phase()));
        }
    }
        
    state_vector_map get_simulation_result() const {
        return get_simulation().run_simulation();
    }
    
   private:

    // By making this private, we make sure that recreate the
    // simulation every time we run it (via get_simulation_result()).
    // This way, the simulation can't get into a bad state (e.g. by
    // resetting the drivers variable via set_duration()) between the
    // time we create it and the time we run it.
    
    biocro_simulation get_simulation() const {
        return biocro_simulation {
            initial_state,
                invariant_parameters,
                drivers,
                steady_state_modules,
                derivative_modules,
                //"boost_rosenbrock", // This works poorly if duration() = 2.
                "boost_rk4",
                //"boost_rkck54",
                //"auto",             // Chooses Rosenbrock in this case.
                //"boost_euler",
                //"homemade_euler",
                1,
                0.0001,
                0.0001,
                200
                };
    }

    unordered_map<string, double> initial_state = { {"position", 0}, {"velocity", 1}};
    unordered_map<string, double> invariant_parameters = { {"mass", 10}, {"spring_constant", 0.1}, {"timestep", 1}};
    unordered_map<string, vector<double>> drivers = { {"time",  { 0, 1 }} };
    mc_vector steady_state_modules { module_factory<standardBML::module_library>::retrieve("harmonic_energy") };
    mc_vector derivative_modules { module_factory<standardBML::module_library>::retrieve("harmonic_oscillator") };
};

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

TEST_F(HarmonicOscillator_Test, PeriodIsCorrect) {

    if (VERBOSE) cout << "phase: " << phase() << endl;
    if (VERBOSE) cout << "period: " << period() << endl;
    if (VERBOSE) cout << "amplitude: " << amplitude() << endl;

    set_duration(round(period() * 20) + 1);
    if (VERBOSE) cout << "duration: " << duration() << endl;
    auto result {get_simulation_result()};
    if (VERBOSE) print_result(result);

    // position should return to zero every half period;
    // and it should change sign as well
    for (double t = 0; t < duration(); t += period()/2) {
        int i = round(t);
        EXPECT_NEAR(result["position"][i], 0.0, 1.0) << "At time " << i << " position is " << result["position"][i];
        double prior_position {result["position"][floor(t)]};
        double subsequent_position {result["position"][floor(t) + 1]};
        EXPECT_TRUE(sgn(prior_position) != sgn(subsequent_position));
        if (VERBOSE) cout    << "At t = " << t << ", the position changes from " << prior_position
                             << " to " << subsequent_position << "." << endl;
    }

    // amplitude should be amplitude()
    // only test if duration() >= period()
    if (duration() >= period()) {
        double maximum {0};
        double minimum {0};
        for (int i = 0; i < duration(); ++i) {
            maximum = max(maximum, result["position"][i]);
            minimum = min(minimum, result["position"][i]);
        }
        EXPECT_NEAR(maximum, amplitude(), 0.0042);
        EXPECT_NEAR(minimum, -amplitude(), 0.001);
    }

    // total energy should be constant
    auto he = result["total_energy"];
    double init_he {he[0]};
    
    for (int i = 0; i < duration(); ++i) {
        EXPECT_NEAR(he[i], init_he, 0.00075);
    }
    
}

    
