#include "Defines.hpp"

#include <chrono>

namespace wlf {

namespace {

template<typename DurationT, typename F, typename... Args>
std::pair<std::invoke_result_t<F&&, Args&&...>, std::int64_t>
MeasureRun(F&& function, Args&&... args) {
   using namespace std::chrono;
   auto begin           = high_resolution_clock::now();
   auto function_output = function(std::forward<Args>(args)...);
   auto end             = high_resolution_clock::now();
   return std::make_pair(function_output,
                         duration_cast<DurationT>(end - begin).count());
}

} // namespace

template<typename F, typename... Args>
ENGINE_API std::pair<std::invoke_result_t<F&&, Args&&...>, std::int64_t>
MeasureRunMicroseconds(F&& function, Args&&... args) {
   return MeasureRun<std::chrono::microseconds>(function, args...);
}

template<typename F, typename... Args>
ENGINE_API std::pair<std::invoke_result_t<F&&, Args&&...>, std::int64_t>
MeasureRunNanoseconds(F&& function, Args&&... args) {
   return MeasureRun<std::chrono::nanoseconds>(function, args...);
}

class Stopwatch {
public:
   void Reset();
   float SmoothedElapsed();
   std::int64_t LastElapsed();

private:
   std::chrono::high_resolution_clock::time_point m_LastTimePoint;
   float m_SmoothingCoefficient;
};

} // namespace wlf
