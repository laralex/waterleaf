#pragma once

#include "Defines.hpp"
#include "UtilityInterfaces.hpp"

#include <chrono>
#include <optional>
#include <vector>

namespace {
using hires_clock     = std::chrono::high_resolution_clock;
using hires_duration  = hires_clock::duration;
using hires_timepoint = hires_clock::time_point;
} // namespace

namespace wlf::utils {

class ENGINE_API Stopwatch {
public:
   explicit Stopwatch() noexcept
         : m_BeginningTimePoint(hires_clock::now()), m_SavedElapsed() {}

   explicit Stopwatch(const Stopwatch&) = default;
   explicit Stopwatch(Stopwatch&&)      = default;
   Stopwatch& operator=(Stopwatch&&) = default;
   Stopwatch& operator=(const Stopwatch&) = default;

   hires_timepoint Beginning() const noexcept;
   void
   SetBeginning(hires_timepoint pastTimepoint = hires_clock::now()) noexcept;

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

class ENGINE_API RecordingStopwatch : INonCopyable {
public:
   explicit RecordingStopwatch(usize recordsCapacity,
                               Stopwatch&& stopwatch) noexcept
         : INonCopyable()
         , m_Stopwatch(std::move(stopwatch))
         , m_Records(recordsCapacity) {
      m_Records.shrink_to_fit();
   }

   explicit RecordingStopwatch(RecordingStopwatch&&) = default;
   RecordingStopwatch& operator=(RecordingStopwatch&&) = default;

   // Stopwatch delegates
   hires_timepoint Beginning() const noexcept;
   void
   SetBeginning(hires_timepoint pastTimepoint = hires_clock::now()) noexcept;

   void SaveElapsed(bool resetBeginning = false) noexcept;
   void AddSaveElapsed(bool resetBeginning = false) noexcept;
   void ClearElapsed() noexcept;
   hires_duration SavedElapsed() const noexcept;
   wlf::u64 SavedElapsedUs() const noexcept;
   wlf::u64 SavedElapsedMs() const noexcept;
   
   // Extended functionality
   void RecordState() noexcept;
   void ClearRecords() noexcept;

   usize RecordsCapacity() const noexcept;
   bool IsRecordAvailable(usize recordingOffset) const noexcept;
   std::optional<wlf::u64>
   RecordedElapsedUs(usize recordingOffset) const noexcept;

private:
   Stopwatch m_Stopwatch;
   std::vector<wlf::u64> m_Records;
   usize m_RecordsEverSaved = 0;
   usize m_RecordsIt        = 0;
};

} // namespace wlf::utils