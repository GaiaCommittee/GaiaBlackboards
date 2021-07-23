#include <gtest/gtest.h>
#include "../GaiaBlackboards/GaiaBlackboards.hpp"

using namespace Gaia::Blackboards;

TEST(BlackboardTest, Basic)
{
//    Blackboard blackboard;
//
//    auto sample_value = blackboard.GetValue<int>("NonExistSampleValue");
//    EXPECT_EQ(sample_value, std::nullopt);
//
//    blackboard.SetValue("SampleValue", 1);
//    EXPECT_EQ(*blackboard.GetValue<int>("SampleValue"), 1);
//
//    EXPECT_EQ(blackboard.GetValue<long>("SampleValue2"), std::nullopt);
//    EXPECT_EQ(blackboard.GetValue<bool>("SampleValue2"), std::nullopt);
//    EXPECT_EQ(blackboard.GetValue<int>("SampleValue2", 3), 3);
//
//    blackboard.Remove("SampleValue");
//    EXPECT_EQ(blackboard.GetValue<int>("SampleValue"), std::nullopt);
//
//    blackboard.Clear();
//    EXPECT_EQ(blackboard.GetValue<int>("SampleValue2"), std::nullopt);
}