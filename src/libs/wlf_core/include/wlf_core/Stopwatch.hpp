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
   auto operator=(Stopwatch&&) -> Stopwatch& = default;
   auto operator=(const Stopwatch&) -> Stopwatch& = default;

   auto Beginning() const noexcept -> detail::hires_timepoint;
   auto SetBeginning(const detail::hires_timepoint pastTimepoint =
                        detail::hires_clock::now()) noexcept -> void;

   auto StoreElapsed(const bool resetBeginning = false) noexcept -> void;
   auto AddStoreElapsed(const bool resetBeginning = false) noexcept -> void;
   auto ClearElapsed() noexcept -> void;
   auto GetElapsed() const noexcept -> detail::hires_duration;
   auto GetElapsedUs() const noexcept -> wlf::u64;
   auto GetElapsedMs() const noexcept -> wlf::u64;

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
   auto operator=(RecordingStopwatch&&) -> RecordingStopwatch& = default;

   // Stopwatch delegates
   auto Beginning() const noexcept -> detail::hires_timepoint;
   auto SetBeginning(const detail::hires_timepoint pastTimepoint =
                        detail::hires_clock::now()) noexcept -> void;

   auto StoreElapsed(const bool resetBeginning = false) noexcept -> void;
   auto AddStoreElapsed(const bool resetBeginning = false) noexcept -> void;
   auto ClearElapsed() noexcept -> void;
   auto GetElapsed() const noexcept -> detail::hires_duration;
   auto GetElapsedUs() const noexcept -> wlf::u64;
   auto GetElapsedMs() const noexcept -> wlf::u64;

   // Extended functionality
   auto RecordState() noexcept -> void;
   auto ClearRecords() noexcept -> void;

   auto RecordsCapacity() const noexcept -> usize;
   auto IsRecordAvailable(const usize recordingOffset) const noexcept -> bool;
   auto RecordedElapsedUs(const usize recordingOffset) const noexcept
      -> std::optional<wlf::u64>;

private:
   Stopwatch m_Stopwatch;
   std::vector<wlf::u64> m_Records;
   usize m_RecordsEverSaved = 0;
   usize m_RecordsIt        = 0;
};

} // namespace wlf::util