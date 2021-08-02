#include "Profiling.hpp"

using namespace wlf;
using namespace wlf::utils;

void Stopwatch::ResetBeginning(hires_clock::time_point timePoint) noexcept {
   m_BeginningTimePoint = timePoint;
}

void Stopwatch::RecordElapsed() noexcept {
   auto now      = hires_clock::now();
   auto elapsed  = now - m_BeginningTimePoint;
   m_LastElapsed = std::max(std::chrono::nanoseconds(0), elapsed);
}

void Stopwatch::RecordElapsedThenReset() noexcept {
   auto now             = hires_clock::now();
   m_LastElapsed        = now - m_BeginningTimePoint;
   m_BeginningTimePoint = now;
}

hires_clock::time_point Stopwatch::GetBeginningTimepoint() const noexcept {
   return m_BeginningTimePoint;
}

hires_clock::duration Stopwatch::LastElapsed() const noexcept {
   return m_LastElapsed;
}

wlf::u64 Stopwatch::LastElapsedUs() const noexcept {
   return std::chrono::duration_cast<
             std::chrono::duration<wlf::u64, std::micro>>(m_LastElapsed)
      .count();
}

wlf::u64 Stopwatch::LastElapsedMs() const noexcept {
   return std::chrono::duration_cast<
             std::chrono::duration<wlf::u64, std::milli>>(m_LastElapsed)
      .count();
}

bool MultiStopwatchBuilder::IsComplete() const noexcept {
   return m_LeftToInitialize == 0;
}

MultiStopwatchBuilder&
MultiStopwatchBuilder::WithStopwatchName(usize key,
                                         std::string&& name) noexcept {
   // TODO(laralex): signal error
   if(key < m_Stopwatches.size()) {
      if(!m_Stopwatches[key].Name) { --m_LeftToInitialize; }
      m_Stopwatches[key].Name = std::make_optional(std::move(name));
   }
   return *this;
}

MultiStopwatchBuilder
MultiStopwatch::CreateBuilder(usize nStopwatches) noexcept {
   return MultiStopwatchBuilder(nStopwatches);
}

std::optional<MultiStopwatch>
MultiStopwatch::ConsumeBuilder(MultiStopwatchBuilder&& builder) noexcept {
   if(!builder.IsComplete()) { return std::nullopt; }
   return std::make_optional(MultiStopwatch{std::move(builder.m_Stopwatches)});
}

usize MultiStopwatch::StopwatchesNumber() const noexcept {
   return m_Stopwatches.size();
}

bool MultiStopwatch::IsKeyValid(usize key) const noexcept {
   return key < m_Stopwatches.size();
}

bool MultiStopwatch::ResetBeginningOf(
   usize key,
   hires_clock::time_point timePoint) noexcept {
   // TODO(laralex): signal error with exception?
   if(!IsKeyValid(key)) return false;
   m_Stopwatches[key].Stopwatch.ResetBeginning(timePoint);
   return true;
}

bool MultiStopwatch::RecordElapsedThenResetOf(usize key) noexcept {
   // TODO(laralex): signal error with exception?
   if(!IsKeyValid(key)) return false;
   m_Stopwatches[key].Stopwatch.RecordElapsedThenReset();
   return true;
}

bool MultiStopwatch::RecordElapsedOf(usize key) noexcept {
   // TODO(laralex): signal error with exception?
   if(!IsKeyValid(key)) return false;
   m_Stopwatches[key].Stopwatch.RecordElapsed();
   return true;
}

std::optional<std::string_view>
MultiStopwatch::NameOf(usize key) const noexcept {
   if(!IsKeyValid(key) || !m_Stopwatches[key].Name) return std::nullopt;
   return std::make_optional<std::string_view>(*m_Stopwatches[key].Name);
}

std::optional<wlf::u64>
MultiStopwatch::LastElapsedUsOf(usize key) const noexcept {
   if(!IsKeyValid(key)) return std::nullopt;
   return std::make_optional(m_Stopwatches[key].Stopwatch.LastElapsedUs());
}
std::optional<wlf::u64>
MultiStopwatch::LastElapsedMsOf(usize key) const noexcept {
   if(!IsKeyValid(key)) return std::nullopt;
   return std::make_optional(m_Stopwatches[key].Stopwatch.LastElapsedMs());
}

std::optional<std::chrono::high_resolution_clock::duration>
MultiStopwatch::LastElapsedOf(usize key) const noexcept {
   if(!IsKeyValid(key)) return std::nullopt;
   return std::make_optional(m_Stopwatches[key].Stopwatch.LastElapsed());
}
