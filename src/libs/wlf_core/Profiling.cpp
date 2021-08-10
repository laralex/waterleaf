#include "Profiling.hpp"


using namespace wlf;
using namespace wlf::utils;

using duration_milli = std::chrono::duration<wlf::u64, std::milli>;
using duration_micro = std::chrono::duration<wlf::u64, std::micro>;

wlf::usize FrameProfiler::StopwatchesNumber() const noexcept {
   return m_ProfilePartsMultiStopwatch.StopwatchesNumber();
}
wlf::usize FrameProfiler::BufferedFramesNumber() const noexcept {
   return m_NumFramesBuffered;
}

bool FrameProfiler::IsKeyValid(usize key) const noexcept {
   return m_ProfilePartsMultiStopwatch.IsKeyValid(key);
}

bool FrameProfiler::IsFrameDataAccessible(usize numFramesBack) const noexcept {
   return m_FrameTimeStopwatch.IsRecordAvailable(numFramesBack - 1);
}

void FrameProfiler::StartNewFrame() noexcept {
   m_ProfilePartsMultiStopwatch.RecordState();
   m_ProfilePartsMultiStopwatch.ClearElapsedOfAll();
   m_FrameTimeStopwatch.SaveElapsed(/*resetBeginning*/ true);
   m_FrameTimeStopwatch.RecordState();
}

bool FrameProfiler::BeginMeasureOf(usize key) noexcept {
   return m_ProfilePartsMultiStopwatch.SetBeginningOf(key);
}

bool FrameProfiler::EndMeasureOf(usize key) noexcept {
   return m_ProfilePartsMultiStopwatch.AddSaveElapsedOf(key);
}

std::optional<wlf::u64>
FrameProfiler::CurrentCumulativeTimingOf(usize key) const noexcept {
   return m_ProfilePartsMultiStopwatch.SavedElapsedUsOf(key);
}

std::optional<wlf::u64>
FrameProfiler::HistoricalTimingOf(usize key,
                                  usize numFramesBack) const noexcept {
   if(numFramesBack == 0) return std::nullopt;
   return m_ProfilePartsMultiStopwatch.RecordedElapsedUsOf(key,
                                                           numFramesBack - 1);
}

std::optional<wlf::u64>
FrameProfiler::HistoricalFrametime(usize numFramesBack) const noexcept {
   if(numFramesBack == 0) return std::nullopt;
   return m_FrameTimeStopwatch.RecordedElapsedUs(numFramesBack - 1);
}
