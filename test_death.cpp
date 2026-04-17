#include <gtest/gtest.h>

TEST(Death, Msg) {
    EXPECT_EXIT(std::system("./test_err"), ::testing::ExitedWithCode(1), "MY ERROR MESSAGE");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
