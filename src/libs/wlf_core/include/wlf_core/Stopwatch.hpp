#pragma once

#include "Defines.hpp"
#include "UtilityDefines.hpp"
#include "UtilityInterfaces.hpp"

#include <chrono>
#include <optional>
#include <vector>


namespace wlf::util {

class ENGINE_API Stopwatch {
public:
   explicit Stopwatch() noexcept
         : m_BeginningTimePoint(detail::hires_clock::now()), m_SavedElapsed() {}

   explicit Stopwatch(const Stopwatch&) = default;
   explicit Stopwatch(Stopwatch&&)      = default;
   Stopwatch& operator=(Stopwatch&&) = default;
   Stopwatch& operator=(const Stopwatch&) = default;

   detail::hires_timepoint Beginning() const noexcept;
   void SetBeginning(
      const detail::hires_timepoint pastTimepoint = detail::hires_clock::now()) noexcept;

   void StoreElapsed(const bool resetBeginning = false) noexcept;
   void AddStoreElapsed(const bool resetBeginning = false) noexcept;
   void ClearElapsed() noexcept;
   detail::hires_duration GetElapsed() const noexcept;
   wlf::u64 GetElapsedUs() const noexcept;
   wlf::u64 GetElapsedMs() const noexcept;

private:
   detail::hires_timepoint m_BeginningTimePoint;
   detail::hires_duration m_SavedElapsed;
};

class ENGINE_API RecordingStopwatch : INonCopyable {
public:
   explicit RecordingStopwatch(const usize recordsCapacity,
                               Stopwatch&& stopwatch) noexcept
         : INonCopyable()
         , m_Stopwatch(std::move(stopwatch))
         , m_Records(recordsCapacity) {
      m_Records.shrink_to_fit();
   }

   explicit RecordingStopwatch(RecordingStopwatch&&) = default;
   RecordingStopwatch& operator=(RecordingStopwatch&&) = default;

   // Stopwatch delegates
   detail::hires_timepoint Beginning() const noexcept;
   void SetBeginning(
      const detail::hires_timepoint pastTimepoint = detail::hires_clock::now()) noexcept;

   void StoreElapsed(const bool resetBeginning = false) noexcept;
   void AddStoreElapsed(const bool resetBeginning = false) noexcept;
   void ClearElapsed() noexcept;
   detail::hires_duration GetElapsed() const noexcept;
   wlf::u64 GetElapsedUs() const noexcept;
   wlf::u64 GetElapsedMs() const noexcept;

   // Extended functionality
   void RecordState() noexcept;
   void ClearRecords() noexcept;

   usize RecordsCapacity() const noexcept;
   bool IsRecordAvailable(const usize recordingOffset) const noexcept;
   std::optional<wlf::u64>
   RecordedElapsedUs(const usize recordingOffset) const noexcept;

private:
   Stopwatch m_Stopwatch;
   std::vector<wlf::u64> m_Records;
   usize m_RecordsEverSaved = 0;
   usize m_RecordsIt        = 0;
};

} // namespace wlf::util