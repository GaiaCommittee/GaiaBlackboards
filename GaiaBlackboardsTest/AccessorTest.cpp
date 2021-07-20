#include <gtest/gtest.h>
#include "../GaiaBlackboards/GaiaBlackboards.hpp"

using namespace Gaia::Blackboards;

TEST(AccessorTest, Basic)
{
    Accessor<int> sample_accessor;

    Blackboard sample_blackboard;
    sample_blackboard.SetValue("SampleValue", 5);

    EXPECT_EQ(sample_accessor.Get(), std::nullopt);
    EXPECT_EQ(sample_accessor.Acquire(2), 2);
    EXPECT_EQ(sample_accessor.Acquire(3), 3);

    sample_accessor.Set(4);

    EXPECT_EQ(sample_blackboard.GetValue<int>("SampleValue"), 5);

    sample_accessor.Connect(sample_blackboard, "SampleValue");

    EXPECT_EQ(*sample_accessor.Get(), 5);

    EXPECT_EQ(sample_accessor.Acquire(6), 5);

    sample_accessor.Set(7);

    EXPECT_EQ(sample_blackboard.GetValue<int>("SampleValue"), 7);

    sample_accessor.Disconnect();

    EXPECT_EQ(sample_accessor.Get(), std::nullopt);
    EXPECT_EQ(sample_accessor.Acquire(8), 8);

    sample_accessor.Set(11);

    EXPECT_EQ(sample_blackboard.GetValue<int>("SampleValue"), 7);
}