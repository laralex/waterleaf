#include "Profiling.hpp"


using namespace wlf;
using namespace wlf::utils;

using duration_milli = std::chrono::duration<wlf::u64, std::milli>;
using duration_micro = std::chrono::duration<wlf::u64, std::micro>;

hires_timepoint Stopwatch::Beginning() const noexcept {
   return m_BeginningTimePoint;
}

void Stopwatch::SetBeginning(hires_timepoint timePoint) noexcept {
   m_BeginningTimePoint = std::min(timePoint, hires_clock::now());
}

void Stopwatch::SaveElapsed(bool resetBeginning) noexcept {
   auto now       = hires_clock::now();
   m_SavedElapsed = now - m_BeginningTimePoint;
   if(resetBeginning) { m_BeginningTimePoint = now; }
}

void Stopwatch::AddSaveElapsed(bool resetBeginning) noexcept {
   auto now = hires_clock::now();
   m_SavedElapsed += now - m_BeginningTimePoint;
   if(resetBeginning) { m_BeginningTimePoint = now; }
}

void Stopwatch::ClearElapsed() noexcept {
   m_SavedElapsed = std::chrono::nanoseconds{0};
}

hires_clock::duration Stopwatch::SavedElapsed() const noexcept {
   return m_SavedElapsed;
}

wlf::u64 Stopwatch::SavedElapsedUs() const noexcept {
   return std::chrono::duration_cast<duration_micro>(m_SavedElapsed).count();
}

wlf::u64 Stopwatch::SavedElapsedMs() const noexcept {
   return std::chrono::duration_cast<duration_milli>(m_SavedElapsed).count();
}

Stopwatch& BufferedStopwatch::InnerStopwatch() noexcept {
   return m_Stopwatch;
}

const Stopwatch& BufferedStopwatch::InnerStopwatch() const noexcept {
   return m_Stopwatch;
}

void BufferedStopwatch::PushStateToHistory() noexcept {
   m_History[m_HistoryIt] = m_Stopwatch.SavedElapsedUs();
   ++m_HistoryIt;
   if(m_HistoryIt == m_History.size()) { m_HistoryIt = 0; }
   ++m_HistoricalStatesEverSaved;
}

void BufferedMultiStopwatch::ClearHistory() noexcept {
   m_HistoryIt                 = 0;
   m_HistoricalStatesEverSaved = 0;
}

bool BufferedStopwatch::IsStateOffsetAvailable(
   usize stateOffset) const noexcept {
   return stateOffset < m_History.size()
          && stateOffset < m_HistoricalStatesEverSaved;
}

std::optional<wlf::u64>
BufferedStopwatch::HistoricalElapsedUs(usize stateOffset) const noexcept {
   if(stateOffset == 0) {
      return std::make_optional(m_Stopwatch.SavedElapsedUs());
   }
   if(!IsStateOffsetAvailable(stateOffset)) { return std::nullopt; }

   if(m_HistoryIt >= stateOffset) {
      return std::make_optional(m_History[m_HistoryIt - stateOffset]);
   }
   return std::make_optional(m_History.rbegin()[stateOffset - m_HistoryIt]);
}

bool MultiStopwatchBuilder::IsComplete() const noexcept {
   return m_LeftToInitialize == 0;
}

MultiStopwatchBuilder
MultiStopwatchBuilder::WithStopwatchName(usize key,
                                         std::string&& name) noexcept {
   // TODO(laralex): signal error
   if(key < m_Stopwatches.size()) {
      if(!m_Names[key]) { --m_LeftToInitialize; }
      m_Names[key]       = std::make_optional(std::move(name));
      m_Stopwatches[key] = {};
   }
   return *this;
}

std::optional<MultiStopwatch>
MultiStopwatch::FromBuilder(MultiStopwatchBuilder&& builder) noexcept {
   if(!builder.IsComplete()) { return std::nullopt; }
   return std::make_optional(MultiStopwatch{std::move(builder)});
}

usize MultiStopwatch::StopwatchesNumber() const noexcept {
   return m_Stopwatches.size();
}

bool MultiStopwatch::IsKeyValid(usize key) const noexcept {
   return key < m_Stopwatches.size();
}

std::optional<std::string_view>
MultiStopwatch::NameOf(usize key) const noexcept {
   if(!IsKeyValid(key) || !m_Names[key]) return std::nullopt;
   return std::make_optional<std::string_view>(*m_Names[key]);
}

std::optional<hires_timepoint>
MultiStopwatch::BeginningOf(usize key) const noexcept {
   if(!IsKeyValid(key)) return std::nullopt;
   return std::make_optional(m_Stopwatches[key].Beginning());
}

bool MultiStopwatch::SetBeginningOf(usize key,
                                        hires_timepoint timePoint) noexcept {
   // TODO(laralex): signal error with exception?
   if(!IsKeyValid(key)) return false;
   m_Stopwatches[key].SetBeginning(timePoint);
   return true;
}

void MultiStopwatch::SetBeginningOfAll(hires_timepoint timePoint) noexcept {
   for(auto& stopwatch : m_Stopwatches) {
      stopwatch.SetBeginning(timePoint);
   }
}

bool MultiStopwatch::SaveElapsedOf(usize key, bool resetBeginning) noexcept {
   // TODO(laralex): signal error with exception?
   if(!IsKeyValid(key)) return false;
   m_Stopwatches[key].SaveElapsed(resetBeginning);
   return true;
}

void MultiStopwatch::SaveElapsedOfAll(bool resetBeginning) noexcept {
   for(auto& stopwatch : m_Stopwatches) {
      stopwatch.SaveElapsed(resetBeginning);
   }
}

bool MultiStopwatch::AddSaveElapsedOf(usize key, bool resetBeginning) noexcept {
   // TODO(laralex): signal error with exception?
   if(!IsKeyValid(key)) return false;
   m_Stopwatches[key].AddSaveElapsed(resetBeginning);
   return true;
}

void MultiStopwatch::AddSaveElapsedOfAll(bool resetBeginning) noexcept {
   for(auto& stopwatch : m_Stopwatches) {
      stopwatch.AddSaveElapsed(resetBeginning);
   }
}

bool MultiStopwatch::ClearElapsedOf(usize key) noexcept {
   // TODO(laralex): signal error with exception?
   if(!IsKeyValid(key)) return false;
   m_Stopwatches[key].ClearElapsed();
   return true;
}

void MultiStopwatch::ClearElapsed() noexcept {
   for(auto& stopwatch : m_Stopwatches) {
      stopwatch.ClearElapsed();
   }
}

std::optional<std::chrono::high_resolution_clock::duration>
MultiStopwatch::SavedElapsedOf(usize key) const noexcept {
   if(!IsKeyValid(key)) return std::nullopt;
   return std::make_optional(m_Stopwatches[key].SavedElapsed());
}

std::optional<wlf::u64>
MultiStopwatch::SavedElapsedUsOf(usize key) const noexcept {
   if(!IsKeyValid(key)) return std::nullopt;
   return std::make_optional(m_Stopwatches[key].SavedElapsedUs());
}
std::optional<wlf::u64>
MultiStopwatch::SavedElapsedMsOf(usize key) const noexcept {
   if(!IsKeyValid(key)) return std::nullopt;
   return std::make_optional(m_Stopwatches[key].SavedElapsedMs());
}

MultiStopwatch& BufferedMultiStopwatch::InnerStopwatch() noexcept {
   return m_MultiStopwatch;
}

const MultiStopwatch& BufferedMultiStopwatch::InnerStopwatch() const noexcept {
   return m_MultiStopwatch;
}

void BufferedMultiStopwatch::PushStateToHistory() noexcept {
   for(usize key = 0; key < m_MultiStopwatch.StopwatchesNumber(); ++key) {
      m_History[m_HistoryIt] = *m_MultiStopwatch.SavedElapsedUsOf(key);
      ++m_HistoryIt;
   }
   if(m_HistoryIt == m_History.size()) { m_HistoryIt = 0; }
   ++m_HistoricalStatesEverSaved;
}

bool BufferedMultiStopwatch::IsKeyValid(usize key) const noexcept {
   return m_MultiStopwatch.IsKeyValid(key);
}

bool BufferedMultiStopwatch::IsStateOffsetAvailable(
   usize stateOffset) const noexcept {
   return stateOffset < m_HistoricalStatesCapacity
          && stateOffset < m_HistoricalStatesEverSaved;
}

std::optional<wlf::u64> BufferedMultiStopwatch::HistoricalElapsedUsOf(
   usize key,
   usize stateOffset) const noexcept {
   if(!m_MultiStopwatch.IsKeyValid(key)
      || !IsStateOffsetAvailable(stateOffset)) {
      return std::nullopt;
   }
   if(stateOffset == 0) { return m_MultiStopwatch.SavedElapsedUsOf(key); }

   auto timingOffset = m_MultiStopwatch.StopwatchesNumber() * stateOffset - key;
   if(m_HistoryIt >= timingOffset) {
      return std::make_optional(m_History[m_HistoryIt - timingOffset]);
   }
   return std::make_optional(m_History.rbegin()[timingOffset - m_HistoryIt]);
}

wlf::usize FrameProfiler::StopwatchesNumber() const noexcept {
   return m_ProfilePartsMultiStopwatch.InnerStopwatch().StopwatchesNumber();
}
wlf::usize FrameProfiler::BufferedFramesNumber() const noexcept {
   return m_NumFramesBuffered;
}

bool FrameProfiler::IsKeyValid(usize key) const noexcept {
   return m_ProfilePartsMultiStopwatch.IsKeyValid(key);
}

bool FrameProfiler::IsFrameDataAccessible(usize numFramesBack) const noexcept {
   return m_FrameTimeStopwatch.IsStateOffsetAvailable(numFramesBack - 1);
}

void FrameProfiler::StartNewFrame() noexcept {
   m_ProfilePartsMultiStopwatch.PushStateToHistory();
   m_ProfilePartsMultiStopwatch.InnerStopwatch().ClearElapsed();
   m_FrameTimeStopwatch.InnerStopwatch().SaveElapsed(/*resetBeginning*/ true);
   m_FrameTimeStopwatch.PushStateToHistory();
}

bool FrameProfiler::BeginMeasureOf(usize key) noexcept {
   return m_ProfilePartsMultiStopwatch.InnerStopwatch().SetBeginningOf(key);
}

bool FrameProfiler::EndMeasureOf(usize key) noexcept {
   return m_ProfilePartsMultiStopwatch.InnerStopwatch().AddSaveElapsedOf(key);
}

std::optional<wlf::u64>
FrameProfiler::CurrentCumulativeTimingOf(usize key) const noexcept {
   return m_ProfilePartsMultiStopwatch.InnerStopwatch().SavedElapsedUsOf(key);
}

std::optional<wlf::u64>
FrameProfiler::HistoricalTimingOf(usize key,
                                  usize numFramesBack) const noexcept {
   if(numFramesBack == 0) return std::nullopt;
   return m_ProfilePartsMultiStopwatch.HistoricalElapsedUsOf(key,
                                                             numFramesBack - 1);
}

std::optional<wlf::u64>
FrameProfiler::HistoricalFrametime(usize numFramesBack) const noexcept {
   if(numFramesBack == 0) return std::nullopt;
   return m_FrameTimeStopwatch.HistoricalElapsedUs(numFramesBack - 1);
}
