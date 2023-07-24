// Compile with the flag -DVERBOSE=true to get verbose output.
#ifndef VERBOSE
#define VERBOSE false
#endif

#include <gtest/gtest.h>

#include <cmath>

#include "BioCro.h"

#include "print_result.h"

using namespace std;
using math_constants::pi;
using Module_provider = BioCro::Standard_BioCro_library_module_factory;

/*
 * This tests simulation of an undamped harmonic oscillator consisting
 * of an object with mass m suspended on a spring with spring constant
 * k.  The position is the vertical displacement from the equilibrium
 * position of the object so that if the object's position and
 * velocity are both zero, the object remains at rest.
 *
 * The oscillation should obey the formula
 *
 *     x(t) = A sin(ωt + φ),
 *
 * where x(t) is the position at time t, A is the amplitude of the
 * oscillation, ω is the oscillation frequency, and φ is the phase.
 * We can use
 *
 *     x(0) = A sin(φ) = initial_state["position"]
 *
 * and
 *
 *     v(0) = Aω cos(φ) = initial_state["velocity"]
 *
 * together with the force equations
 *
 *     F(t) = m a(t) = m (-Aω² sin(ωt + φ)) = m (-ω² x(t))
 *
 * and
 *
 *     F(t) = -k x(t)
 *
 * to solve for the parameters A, ω, and φ in terms of m, k, and the
 * initial state (x(0), v(0)).
 */
class HarmonicOscillator_Test : public ::testing::Test {
   protected:
    HarmonicOscillator_Test() {
        double mass = parameters["mass"];
        double spring_constant = parameters["spring_constant"];
    }

    void set_duration(int n) {
        vector<double> times;
        for (auto i = 0; i <= n; ++i) {
            times.push_back(i);
        }
        drivers = { { "time", times } };
    }

    // This is the total number of steps.
    int duration() const {
        return drivers.at("time").size() - 1;
    }

    double omega() const {
        double m = parameters.at("mass");
        double k = parameters.at("spring_constant");
        return sqrt(k/m);
    }
        
    double period() const {
        return 2 * pi / omega();
    }

    // φ (phi)
    double phase() const {
        return atan2(omega() * initial_state.at("position"),
                     initial_state.at("velocity"));
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
        
    BioCro::Simulation_result get_simulation_result() const {
        return get_simulator().run_simulation();
    }
    
   private:

    // By making this private, we make sure that we recreate the
    // simulation every time we run it (via get_simulation_result()).
    // This way, the simulation can't get into a bad state (e.g. by
    // resetting the drivers variable via set_duration()) between the
    // time we create it and the time we run it.
    
    BioCro::Simulator get_simulator() const {
        return BioCro::Simulator {
            initial_state,
                parameters,
                drivers,
                steady_state_modules,
                derivative_modules,
                //"boost_rosenbrock", // This gives odd results if
                                      // duration() = 1, appearing to
                                      // show no change in state
                                      // from time 0 to time 1.
                "boost_rk4",          // This and boost_rkck54 seem to work the best here.
                //"boost_rkck54",
                //"auto",             // Chooses Rosenbrock in this case.

                //"boost_euler",      // The Euler solvers perform
                                      // extremely poorly, showing the
                                      // total energy climbing from 5
                //"homemade_euler",   // to about 1,352,000.
                1,
                0.0001,
                0.0001,
                200
                };
    }

    BioCro::State initial_state { {"position", 0}, {"velocity", 1}};
    BioCro::Parameter_set parameters { {"mass", 10},
                                       {"spring_constant", 0.1},
                                       {"timestep", 1}};
    BioCro::System_drivers drivers { {"time",  { 0, 1 }} };
    BioCro::Module_set steady_state_modules
        { Module_provider::retrieve("harmonic_energy") };
    BioCro::Module_set derivative_modules
        { Module_provider::retrieve("harmonic_oscillator") };
};

template <typename T> int sgn(T val) {
    return (T{0} < val) - (val < T{0});
}

// Check that the object position returns to zero every half period
// and passes from a positive to a negative value (or vice versa)
// during the step that crosses the half-period time point.
TEST_F(HarmonicOscillator_Test, PeriodIsCorrect) {

    if (VERBOSE) cout << "phase: " << phase() << endl;
    if (VERBOSE) cout << "period: " << period() << endl;
    if (VERBOSE) cout << "amplitude: " << amplitude() << endl;

    set_duration(floor(period() * 40) + 1); // We want to inspect the
                                            // values both before (or
                                            // at) and after the time
                                            // point marking the 40th
                                            // period.
    
    if (VERBOSE) cout << "duration: " << duration() << endl;
    auto result {get_simulation_result()};
    if (VERBOSE) print_result(result);

    // position should return to zero every half period.
    // It should change sign as well.
    for (double t = 0;
         t < duration(); // duration() is the maximum allowable index,
                         // so ensure floor(t) + 1 (used as an index
                         // below) is less than or equal to
                         // duration().
         t += period()/2) {
        
        int i = round(t);
        EXPECT_NEAR(result["position"][i], 0.0, 1.0)
            << "At time " << i << " position is " << result["position"][i];
        double prior_position {result["position"][floor(t)]};
        double subsequent_position {result["position"][floor(t) + 1]};
        EXPECT_TRUE(sgn(prior_position) != sgn(subsequent_position));
        if (VERBOSE) cout << "Near t = " << t << ", the position changes from "
                          << prior_position << " to " << subsequent_position
                          << "." << endl;
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
    
    for (int i = 0; i <= duration(); ++i) {
        EXPECT_NEAR(he[i], init_he, 0.00075);
    }
    
}

    
