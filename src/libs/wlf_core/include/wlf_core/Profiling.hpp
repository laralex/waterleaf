#pragma once
#include "Defines.hpp"
#include "Stopwatch.hpp"
#include "MultiStopwatch.hpp"
#include "UtilityInterfaces.hpp"
#include "UtilityWrappers.hpp"

#include <chrono>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace {

using hires_clock     = std::chrono::high_resolution_clock;
using hires_duration  = hires_clock::duration;
using hires_timepoint = hires_clock::time_point;

template<typename DurationT, typename F, typename... Args>
auto ProfileRun(F&& function, Args&&... args) {
   const auto begin = hires_clock::now();
   // if there's something to return - return a pair of result and time
   if constexpr(!std::is_same_v<std::invoke_result_t<F&&, Args&&...>, void>) {
      auto function_output = function(std::forward<Args>(args)...);
      const auto end       = hires_clock::now();
      return std::make_pair(
         function_output,
         std::chrono::duration_cast<DurationT>(end - begin).count());
   } else { // or return just time
      function(std::forward<Args>(args)...);
      const auto end = hires_clock::now();
      return std::chrono::duration_cast<DurationT>(end - begin).count();
   }
}

} // namespace

namespace wlf::utils {

template<typename F, typename... Args>
ENGINE_API auto ProfileInMicrosecs(F&& function, Args&&... args) {
   return ProfileRun<std::chrono::duration<wlf::u64, std::micro>>(
      std::forward<F>(function), std::forward<Args>(args)...);
}

template<typename F, typename... Args>
ENGINE_API auto ProfileInMillisecs(F&& function, Args&&... args) {
   return ProfileRun<std::chrono::duration<wlf::u64, std::milli>>(
      std::forward<F>(function), std::forward<Args>(args)...);
}

class ENGINE_API FrameProfiler : public INonCopyable {
public:
   explicit FrameProfiler(usize nFramesBuffered,
                          MultiStopwatch&& stopwatches) noexcept
         : INonCopyable()
         , m_NumFramesBuffered(nFramesBuffered)
         , m_ProfilePartsMultiStopwatch(nFramesBuffered, std::move(stopwatches))
         , m_FrameTimeStopwatch(nFramesBuffered, Stopwatch{}) {}

   wlf::usize StopwatchesNumber() const noexcept;
   wlf::usize BufferedFramesNumber() const noexcept;
   bool IsKeyValid(usize key) const noexcept;
   bool IsFrameDataAccessible(usize numFramesBack) const noexcept;

   void StartNewFrame() noexcept;
   bool BeginMeasureOf(usize key) noexcept;
   bool EndMeasureOf(usize key) noexcept;

   std::optional<wlf::u64> CurrentCumulativeTimingOf(usize key) const noexcept;
   std::optional<wlf::u64>
   HistoricalTimingOf(usize key, usize numFramesBack) const noexcept;
   std::optional<wlf::u64>
   HistoricalFrametime(usize numFramesBack = 1) const noexcept;

private:
   usize m_NumFramesBuffered;
   RecordingMultiStopwatch m_ProfilePartsMultiStopwatch;
   RecordingStopwatch m_FrameTimeStopwatch;
};

} // namespace wlf::utils
