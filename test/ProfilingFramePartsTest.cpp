#include "wlf_core/Profiling.hpp"

#include <gtest/gtest.h>

using namespace wlf;
using namespace wlf::utils;

TEST(ProfilingFramePartsTest, Test){
   auto frameProfiler = FrameProfiler{
      MultiStopwatch::FromBuilder(
         MultiStopwatchBuilder{3}
            .WithStopwatchName(0, "AAA")
            .WithStopwatchName(1, "BBB")
            .WithStopwatchName(2, "CCC"))
            .value()};

   frameProfiler.StartNewFrame();
}