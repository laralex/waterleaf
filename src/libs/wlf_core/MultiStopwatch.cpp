#include "MultiStopwatch.hpp"

using namespace wlf;
using namespace wlf::utils;

using duration_milli = std::chrono::duration<wlf::u64, std::milli>;
using duration_micro = std::chrono::duration<wlf::u64, std::micro>;


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
      m_Stopwatches[key] = Stopwatch{};
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

void MultiStopwatch::ClearElapsedOfAll() noexcept {
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

std::optional<RecordingMultiStopwatch>
RecordingMultiStopwatch::FromBuilder(usize recordsCapacity,
                                     MultiStopwatchBuilder&& builder) noexcept {
   auto multistopwatch = MultiStopwatch::FromBuilder(std::move(builder));
   if(!multistopwatch) { return std::nullopt; }
   return std::make_optional(RecordingMultiStopwatch{
      recordsCapacity, std::move(multistopwatch.value())});
}

usize RecordingMultiStopwatch::StopwatchesNumber() const noexcept {
   return m_MultiStopwatch.StopwatchesNumber();
}

bool RecordingMultiStopwatch::IsKeyValid(usize key) const noexcept {
   return m_MultiStopwatch.IsKeyValid(key);
}

std::optional<std::string_view>
RecordingMultiStopwatch::NameOf(usize key) const noexcept {
   return m_MultiStopwatch.NameOf(key);
}

std::optional<hires_timepoint>
RecordingMultiStopwatch::BeginningOf(usize key) const noexcept {
   return m_MultiStopwatch.BeginningOf(key);
}

bool RecordingMultiStopwatch::SetBeginningOf(
   usize key,
   hires_timepoint timepointInPast) noexcept {
   return m_MultiStopwatch.SetBeginningOf(key, timepointInPast);
}

void RecordingMultiStopwatch::SetBeginningOfAll(
   hires_timepoint timepointInPast) noexcept {
   return m_MultiStopwatch.SetBeginningOfAll(timepointInPast);
}

bool RecordingMultiStopwatch::SaveElapsedOf(usize key,
                                            bool resetBeginning) noexcept {
   return m_MultiStopwatch.SaveElapsedOf(key, resetBeginning);
}
void RecordingMultiStopwatch::SaveElapsedOfAll(bool resetBeginning) noexcept {
   return m_MultiStopwatch.SaveElapsedOfAll(resetBeginning);
}

bool RecordingMultiStopwatch::AddSaveElapsedOf(usize key,
                                               bool resetBeginning) noexcept {
   return m_MultiStopwatch.AddSaveElapsedOf(key, resetBeginning);
}
void RecordingMultiStopwatch::AddSaveElapsedOfAll(
   bool resetBeginning) noexcept {
   return m_MultiStopwatch.AddSaveElapsedOfAll(resetBeginning);
}

bool RecordingMultiStopwatch::ClearElapsedOf(usize key) noexcept {
    return m_MultiStopwatch.ClearElapsedOf(key);
}
void RecordingMultiStopwatch::ClearElapsedOfAll() noexcept {
   return m_MultiStopwatch.ClearElapsedOfAll();
}

std::optional<hires_duration>
RecordingMultiStopwatch::SavedElapsedOf(usize key) const noexcept {
   return m_MultiStopwatch.SavedElapsedOf(key);
}
std::optional<wlf::u64>
RecordingMultiStopwatch::SavedElapsedUsOf(usize key) const noexcept {
   return m_MultiStopwatch.SavedElapsedUsOf(key);
}
std::optional<wlf::u64>
RecordingMultiStopwatch::SavedElapsedMsOf(usize key) const noexcept {
   return m_MultiStopwatch.SavedElapsedMsOf(key);
}

void RecordingMultiStopwatch::RecordState() noexcept {
   for(usize key = 0; key < m_MultiStopwatch.StopwatchesNumber(); ++key) {
      m_Records[m_RecordsIt] = *m_MultiStopwatch.SavedElapsedUsOf(key);
      ++m_RecordsIt;
   }
   if(m_RecordsIt == m_Records.size()) { m_RecordsIt = 0; }
   ++m_RecordsEverSaved;
}

usize RecordingMultiStopwatch::RecordsCapacity() const noexcept {
   return m_RecordsCapacity;
}

void RecordingMultiStopwatch::ClearRecords() noexcept {
   m_RecordsIt        = 0;
   m_RecordsEverSaved = 0;
}

bool RecordingMultiStopwatch::IsRecordAvailable(
   usize recordOffset) const noexcept {
   return recordOffset < m_RecordsCapacity && recordOffset < m_RecordsEverSaved;
}

std::optional<wlf::u64>
RecordingMultiStopwatch::RecordedElapsedUsOf(usize key,
                                             usize stateOffset) const noexcept {
   if(!m_MultiStopwatch.IsKeyValid(key) || !IsRecordAvailable(stateOffset)) {
      return std::nullopt;
   }
   if(stateOffset == 0) { return m_MultiStopwatch.SavedElapsedUsOf(key); }

   auto timingOffset = m_MultiStopwatch.StopwatchesNumber() * stateOffset - key;
   if(m_RecordsIt >= timingOffset) {
      return std::make_optional(m_Records[m_RecordsIt - timingOffset]);
   }
   return std::make_optional(m_Records.rbegin()[timingOffset - m_RecordsIt]);
}
