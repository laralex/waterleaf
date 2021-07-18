#include "wlf_render/Dummy.hpp"

#include "wlf_core/Prelude.hpp"

#include <glm/glm.hpp>
#include <string_view>

namespace wlf::render {

constexpr float ExampleSin(float x) {
   auto s = std::string_view{"Whatever"};
   return glm::sin(x) + ((float)s.length() * 0.0f);
}

} // namespace wlf::render
