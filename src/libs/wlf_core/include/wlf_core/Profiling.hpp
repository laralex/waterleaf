#pragma once
#include "Defines.hpp"
#include "UtilityInterfaces.hpp"

#include <chrono>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>


namespace {

using hires_clock = std::chrono::high_resolution_clock;

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

class ENGINE_API Stopwatch {
public:
   Stopwatch() : m_BeginningTimePoint(hires_clock::now()), m_LastElapsed() {}

   void ResetBeginning(
      hires_clock::time_point fromTimePoint = hires_clock::now()) noexcept;
   void RecordElapsed() noexcept;
   void RecordElapsedThenReset() noexcept;
   hires_clock::time_point GetBeginningTimepoint() const noexcept;
   hires_clock::duration LastElapsed() const noexcept;
   wlf::u64 LastElapsedUs() const noexcept;
   wlf::u64 LastElapsedMs() const noexcept;

private:
   hires_clock::time_point m_BeginningTimePoint;
   hires_clock::duration m_LastElapsed;
};

class ENGINE_API MultiStopwatch;

class ENGINE_API MultiStopwatchBuilder {
public:
   MultiStopwatchBuilder(usize nStopwatches)
         : m_Stopwatches(nStopwatches)
         , m_Names(nStopwatches)
         , m_LeftToInitialize(nStopwatches) {}
   bool IsComplete() const noexcept;
   MultiStopwatchBuilder
   WithStopwatchName(usize key, std::string&& displayedName) noexcept;

private:
   friend MultiStopwatch;
   std::vector<Stopwatch> m_Stopwatches;
   std::vector<std::optional<std::string>> m_Names;
   usize m_LeftToInitialize;
};

class ENGINE_API MultiStopwatch : public INonCopyable {
public:
   MultiStopwatch()                 = delete;
   MultiStopwatch(MultiStopwatch&&) = default;
   MultiStopwatch& operator=(MultiStopwatch&&) = default;

   static MultiStopwatchBuilder CreateBuilder(usize nStopwatches) noexcept;

   static std::optional<MultiStopwatch>
   FromBuilder(MultiStopwatchBuilder&&) noexcept;

   usize StopwatchesNumber() const noexcept;

   bool ResetBeginningOf(
      usize key,
      hires_clock::time_point fromTimePoint = hires_clock::now()) noexcept;
   bool RecordElapsedThenResetOf(usize key) noexcept;
   bool RecordElapsedOf(usize key) noexcept;
   bool IsKeyValid(usize key) const noexcept;

   std::optional<std::string_view> NameOf(usize key) const noexcept;
   std::optional<wlf::u64> LastElapsedUsOf(usize key) const noexcept;
   std::optional<wlf::u64> LastElapsedMsOf(usize key) const noexcept;
   std::optional<hires_clock::duration> LastElapsedOf(usize key) const noexcept;

private:
   MultiStopwatch(MultiStopwatchBuilder&& builder)
         : m_Stopwatches(std::move(builder.m_Stopwatches))
         , m_Names(std::move(builder.m_Names)) {}
   std::vector<Stopwatch> m_Stopwatches;
   std::vector<std::optional<std::string>> m_Names;
};


class ENGINE_API FrameProfiler : public INonCopyable {
public:
   FrameProfiler(MultiStopwatch&& stopwatches)
         : m_PartsStopwatches(std::move(stopwatches)), m_FrameTimeStopwatch() {}
   void StartNewFrame() noexcept {}
   void FindLongestElapsed(usize nLongest) const noexcept;

private:
   MultiStopwatch m_PartsStopwatches;
   Stopwatch m_FrameTimeStopwatch;
};

} // namespace wlf::utils
