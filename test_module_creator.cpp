#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "BioCro.h"

constexpr char known_module_name[] {"harmonic_oscillator"};
constexpr char known_module_inputs[] {"position|velocity|mass|spring_constant"}; // as a regexp
constexpr char known_module_outputs[] {"position|velocity"}; // as a regexp
constexpr char bogus_module_name[] {"bogus"};

using testing::MatchesRegex;

using Module_factory = BioCro::Standard_BioCro_library_module_factory;

TEST(ModuleCreatorTest, KnownModule) {
    BioCro::Module_creator creator;
    ASSERT_NO_THROW({
            creator = Module_factory::retrieve(known_module_name);
        });
    ASSERT_EQ(creator->get_name(), known_module_name);

    auto inputs = creator->get_inputs();
    auto outputs = creator->get_outputs();

    EXPECT_EQ(inputs.size(), 4);
    for (auto& item : inputs) {
        EXPECT_THAT(item, MatchesRegex(known_module_inputs));
    }

    EXPECT_EQ(outputs.size(), 2);
    for (auto& item : outputs) {
        EXPECT_THAT(item, MatchesRegex(known_module_outputs));
    }
}


TEST(ModuleCreatorTest, BogusModule) {
    ASSERT_THROW({
            auto creator = Module_factory::retrieve(bogus_module_name);
        }, std::out_of_range);
}
