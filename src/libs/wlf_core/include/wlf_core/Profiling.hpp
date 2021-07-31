#include "Defines.hpp"

#include <chrono>

namespace {

template<typename DurationT, typename F, typename... Args>
auto ProfileRun(F&& function, Args&&... args) {
   using namespace std::chrono;
   const auto begin = high_resolution_clock::now();
   // if there's something to return - return a pair of result and time
   if constexpr(!std::is_same_v<std::invoke_result_t<F&&, Args&&...>, void>) {
      auto function_output = function(std::forward<Args>(args)...);
      const auto end       = high_resolution_clock::now();
      return std::make_pair(function_output,
                            duration_cast<DurationT>(end - begin).count());
   } else { // or return just time
      function(std::forward<Args>(args)...);
      const auto end = high_resolution_clock::now();
      return duration_cast<DurationT>(end - begin).count();
   }
}

} // namespace

namespace wlf::utils {

template<typename F, typename... Args>
ENGINE_API auto ProfileInMicrosecs(F&& function, Args&&... args) {
   return ProfileRun<std::chrono::duration<wlf::u64, std::micro>>(
      std::forward<F>(function), std::forward<Args>(args)...);
}

template<typename F, typename... Args>
ENGINE_API auto ProfileInMillisecs(F&& function, Args&&... args) {
   return ProfileRun<std::chrono::duration<wlf::u64, std::milli>>(
      std::forward<F>(function), std::forward<Args>(args)...);
}

class MilliseconStopwatch {
public:
   void Reset();
   wlf::f32 SmoothedElapsed();
   wlf::u64 LastElapsed();

private:
   std::chrono::high_resolution_clock::time_point m_LastTimePoint;
   wlf::f32 m_SmoothingCoefficient;
};

} // namespace wlf::utils
