#pragma once

#include "Defines.hpp"
#include "Stopwatch.hpp"
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
} // namespace

namespace wlf::utils {

class ENGINE_API MultiStopwatch;

class ENGINE_API MultiStopwatchBuilder {
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

class ENGINE_API MultiStopwatch : public INonCopyable {
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

class ENGINE_API RecordingMultiStopwatch : public INonCopyable {
public:
   explicit RecordingMultiStopwatch(usize recordsCapacity,
                                    MultiStopwatch&& stopwatches) noexcept
         : INonCopyable()
         , m_RecordsCapacity(recordsCapacity)
         , m_Records(recordsCapacity * stopwatches.StopwatchesNumber())
         , m_MultiStopwatch(std::move(stopwatches)) {
      m_Records.shrink_to_fit();
   }

   // MultiStopwatch delegates
   static std::optional<RecordingMultiStopwatch>
   FromBuilder(usize recordsCapacity, MultiStopwatchBuilder&&) noexcept;

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

   // Extending functionality

   void RecordState() noexcept;
   void ClearRecords() noexcept;

   usize RecordsCapacity() const noexcept;
   bool IsRecordAvailable(usize stateOffset) const noexcept;

   std::optional<wlf::u64>
   RecordedElapsedUsOf(usize key, usize stateOffset) const noexcept;

private:
   usize m_RecordsCapacity  = 0;
   usize m_RecordsEverSaved = 0;
   std::vector<wlf::u64> m_Records;
   usize m_RecordsIt = 0;
   MultiStopwatch m_MultiStopwatch;
};

} // namespace wlf::utils