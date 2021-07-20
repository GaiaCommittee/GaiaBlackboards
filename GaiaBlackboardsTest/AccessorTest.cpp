#include <gtest/gtest.h>
#include "../GaiaBlackboards/GaiaBlackboards.hpp"

using namespace Gaia::Blackboards;

TEST(AccessorTest, Basic)
{
    ValueAccessor<int> sample_accessor;

    Blackboard sample_blackboard;
    sample_blackboard.SetValue<int>("SampleValue", 5);

    sample_accessor.Connect(sample_blackboard, "SampleValue");
    EXPECT_EQ(sample_accessor.Get().value_or(7), 5);

    sample_accessor.Set(4);

    EXPECT_EQ(sample_blackboard.GetValue<int>("SampleValue").value(), 4);

    EXPECT_EQ(*sample_accessor.Get(), 4);

    sample_accessor.Set(7);

    EXPECT_EQ(sample_blackboard.GetValue<int>("SampleValue"), 7);

    sample_accessor.Disconnect();

    EXPECT_EQ(sample_accessor.Get(), std::nullopt);

    sample_accessor.Set(11);

    EXPECT_EQ(sample_blackboard.GetValue<int>("SampleValue"), 7);
}