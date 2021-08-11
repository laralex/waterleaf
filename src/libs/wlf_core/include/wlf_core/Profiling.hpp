#pragma once
#include "Defines.hpp"
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



namespace wlf::util::detail {

template<
   typename F,
   typename... Args,
   typename = std::enable_if_t<!std::is_member_pointer_v<std::decay_t<F>>>>
auto Invoke(F&& function, Args&&... args) noexcept(
   noexcept(std::forward<F>(function)(std::forward<Args>(args)...)))
   -> decltype(std::forward<F>(function)(std::forward<Args>(args)...)) {
   return std::forward<F>(function)(std::forward<Args>(args)...);
}

template<typename F,
         typename... Args,
         typename = std::enable_if_t<std::is_member_pointer_v<std::decay_t<F>>>,
         int      = 0>
auto Invoke(F&& function, Args&&... args) noexcept(
   noexcept(std::mem_fn(function)(std::forward<Args>(args)...)))
   -> decltype(std::mem_fn(function)(std::forward<Args>(args)...)) {
   return std::mem_fn(function)(std::forward<Args>(args)...);
}

} // namespace wlf::util::detail

namespace wlf::util {

template<typename DurationT, typename F, typename... Args>
auto ProfileInvokeDiscardResult(F&& function, Args&&... args) noexcept(noexcept(
   detail::Invoke(std::forward<F>(function), std::forward<Args>(args)...))) {
   const auto begin = hires_clock::now();
   detail::Invoke(std::forward<F>(function), std::forward<Args>(args)...);
   const auto end = hires_clock::now();
   return std::chrono::duration_cast<DurationT>(end - begin).count();
}

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

// if there's something to return - return a pair of result and time
template<typename DurationT,
         typename F,
         typename... Args,
         typename =
            std::enable_if_t<!std::is_void_v<std::invoke_result_t<F, Args...>>>,
         int = 0>
auto ProfileInvoke(F&& function, Args&&... args) noexcept(noexcept(
   detail::Invoke(std::forward<F>(function), std::forward<Args>(args)...))) {
   const auto begin = hires_clock::now();
   auto&& functionOutput =
      detail::Invoke(std::forward<F>(function), std::forward<Args>(args)...);
   const auto end = hires_clock::now();
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

   wlf::usize StopwatchesNumber() const noexcept;
   wlf::usize BufferedFramesNumber() const noexcept;
   bool IsKeyValid(const usize key) const noexcept;
   bool IsFrameDataAccessible(const usize numFramesBack) const noexcept;

   void StartNewFrame() noexcept;
   bool BeginMeasureOf(const usize key) noexcept;
   bool EndMeasureOf(const usize key) noexcept;

   std::optional<wlf::u64>
   CurrentCumulativeTimingOf(const usize key) const noexcept;
   std::optional<wlf::u64>
   HistoricalTimingOf(const usize key,
                      const usize numFramesBack) const noexcept;
   std::optional<wlf::u64>
   HistoricalFrametime(const usize numFramesBack = 1) const noexcept;

private:
   usize m_NumFramesBuffered;
   RecordingMultiStopwatch m_ProfilePartsMultiStopwatch;
   RecordingStopwatch m_FrameTimeStopwatch;
};

} // namespace wlf::util
