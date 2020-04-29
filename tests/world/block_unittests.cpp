#include <gtest/gtest.h>

#include "world/block.hpp"

TEST(BlockTests, DirectionReflexiveInverse)
{
	using namespace world;
	for (auto dir : kDirections)
	{
		EXPECT_EQ(dir, ord_to_dir_inv(dir_to_ord(ord_to_dir_inv(dir_to_ord(dir)))));
	}
}
