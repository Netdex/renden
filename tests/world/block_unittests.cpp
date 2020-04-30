#include <gtest/gtest.h>

#include "world/block.hpp"

TEST(BlockTests, DirectionInverseIsSymmetric)
{
	using namespace world;
	for (auto dir : kDirections)
		EXPECT_EQ(dir, dir_inv(dir_inv(dir)));
}

TEST(BlockTests, DirectionOrdinalIsSymmetric)
{
	using namespace world;
	for (auto dir : kDirections)
		EXPECT_EQ(dir, ord_to_dir(dir_to_ord(dir)));
}

TEST(BlockTests, DirectionInverseOffsetIsSymmetric)
{
	using namespace world;
	for (auto dir : kDirections)
	{
		Direction inv = dir_inv(dir);
		EXPECT_EQ(glm::ivec3{}, dir_to_offset(dir) + dir_to_offset(inv));
	}
}
