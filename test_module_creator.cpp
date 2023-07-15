#include <gtest/gtest.h>
//#include <gmock/gmock.h>

#include "BioCro.h"

constexpr char known_module_name[] {"harmonic_oscillator"};
constexpr char bogus_module_name[] {"bogus"};

//using testing::MatchesRegex;

TEST(ModuleCreatorTest, KnownModule) {
    BioCro::Module_creator creator;
    ASSERT_NO_THROW({
            creator = BioCro::Standard_BioCro_library_module_factory::retrieve(known_module_name);
        });
    ASSERT_EQ(creator->get_name(), known_module_name);
}


TEST(ModuleCreatorTest, BogusModule) {
    ASSERT_THROW({
            auto creator = BioCro::Standard_BioCro_library_module_factory::retrieve(bogus_module_name);
        }, std::out_of_range);
}
