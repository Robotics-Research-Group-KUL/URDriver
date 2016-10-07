// Bring in my package's API, which is what I'm testing
#include "URDriver/utils.hpp"
// Bring in gtest
#include <gtest/gtest.h>

// Declare a test
TEST(TestSuite, swapTestInt)
{
	unsigned int a=1<<7;

	swap(a);
	ASSERT_EQ(a,1<<31);
}



// Run all the tests that were declared with TEST()
int main(int argc, char **argv){
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
