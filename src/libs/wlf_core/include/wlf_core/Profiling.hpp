#pragma once
#include "Defines.hpp"
#include "Invoke.hpp"
#include "MultiStopwatch.hpp"
#include "Stopwatch.hpp"
#include "UtilityDefines.hpp"
#include "UtilityInterfaces.hpp"

#include <chrono>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace wlf::util {

template<typename DurationT, typename F, typename... Args>
auto ProfileInvokeDiscardResult(F&& function, Args&&... args) noexcept(noexcept(
   std::invoke(std::forward<F>(function), std::forward<Args>(args)...))) {
   const auto begin = detail::hires_clock::now();
   std::invoke(std::forward<F>(function), std::forward<Args>(args)...);
   const auto end = detail::hires_clock::now();
   return std::chrono::duration_cast<DurationT>(end - begin).count();
}

// if 'function' returns void, return just timing
template<typename DurationT,
         typename F,
         typename... Args,
         typename =
            std::enable_if_t<std::is_void_v<std::invoke_result_t<F, Args...>>>>
auto ProfileInvoke(F&& function, Args&&... args) noexcept(noexcept(
   ProfileInvokeDiscardResult<DurationT>(std::forward<F>(function),
                                         std::forward<Args>(args)...))) {
   return ProfileInvokeDiscardResult<DurationT>(std::forward<F>(function),
                                                std::forward<Args>(args)...);
}

// if there's something to return - return a pair of result and timing
template<typename DurationT,
         typename F,
         typename... Args,
         typename =
            std::enable_if_t<!std::is_void_v<std::invoke_result_t<F, Args...>>>,
         int = 0>
auto ProfileInvoke(F&& function, Args&&... args) noexcept(noexcept(
   std::invoke(std::forward<F>(function), std::forward<Args>(args)...))) {
   const auto begin = detail::hires_clock::now();
   auto&& functionOutput =
      std::invoke(std::forward<F>(function), std::forward<Args>(args)...);
   const auto end = detail::hires_clock::now();
   return std::make_pair(
      std::move(functionOutput),
      std::chrono::duration_cast<DurationT>(end - begin).count());
}

class ENGINE_API FrameProfiler : public INonCopyable {
public:
   explicit FrameProfiler(const usize nFramesBuffered,
                          MultiStopwatch&& stopwatches) noexcept
         : INonCopyable()
         , m_NumFramesBuffered(nFramesBuffered)
         , m_ProfilePartsMultiStopwatch(nFramesBuffered, std::move(stopwatches))
         , m_FrameTimeStopwatch(nFramesBuffered, Stopwatch{}) {}

   auto StopwatchesNumber() const noexcept -> wlf::usize;
   auto BufferedFramesNumber() const noexcept -> wlf::usize;
   auto IsKeyValid(const usize key) const noexcept -> bool;
   auto IsFrameDataAccessible(const usize numFramesBack) const noexcept -> bool;

   auto StartNewFrame() noexcept -> void;
   auto BeginMeasureOf(const usize key) noexcept -> bool;
   auto EndMeasureOf(const usize key) noexcept -> bool;


   auto CurrentCumulativeTimingOf(const usize key) const noexcept
      -> std::optional<wlf::u64>;

   auto HistoricalTimingOf(const usize key,
                           const usize numFramesBack) const noexcept
      -> std::optional<wlf::u64>;
   auto HistoricalFrametime(const usize numFramesBack = 1) const noexcept
      -> std::optional<wlf::u64>;

private:
   usize m_NumFramesBuffered;
   RecordingMultiStopwatch m_ProfilePartsMultiStopwatch;
   RecordingStopwatch m_FrameTimeStopwatch;
};

} // namespace wlf::util
