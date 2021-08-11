#include "Stopwatch.hpp"

using namespace wlf;
using namespace wlf::util;

using duration_milli = std::chrono::duration<wlf::u64, std::milli>;
using duration_micro = std::chrono::duration<wlf::u64, std::micro>;

hires_timepoint Stopwatch::Beginning() const noexcept {
   return m_BeginningTimePoint;
}

void Stopwatch::SetBeginning(const hires_timepoint timePoint) noexcept {
   m_BeginningTimePoint = std::min(timePoint, hires_clock::now());
}

void Stopwatch::StoreElapsed(const bool resetBeginning) noexcept {
   auto now       = hires_clock::now();
   m_SavedElapsed = now - m_BeginningTimePoint;
   if(resetBeginning) { m_BeginningTimePoint = now; }
}

void Stopwatch::AddStoreElapsed(const bool resetBeginning) noexcept {
   auto now = hires_clock::now();
   m_SavedElapsed += now - m_BeginningTimePoint;
   if(resetBeginning) { m_BeginningTimePoint = now; }
}

void Stopwatch::ClearElapsed() noexcept {
   m_SavedElapsed = std::chrono::nanoseconds{0};
}

hires_clock::duration Stopwatch::GetElapsed() const noexcept {
   return m_SavedElapsed;
}

wlf::u64 Stopwatch::GetElapsedUs() const noexcept {
   return std::chrono::duration_cast<duration_micro>(m_SavedElapsed).count();
}

wlf::u64 Stopwatch::GetElapsedMs() const noexcept {
   return std::chrono::duration_cast<duration_milli>(m_SavedElapsed).count();
}

hires_timepoint RecordingStopwatch::Beginning() const noexcept {
   return m_Stopwatch.Beginning();
}
void RecordingStopwatch::SetBeginning(const hires_timepoint pastTimepoint) noexcept {
   m_Stopwatch.SetBeginning(pastTimepoint);
}

void RecordingStopwatch::StoreElapsed(const bool resetBeginning) noexcept {
   m_Stopwatch.StoreElapsed(resetBeginning);
}

void RecordingStopwatch::AddStoreElapsed(const bool resetBeginning) noexcept {
   m_Stopwatch.AddStoreElapsed(resetBeginning);
}

void RecordingStopwatch::ClearElapsed() noexcept {
   m_Stopwatch.ClearElapsed();
}

hires_duration RecordingStopwatch::GetElapsed() const noexcept {
   return m_Stopwatch.GetElapsed();
}

wlf::u64 RecordingStopwatch::GetElapsedUs() const noexcept {
   return m_Stopwatch.GetElapsedUs();
}

wlf::u64 RecordingStopwatch::GetElapsedMs() const noexcept {
   return m_Stopwatch.GetElapsedMs();
}

void RecordingStopwatch::RecordState() noexcept {
   ++m_RecordsIt;
   if(m_RecordsIt == m_Records.size()) { m_RecordsIt = 0; }
   m_Records[m_RecordsIt] = GetElapsedUs();
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