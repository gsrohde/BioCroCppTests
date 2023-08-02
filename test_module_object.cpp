// Here we show that BioCro modules  may be used directly, but that we
// have  to  be careful  when  doing  so.   In particular,  the  input
// quantities object passed to the  module constructor must persist at
// least  until the  module is  run in  order to  obtain the  expected
// output.  This  means that we can't  pass the input quantities  as a
// state_map literal, even though it may seem natural to do so.

#include <gtest/gtest.h>

#include <iostream>
using namespace std;
#include <module_library/thermal_time_linear.h>
#include <framework/module_helper_functions.h>

class ModuleObjectTest : public ::testing::Test {
   protected:
    const double input_time {200};
    const double input_sowing_time {100};
    const double input_temp {25};
    const double input_tbase {1};

    state_map input_quantities {{"time", input_time},
                                {"sowing_time", input_sowing_time},
                                {"temp", input_temp},
                                {"tbase", input_tbase}};

    state_map output {{"TTc", 0}};

    double expected_output_value = input_time < input_sowing_time ? 0.0
                                 : input_temp <= input_tbase      ? 0.0
                                 :                                  (input_temp - input_tbase)/24.0;
};

TEST_F(ModuleObjectTest, CorrectDirectUsage)
{
    standardBML::thermal_time_linear ttl{input_quantities, &output};
    ttl.run();

    EXPECT_DOUBLE_EQ(output.at("TTc"), expected_output_value);
}

TEST_F(ModuleObjectTest, AlteredReferencedObject)
{
    standardBML::thermal_time_linear ttl{input_quantities, &output};
    input_quantities = {};
    ttl.run();

    // We get something near zero for output rather than what one
    // would expect.
    EXPECT_NEAR(output.at("TTc"), 0, 1e-309);

}

TEST_F(ModuleObjectTest, AlterationAfterRunOK)
{
    standardBML::thermal_time_linear ttl{input_quantities, &output};
    ttl.run();
    input_quantities = {};

    EXPECT_DOUBLE_EQ(output.at("TTc"), expected_output_value);
}

TEST_F(ModuleObjectTest, RerunningDoublesTheOutput)
{
    standardBML::thermal_time_linear ttl{input_quantities, &output};
    ttl.run();
    ttl.run();

    // This, more precisely, doubles the increase; but since we
    // started from zero, this is the same as doubling the output
    // value.
    EXPECT_DOUBLE_EQ(output.at("TTc"), 2 * expected_output_value);
}

TEST_F(ModuleObjectTest, RerunningAfterAlterationBarelyAltersTheOutput)
{
    standardBML::thermal_time_linear ttl{input_quantities, &output};
    ttl.run();
    input_quantities = {};
    ttl.run();

    // When we reset the input_quantities, we get something close to
    // what we get when we only run the module once.
    EXPECT_DOUBLE_EQ(output.at("TTc"), expected_output_value);
}

TEST_F(ModuleObjectTest, RvalueInputNotOK)
{
    standardBML::thermal_time_linear ttl{
                                         {{"time", input_time},
                                          {"sowing_time", input_sowing_time},
                                          {"temp", input_temp},
                                          {"tbase", input_tbase}},
                                         &output};
    ttl.run();

    // We get something near zero for output rather than what one
    // would expect.
    EXPECT_NEAR(output.at("TTc"), 0, 1e-309);

}

TEST_F(ModuleObjectTest, LiteralInputNotOK)
{
    standardBML::thermal_time_linear ttl{
                                         {{"time", 200},
                                          {"sowing_time", 100},
                                          {"temp", 25},
                                          {"tbase", 1}},
                                         &output};
    ttl.run();

    // We get something near zero for output rather than what one
    // would expect.
    EXPECT_NEAR(output.at("TTc"), 0, 1e-309);

}

