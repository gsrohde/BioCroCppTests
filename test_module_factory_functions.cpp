// Compile with the flag -DVERBOSE=true to get verbose output.
//
// This suite tests the public methods of
// BioCro::Standard_BioCro_library_module_factory
// (a.k.a. module_factory<standardBML::module_library>) other than
// retrieve, which is used extensively elsewhere.
//
#ifndef VERBOSE
#define VERBOSE false
#endif

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "BioCro.h"

// These are the numbers for the Standard BioCro Library.  Note that
// as modules are added and deleted, these numbers will likely have to
// change.
constexpr int expected_number_of_modules = 83;
constexpr int expected_number_of_quantites = 2180;

using std::cout;
using std::endl;

using testing::MatchesRegex;

template<class T>
void print_list(T list) {
    for (auto item : list) {
        cout << item << endl;
    }
}


// This tests get_all_modules by checking the expected number of
// module names are returned.
TEST(ModuleReporterTest, ModuleCount) {
    auto modules =
        BioCro::Standard_BioCro_library_module_factory::get_all_modules();

    if (VERBOSE) print_list(modules);

    EXPECT_EQ(modules.size(), expected_number_of_modules);
}


// This tests get_all_quantities returns the expected number of items.
TEST(ModuleReporterTest, QuantityCount) {
    auto quantities =
        BioCro::Standard_BioCro_library_module_factory::get_all_quantities();

    if (VERBOSE) print_list(quantities["quantity_name"]);

    for (auto& item : quantities) {
        EXPECT_EQ(item.second.size(), expected_number_of_quantites);
    }
}


// This tests that the structure returned by get_all_quantities has
// the expected number of columns with the expected names.  And it
// checks that all values in the quantity_type column are either
// "input" or "output".
TEST(ModuleReporterTest, QuantityMapStructure) {
    auto quantities =
        BioCro::Standard_BioCro_library_module_factory::get_all_quantities();
    ASSERT_EQ(quantities.size(), 3);
    for (auto& item : quantities) {
        ASSERT_THAT(item.first, MatchesRegex("quantity_name|module_name|quantity_type"));
    }
    for (auto& type : quantities["quantity_type"]) {
        ASSERT_THAT(type, MatchesRegex("(in|out)put"));
    }
}
