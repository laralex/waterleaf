#include "MultiStopwatch.hpp"

using namespace wlf;
using namespace wlf::utils;

using duration_milli = std::chrono::duration<wlf::u64, std::milli>;
using duration_micro = std::chrono::duration<wlf::u64, std::micro>;


bool MultiStopwatchBuilder::IsComplete() const noexcept {
   return m_LeftToInitialize == 0;
}

MultiStopwatchBuilder
MultiStopwatchBuilder::WithStopwatchName(const usize key,
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

bool MultiStopwatch::IsKeyValid(const usize key) const noexcept {
   return key < m_Stopwatches.size();
}

std::optional<std::string_view>
MultiStopwatch::NameOf(const usize key) const noexcept {
   if(!IsKeyValid(key) || !m_Names[key]) return std::nullopt;
   return std::make_optional<std::string_view>(*m_Names[key]);
}

std::optional<hires_timepoint>
MultiStopwatch::BeginningOf(const usize key) const noexcept {
   if(!IsKeyValid(key)) return std::nullopt;
   return std::make_optional(m_Stopwatches[key].Beginning());
}

bool MultiStopwatch::SetBeginningOf(const usize key,
                                    const hires_timepoint timePoint) noexcept {
   // TODO(laralex): signal error with exception?
   if(!IsKeyValid(key)) return false;
   m_Stopwatches[key].SetBeginning(timePoint);
   return true;
}

void MultiStopwatch::SetBeginningOfAll(const hires_timepoint timePoint) noexcept {
   for(auto& stopwatch : m_Stopwatches) {
      stopwatch.SetBeginning(timePoint);
   }
}

bool MultiStopwatch::SaveElapsedOf(const usize key, const bool resetBeginning) noexcept {
   // TODO(laralex): signal error with exception?
   if(!IsKeyValid(key)) return false;
   m_Stopwatches[key].SaveElapsed(resetBeginning);
   return true;
}

void MultiStopwatch::SaveElapsedOfAll(const bool resetBeginning) noexcept {
   for(auto& stopwatch : m_Stopwatches) {
      stopwatch.SaveElapsed(resetBeginning);
   }
}

bool MultiStopwatch::AddSaveElapsedOf(const usize key, const bool resetBeginning) noexcept {
   // TODO(laralex): signal error with exception?
   if(!IsKeyValid(key)) return false;
   m_Stopwatches[key].AddSaveElapsed(resetBeginning);
   return true;
}

void MultiStopwatch::AddSaveElapsedOfAll(const bool resetBeginning) noexcept {
   for(auto& stopwatch : m_Stopwatches) {
      stopwatch.AddSaveElapsed(resetBeginning);
   }
}

bool MultiStopwatch::ClearElapsedOf(const usize key) noexcept {
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
MultiStopwatch::SavedElapsedOf(const usize key) const noexcept {
   if(!IsKeyValid(key)) return std::nullopt;
   return std::make_optional(m_Stopwatches[key].SavedElapsed());
}

std::optional<wlf::u64>
MultiStopwatch::SavedElapsedUsOf(const usize key) const noexcept {
   if(!IsKeyValid(key)) return std::nullopt;
   return std::make_optional(m_Stopwatches[key].SavedElapsedUs());
}
std::optional<wlf::u64>
MultiStopwatch::SavedElapsedMsOf(const usize key) const noexcept {
   if(!IsKeyValid(key)) return std::nullopt;
   return std::make_optional(m_Stopwatches[key].SavedElapsedMs());
}

std::optional<RecordingMultiStopwatch>
RecordingMultiStopwatch::FromBuilder(const usize recordsCapacity,
                                     MultiStopwatchBuilder&& builder) noexcept {
   auto multistopwatch = MultiStopwatch::FromBuilder(std::move(builder));
   if(!multistopwatch) { return std::nullopt; }
   return std::make_optional(RecordingMultiStopwatch{
      recordsCapacity, std::move(multistopwatch.value())});
}

usize RecordingMultiStopwatch::StopwatchesNumber() const noexcept {
   return m_MultiStopwatch.StopwatchesNumber();
}

bool RecordingMultiStopwatch::IsKeyValid(const usize key) const noexcept {
   return m_MultiStopwatch.IsKeyValid(key);
}

std::optional<std::string_view>
RecordingMultiStopwatch::NameOf(const usize key) const noexcept {
   return m_MultiStopwatch.NameOf(key);
}

std::optional<hires_timepoint>
RecordingMultiStopwatch::BeginningOf(const usize key) const noexcept {
   return m_MultiStopwatch.BeginningOf(key);
}

bool RecordingMultiStopwatch::SetBeginningOf(
   const usize key,
   const hires_timepoint timepointInPast) noexcept {
   return m_MultiStopwatch.SetBeginningOf(key, timepointInPast);
}

void RecordingMultiStopwatch::SetBeginningOfAll(
   const hires_timepoint timepointInPast) noexcept {
   return m_MultiStopwatch.SetBeginningOfAll(timepointInPast);
}

bool RecordingMultiStopwatch::SaveElapsedOf(const usize key,
                                            const bool resetBeginning) noexcept {
   return m_MultiStopwatch.SaveElapsedOf(key, resetBeginning);
}
void RecordingMultiStopwatch::SaveElapsedOfAll(const bool resetBeginning) noexcept {
   return m_MultiStopwatch.SaveElapsedOfAll(resetBeginning);
}

bool RecordingMultiStopwatch::AddSaveElapsedOf(const usize key,
                                               const bool resetBeginning) noexcept {
   return m_MultiStopwatch.AddSaveElapsedOf(key, resetBeginning);
}
void RecordingMultiStopwatch::AddSaveElapsedOfAll(
   const bool resetBeginning) noexcept {
   return m_MultiStopwatch.AddSaveElapsedOfAll(resetBeginning);
}

bool RecordingMultiStopwatch::ClearElapsedOf(const usize key) noexcept {
    return m_MultiStopwatch.ClearElapsedOf(key);
}
void RecordingMultiStopwatch::ClearElapsedOfAll() noexcept {
   return m_MultiStopwatch.ClearElapsedOfAll();
}

std::optional<hires_duration>
RecordingMultiStopwatch::SavedElapsedOf(const usize key) const noexcept {
   return m_MultiStopwatch.SavedElapsedOf(key);
}
std::optional<wlf::u64>
RecordingMultiStopwatch::SavedElapsedUsOf(const usize key) const noexcept {
   return m_MultiStopwatch.SavedElapsedUsOf(key);
}
std::optional<wlf::u64>
RecordingMultiStopwatch::SavedElapsedMsOf(const usize key) const noexcept {
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
   const usize recordOffset) const noexcept {
   return recordOffset < m_RecordsCapacity && recordOffset < m_RecordsEverSaved;
}

std::optional<wlf::u64>
RecordingMultiStopwatch::RecordedElapsedUsOf(const usize key,
                                             const usize stateOffset) const noexcept {
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
