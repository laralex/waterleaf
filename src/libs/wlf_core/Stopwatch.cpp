#include "Stopwatch.hpp"

using namespace wlf;
using namespace wlf::utils;

using duration_milli = std::chrono::duration<wlf::u64, std::milli>;
using duration_micro = std::chrono::duration<wlf::u64, std::micro>;

hires_timepoint Stopwatch::Beginning() const noexcept {
   return m_BeginningTimePoint;
}

void Stopwatch::SetBeginning(const hires_timepoint timePoint) noexcept {
   m_BeginningTimePoint = std::min(timePoint, hires_clock::now());
}

void Stopwatch::SaveElapsed(const bool resetBeginning) noexcept {
   auto now       = hires_clock::now();
   m_SavedElapsed = now - m_BeginningTimePoint;
   if(resetBeginning) { m_BeginningTimePoint = now; }
}

void Stopwatch::AddSaveElapsed(const bool resetBeginning) noexcept {
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

hires_timepoint RecordingStopwatch::Beginning() const noexcept {
   return m_Stopwatch.Beginning();
}
void RecordingStopwatch::SetBeginning(const hires_timepoint pastTimepoint) noexcept {
   m_Stopwatch.SetBeginning(pastTimepoint);
}

void RecordingStopwatch::SaveElapsed(const bool resetBeginning) noexcept {
   m_Stopwatch.SaveElapsed(resetBeginning);
}

void RecordingStopwatch::AddSaveElapsed(const bool resetBeginning) noexcept {
   m_Stopwatch.AddSaveElapsed(resetBeginning);
}

void RecordingStopwatch::ClearElapsed() noexcept {
   m_Stopwatch.ClearElapsed();
}

hires_duration RecordingStopwatch::SavedElapsed() const noexcept {
   return m_Stopwatch.SavedElapsed();
}

wlf::u64 RecordingStopwatch::SavedElapsedUs() const noexcept {
   return m_Stopwatch.SavedElapsedUs();
}

wlf::u64 RecordingStopwatch::SavedElapsedMs() const noexcept {
   return m_Stopwatch.SavedElapsedMs();
}

void RecordingStopwatch::RecordState() noexcept {
   ++m_RecordsIt;
   if(m_RecordsIt == m_Records.size()) { m_RecordsIt = 0; }
   m_Records[m_RecordsIt] = SavedElapsedUs();
   ++m_RecordsEverSaved;
}

void RecordingStopwatch::ClearRecords() noexcept {
   m_RecordsIt        = 0;
   m_RecordsEverSaved = 0;
}

usize RecordingStopwatch::RecordsCapacity() const noexcept {
   return m_Records.size();
}

bool RecordingStopwatch::IsRecordAvailable(const usize recordOffset) const noexcept {
   return recordOffset < m_Records.size() && recordOffset < m_RecordsEverSaved;
}

std::optional<wlf::u64>
RecordingStopwatch::RecordedElapsedUs(const usize recordOffset) const noexcept {
   if(!IsRecordAvailable(recordOffset)) { return std::nullopt; }

   if(m_RecordsIt >= recordOffset) {
      return std::make_optional(m_Records[m_RecordsIt - recordOffset]);
   }
   return std::make_optional(
      m_Records[m_Records.size() - recordOffset + m_RecordsIt]);
}