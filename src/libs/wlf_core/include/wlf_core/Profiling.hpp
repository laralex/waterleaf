#pragma once
#include "Defines.hpp"

#include <chrono>
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

class Stopwatch {
public:
   Stopwatch() : m_LastTimePoint(hires_clock::now()), m_LastElapsed() {}
   Stopwatch(Stopwatch&& s)
         : m_LastTimePoint(std::move(s.m_LastTimePoint))
         , m_LastElapsed(std::move(s.m_LastElapsed)) {}
   void BeginMeasure() noexcept;
   void EndMeasure() noexcept;
   void SetBeginning(hires_clock::time_point timePoint) noexcept;
   hires_clock::duration LastElapsed() const noexcept;
   wlf::u64 LastElapsedUs() const noexcept;
   wlf::u64 LastElapsedMs() const noexcept;

private:
   hires_clock::time_point m_LastTimePoint;
   hires_clock::duration m_LastElapsed;
};

namespace {
struct NamedStopwatch {
   std::optional<std::string> Name;
   Stopwatch Stopwatch;
   NamedStopwatch() : Name(std::nullopt), Stopwatch() {}
   NamedStopwatch(std::string&& name)
         : Name(std::make_optional(std::move(name))), Stopwatch() {}
   NamedStopwatch(NamedStopwatch&& s)
         : Name(std::move(s.Name)), Stopwatch(std::move(s.Stopwatch)) {}
};
} // namespace

class MultiStopwatch;

class MultiStopwatchBuilder {
public:
   MultiStopwatchBuilder(usize nStopwatches)
         : m_Stopwatches(nStopwatches), m_LeftToInitialize(nStopwatches) {}
   bool IsComplete() const noexcept;
   MultiStopwatchBuilder&
   WithStopwatchName(usize key, std::string&& displayedName) noexcept;

private:
   friend MultiStopwatch;
   std::vector<NamedStopwatch> m_Stopwatches;
   usize m_LeftToInitialize;
};

class MultiStopwatch {
public:
   MultiStopwatch() = delete;

   static MultiStopwatchBuilder CreateBuilder(usize nStopwatches) noexcept;

   static std::optional<MultiStopwatch>
   ConsumeBuilder(MultiStopwatchBuilder&&) noexcept;

   void BeginMeasureOf(usize key) noexcept;
   void EndMeasureOf(usize key) noexcept;
   bool IsKeyValid(usize key) const noexcept;

   std::optional<std::string_view> NameOf(usize key) const noexcept;
   std::optional<wlf::u64> LastElapsedUsOf(usize key) const noexcept;
   std::optional<wlf::u64> LastElapsedMsOf(usize key) const noexcept;
   std::optional<hires_clock::duration> LastElapsedOf(usize key) const noexcept;

private:
   MultiStopwatch(std::vector<NamedStopwatch>&& stopwatches)
         : m_Stopwatches(std::move(stopwatches)) {}
   std::vector<NamedStopwatch> m_Stopwatches;
};


class FramePartsProfiler {
public:
   FramePartsProfiler(MultiStopwatch&& stopwatches)
         : m_Stopwatches(std::move(stopwatches)) {}
   void StartNewFrame() noexcept;
   void FindLongestElapsed(usize nLongest) const noexcept;

private:
   MultiStopwatch m_Stopwatches;
};

} // namespace wlf::utils
