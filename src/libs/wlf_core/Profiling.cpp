#include "Profiling.hpp"

using namespace wlf;
using namespace wlf::utils;

void Stopwatch::BeginMeasure() noexcept {
   SetBeginning(std::chrono::high_resolution_clock::now());
}

void Stopwatch::EndMeasure() noexcept {
   auto now      = std::chrono::high_resolution_clock::now();
   m_LastElapsed = now - m_LastTimePoint;
}

void Stopwatch::SetBeginning(hires_clock::time_point timePoint) noexcept {
   m_LastTimePoint = timePoint;
}

std::chrono::high_resolution_clock::duration
Stopwatch::LastElapsed() const noexcept {
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
      m_Stopwatches[key].Name = std::make_optional(name);
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

bool MultiStopwatch::IsKeyValid(usize key) const noexcept {
   return key < m_Stopwatches.size();
}

void MultiStopwatch::BeginMeasureOf(usize key) noexcept {
   // TODO(laralex): signal error
   if(!IsKeyValid(key)) return;
   m_Stopwatches[key].Stopwatch.BeginMeasure();
}

void MultiStopwatch::EndMeasureOf(usize key) noexcept {
   // TODO(laralex): signal error
   if(!IsKeyValid(key)) return;
   m_Stopwatches[key].Stopwatch.EndMeasure();
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
