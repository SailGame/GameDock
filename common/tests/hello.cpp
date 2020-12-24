  
#include <gtest/gtest.h>

namespace Common { namespace Test {

using namespace testing;

class Fixture : public ::testing::Test {
public:
    Fixture() {}

    void SetUp() {}

    void TearDown() {}
};

TEST_F(Fixture, Hello) {
    EXPECT_TRUE(true);
}

}}