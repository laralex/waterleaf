#include <glm/glm.hpp>

#include "wlf_core/Prelude.hpp"
#include "Dummy.hpp"

namespace wlf::render {
    ENGINE_API constexpr float ExampleSin(float x) {
        return glm::sin(x);
    }
}

