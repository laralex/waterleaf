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
   auto IsComplete() const noexcept -> bool;
   auto WithStopwatchName(const usize key, std::string&& displayedName) noexcept
      -> MultiStopwatchBuilder;

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

   auto static FromBuilder(MultiStopwatchBuilder&&) noexcept
      -> std::optional<MultiStopwatch>;

   auto StopwatchesNumber [[nodiscard]] () const noexcept -> usize;
   auto IsKeyValid [[nodiscard]] (const usize key) const noexcept -> bool;
   auto NameOf(const usize key) const noexcept
      -> std::optional<std::string_view>;

   auto BeginningOf(const usize key) const noexcept
      -> std::optional<detail::hires_timepoint>;

   auto SetBeginningOf [[nodiscard]] (
      const usize key, const detail::hires_timepoint timepointInPast =
                          detail::hires_clock::now()) noexcept -> bool;

   void SetBeginningOfAll(
      const detail::hires_timepoint timepointInPast =
         detail::hires_clock::now()) noexcept;

   auto StoreElapsedOf [[nodiscard]] (
      const usize key, const bool resetBeginning = false) noexcept -> bool;
   void StoreElapsedOfAll(const bool resetBeginning = false) noexcept;

   auto AddStoreElapsedOf [[nodiscard]] (
      const usize key, const bool resetBeginning = false) noexcept -> bool;
   void AddStoreElapsedOfAll(const bool resetBeginning = false) noexcept;

   auto ClearElapsedOf [[nodiscard]] (const usize key) noexcept -> bool;

   void ClearElapsedOfAll() noexcept;

   auto ElapsedOf(const usize key) const noexcept
      -> std::optional<detail::hires_duration>;
   auto ElapsedUsOf(const usize key) const noexcept -> std::optional<wlf::u64>;
   auto ElapsedMsOf(const usize key) const noexcept -> std::optional<wlf::u64>;

private:
   explicit MultiStopwatch(MultiStopwatchBuilder&& builder) noexcept
         : m_Stopwatches(std::move(builder.m_Stopwatches))
         , m_Names(std::move(builder.m_Names)) {}
   std::vector<Stopwatch> m_Stopwatches;
   std::vector<std::optional<std::string>> m_Names;
};

class ENGINE_API RecordingMultiStopwatch : public INonCopyable {
public:
   explicit RecordingMultiStopwatch(
      const usize recordsCapacity, MultiStopwatch&& stopwatches) noexcept
         : INonCopyable()
         , m_RecordsCapacity(recordsCapacity)
         , m_Records(recordsCapacity * stopwatches.StopwatchesNumber())
         , m_MultiStopwatch(std::move(stopwatches)) {
      m_Records.shrink_to_fit();
   }

   // MultiStopwatch delegates
   auto static FromBuilder(
      const usize recordsCapacity, MultiStopwatchBuilder&&) noexcept
      -> std::optional<RecordingMultiStopwatch>;

   auto StopwatchesNumber [[nodiscard]] () const noexcept -> usize;
   auto IsKeyValid [[nodiscard]] (const usize key) const noexcept -> bool;
   auto NameOf(const usize key) const noexcept
      -> std::optional<std::string_view>;

   auto BeginningOf(const usize key) const noexcept
      -> std::optional<detail::hires_timepoint>;

   auto SetBeginningOf [[nodiscard]] (
      const usize key, const detail::hires_timepoint timepointInPast =
                          detail::hires_clock::now()) noexcept -> bool;

   void SetBeginningOfAll(
      const detail::hires_timepoint timepointInPast =
         detail::hires_clock::now()) noexcept;

   auto StoreElapsedOf [[nodiscard]] (
      const usize key, const bool resetBeginning = false) noexcept -> bool;
   void StoreElapsedOfAll(const bool resetBeginning = false) noexcept;

   auto AddStoreElapsedOf [[nodiscard]] (
      const usize key, const bool resetBeginning = false) noexcept -> bool;
   void AddStoreElapsedOfAll(const bool resetBeginning = false) noexcept;

   auto ClearElapsedOf [[nodiscard]] (const usize key) noexcept -> bool;
   void ClearElapsedOfAll() noexcept;

   auto ElapsedOf(const usize key) const noexcept
      -> std::optional<detail::hires_duration>;
   auto ElapsedUsOf(const usize key) const noexcept -> std::optional<wlf::u64>;
   auto ElapsedMsOf(const usize key) const noexcept -> std::optional<wlf::u64>;

   // Extending functionality

   void RecordState() noexcept;
   void ClearRecords() noexcept;

   auto RecordsCapacity [[nodiscard]] () const noexcept -> usize;
   auto IsRecordAvailable [[nodiscard]] (const usize stateOffset) const noexcept
      -> bool;

   auto
   RecordedElapsedUsOf(const usize key, const usize stateOffset) const noexcept
      -> std::optional<wlf::u64>;

private:
   usize m_RecordsCapacity  = 0;
   usize m_RecordsEverSaved = 0;
   std::vector<wlf::u64> m_Records;
   usize m_RecordsIt = 0;
   MultiStopwatch m_MultiStopwatch;
};

} // namespace wlf::util