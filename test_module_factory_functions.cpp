#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "BioCro.h"

using testing::MatchesRegex;

TEST(ModuleReporterTest, ModuleCount) {
    auto modules =
        BioCro::Standard_BioCro_library_module_factory::get_all_modules();
    EXPECT_EQ(modules.size(), 83);
}


TEST(ModuleReporterTest, QuantityCount) {
    auto quantities =
        BioCro::Standard_BioCro_library_module_factory::get_all_quantities();
    for (auto& item : quantities) {
        EXPECT_EQ(item.second.size(), 2004);
    }
}


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
