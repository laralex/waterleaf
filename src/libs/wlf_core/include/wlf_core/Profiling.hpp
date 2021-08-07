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

class ENGINE_API Stopwatch final {
public:
   explicit Stopwatch() noexcept
         : m_BeginningTimePoint(hires_clock::now()), m_SavedElapsed() {}

   hires_timepoint Beginning() const noexcept;
   void
   SetBeginning(hires_timepoint timePointInPast = hires_clock::now()) noexcept;

   void SaveElapsed(bool resetBeginning = false) noexcept;
   void AddSaveElapsed(bool resetBeginning = false) noexcept;
   void ClearElapsed() noexcept;
   hires_duration SavedElapsed() const noexcept;
   wlf::u64 SavedElapsedUs() const noexcept;
   wlf::u64 SavedElapsedMs() const noexcept;

private:
   hires_timepoint m_BeginningTimePoint;
   hires_duration m_SavedElapsed;
};

class ENGINE_API RecordingStopwatch final : public INonCopyable {
public:
   explicit RecordingStopwatch(usize recordsCapacity,
                               Stopwatch&& stopwatch) noexcept
         : INonCopyable()
         , m_Stopwatch(std::move(stopwatch))
         , m_Records(recordsCapacity)
         , m_RecordsEverSaved(0)
         , m_RecordsIt(0) {
      m_Records.shrink_to_fit();
   }

   Stopwatch& InnerStopwatch() noexcept;
   const Stopwatch& InnerStopwatch() const noexcept;

   void RecordState() noexcept;
   void ClearRecords() noexcept;

   usize RecordsCapacity() const noexcept;
   bool IsRecordAvailable(usize recordingOffset) const noexcept;
   std::optional<wlf::u64>
   RecordedElapsedUs(usize recordingOffset) const noexcept;

private:
   Stopwatch m_Stopwatch;
   std::vector<wlf::u64> m_Records;
   usize m_RecordsEverSaved;
   usize m_RecordsIt;
};

class ENGINE_API MultiStopwatch;

class ENGINE_API MultiStopwatchBuilder final {
public:
   explicit MultiStopwatchBuilder(usize nStopwatches) noexcept
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

class ENGINE_API MultiStopwatch final : public INonCopyable {
public:
   explicit MultiStopwatch()                 = delete;
   explicit MultiStopwatch(MultiStopwatch&&) = default;
   MultiStopwatch& operator=(MultiStopwatch&&) = default;

   static std::optional<MultiStopwatch>
   FromBuilder(MultiStopwatchBuilder&&) noexcept;

   usize StopwatchesNumber() const noexcept;
   bool IsKeyValid(usize key) const noexcept;
   std::optional<std::string_view> NameOf(usize key) const noexcept;

   std::optional<hires_timepoint> BeginningOf(usize key) const noexcept;

   bool SetBeginningOf(
      usize key,
      hires_timepoint timepointInPast = hires_clock::now()) noexcept;

   void SetBeginningOfAll(
      hires_timepoint timepointInPast = hires_clock::now()) noexcept;

   bool SaveElapsedOf(usize key, bool resetBeginning = false) noexcept;
   void SaveElapsedOfAll(bool resetBeginning = false) noexcept;

   bool AddSaveElapsedOf(usize key, bool resetBeginning = false) noexcept;
   void AddSaveElapsedOfAll(bool resetBeginning = false) noexcept;

   bool ClearElapsedOf(usize key) noexcept;
   void ClearElapsedOfAll() noexcept;

   std::optional<hires_duration> SavedElapsedOf(usize key) const noexcept;
   std::optional<wlf::u64> SavedElapsedUsOf(usize key) const noexcept;
   std::optional<wlf::u64> SavedElapsedMsOf(usize key) const noexcept;

private:
   explicit MultiStopwatch(MultiStopwatchBuilder&& builder) noexcept
         : m_Stopwatches(std::move(builder.m_Stopwatches))
         , m_Names(std::move(builder.m_Names)) {}
   std::vector<Stopwatch> m_Stopwatches;
   std::vector<std::optional<std::string>> m_Names;
};

class RecordingMultiStopwatch final : public INonCopyable {
public:
   explicit RecordingMultiStopwatch(usize recordsCapacity,
                                    MultiStopwatch&& stopwatches) noexcept
         : INonCopyable()
         , m_RecordsCapacity(recordsCapacity)
         , m_Records(recordsCapacity * stopwatches.StopwatchesNumber())
         , m_MultiStopwatch(std::move(stopwatches)) {
      m_Records.shrink_to_fit();
   }

   MultiStopwatch& InnerStopwatch() noexcept;
   const MultiStopwatch& InnerStopwatch() const noexcept;
   void RecordState() noexcept;
   void ClearRecords() noexcept;

   usize RecordsCapacity() const noexcept;
   bool IsKeyValid(usize key) const noexcept;
   bool IsRecordAvailable(usize stateOffset) const noexcept;

   std::optional<wlf::u64>
   RecordedElapsedUsOf(usize key, usize stateOffset) const noexcept;

private:
   usize m_RecordsCapacity;
   usize m_RecordsEverSaved = 0;
   std::vector<wlf::u64> m_Records;
   usize m_RecordsIt = 0;
   MultiStopwatch m_MultiStopwatch;
};

class ENGINE_API FrameProfiler final : public INonCopyable {
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
