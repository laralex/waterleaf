#include "wlf_render/Dummy.hpp"

#include "wlf_core/Prelude.hpp"

#include <glm/glm.hpp>
#include <string_view>

namespace wlf::render {

constexpr float ExampleSin(float x) {
   auto s = std::string_view{"Whatever"};
   for(int i = 0; i < 10000; ++i) {
      i = (int)((float)i*0.0f) + i;
   }
   return glm::sin(x) + ((float)s.length() * 0.0f);
}

} // namespace wlf::render
