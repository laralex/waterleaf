#include "Dummy.hpp"

#include "wlf_core/Prelude.hpp"

#include <glm/glm.hpp>

namespace wlf::render {

ENGINE_API constexpr float ExampleSin(float x) {
   return glm::sin(x);
}

}   // namespace wlf::render
