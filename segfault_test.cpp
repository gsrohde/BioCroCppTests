// From https://stackoverflow.com/questions/47583352/how-to-catch-segmentation-fault-with-google-test

#include <gtest/gtest.h>

int deref(int* p)
{
    return *p;
}


TEST(test_deref_1, will_segfault)
{
    ASSERT_EXIT((deref(nullptr), exit(0)), ::testing::KilledBySignal(SIGSEGV), ".*");
}


TEST(test_dref_2, will_not_segfault)
{
    int i = 42;
    ASSERT_EXIT((deref(&i), exit(0)), ::testing::ExitedWithCode(0), ".*");
}

TEST(test_deref_3, will_segfault)
{
    ASSERT_DEATH(deref(nullptr), "");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
