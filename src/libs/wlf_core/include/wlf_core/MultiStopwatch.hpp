#pragma once

#include "Defines.hpp"
#include "Stopwatch.hpp"
#include "UtilityDefines.hpp"
#include "UtilityInterfaces.hpp"

#include <chrono>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>


namespace wlf::util {

class ENGINE_API MultiStopwatch;

class ENGINE_API MultiStopwatchBuilder {
public:
   explicit MultiStopwatchBuilder(const usize nStopwatches) noexcept
         : m_Stopwatches(nStopwatches)
         , m_Names(nStopwatches)
         , m_LeftToInitialize(nStopwatches) {}
   bool IsComplete() const noexcept;
   MultiStopwatchBuilder
   WithStopwatchName(const usize key, std::string&& displayedName) noexcept;

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
   bool IsKeyValid(const usize key) const noexcept;
   std::optional<std::string_view> NameOf(const usize key) const noexcept;

   std::optional<hires_timepoint> BeginningOf(const usize key) const noexcept;

   bool SetBeginningOf(
      const usize key,
      const hires_timepoint timepointInPast = hires_clock::now()) noexcept;

   void SetBeginningOfAll(
      const hires_timepoint timepointInPast = hires_clock::now()) noexcept;

   bool StoreElapsedOf(const usize key,
                      const bool resetBeginning = false) noexcept;
   void StoreElapsedOfAll(const bool resetBeginning = false) noexcept;

   bool AddStoreElapsedOf(const usize key,
                         const bool resetBeginning = false) noexcept;
   void AddStoreElapsedOfAll(const bool resetBeginning = false) noexcept;

   bool ClearElapsedOf(const usize key) noexcept;
   void ClearElapsedOfAll() noexcept;

   std::optional<hires_duration> GetElapsedOf(const usize key) const noexcept;
   std::optional<wlf::u64> GetElapsedUsOf(const usize key) const noexcept;
   std::optional<wlf::u64> GetElapsedMsOf(const usize key) const noexcept;

private:
   explicit MultiStopwatch(MultiStopwatchBuilder&& builder) noexcept
         : m_Stopwatches(std::move(builder.m_Stopwatches))
         , m_Names(std::move(builder.m_Names)) {}
   std::vector<Stopwatch> m_Stopwatches;
   std::vector<std::optional<std::string>> m_Names;
};

class ENGINE_API RecordingMultiStopwatch : public INonCopyable {
public:
   explicit RecordingMultiStopwatch(const usize recordsCapacity,
                                    MultiStopwatch&& stopwatches) noexcept
         : INonCopyable()
         , m_RecordsCapacity(recordsCapacity)
         , m_Records(recordsCapacity * stopwatches.StopwatchesNumber())
         , m_MultiStopwatch(std::move(stopwatches)) {
      m_Records.shrink_to_fit();
   }

   // MultiStopwatch delegates
   static std::optional<RecordingMultiStopwatch>
   FromBuilder(const usize recordsCapacity, MultiStopwatchBuilder&&) noexcept;

   usize StopwatchesNumber() const noexcept;
   bool IsKeyValid(const usize key) const noexcept;
   std::optional<std::string_view> NameOf(const usize key) const noexcept;

   std::optional<hires_timepoint> BeginningOf(const usize key) const noexcept;

   bool SetBeginningOf(
      const usize key,
      const hires_timepoint timepointInPast = hires_clock::now()) noexcept;

   void SetBeginningOfAll(
      const hires_timepoint timepointInPast = hires_clock::now()) noexcept;

   bool StoreElapsedOf(const usize key,
                      const bool resetBeginning = false) noexcept;
   void StoreElapsedOfAll(const bool resetBeginning = false) noexcept;

   bool AddStoreElapsedOf(const usize key,
                         const bool resetBeginning = false) noexcept;
   void AddStoreElapsedOfAll(const bool resetBeginning = false) noexcept;

   bool ClearElapsedOf(const usize key) noexcept;
   void ClearElapsedOfAll() noexcept;

   std::optional<hires_duration> GetElapsedOf(const usize key) const noexcept;
   std::optional<wlf::u64> GetElapsedUsOf(const usize key) const noexcept;
   std::optional<wlf::u64> GetElapsedMsOf(const usize key) const noexcept;

   // Extending functionality

   void RecordState() noexcept;
   void ClearRecords() noexcept;

   usize RecordsCapacity() const noexcept;
   bool IsRecordAvailable(const usize stateOffset) const noexcept;

   std::optional<wlf::u64>
   RecordedElapsedUsOf(const usize key, const usize stateOffset) const noexcept;

private:
   usize m_RecordsCapacity  = 0;
   usize m_RecordsEverSaved = 0;
   std::vector<wlf::u64> m_Records;
   usize m_RecordsIt = 0;
   MultiStopwatch m_MultiStopwatch;
};

} // namespace wlf::util