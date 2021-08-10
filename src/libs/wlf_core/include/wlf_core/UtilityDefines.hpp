#pragma once

#include <chrono>

namespace {
using hires_clock     = std::chrono::high_resolution_clock;
using hires_duration  = hires_clock::duration;
using hires_timepoint = hires_clock::time_point;
} // namespace
