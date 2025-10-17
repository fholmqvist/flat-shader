#include "vertex.hpp"

static_assert(std::is_standard_layout<Vertex>::value, "Vertex must be standard-layout!");

void Vertex::print(int index) {
    printf("Vertex (%d)\n", index);
    printf("  Pos:     { %.1f, %.1f, %.1f }\n", pos.x, pos.y, pos.z);
    printf("  Normal:  { %.1f, %.1f, %.1f }\n", normal.x, normal.y, normal.z);
    printf("  UV:      { %.1f, %.1f }\n", uv.x, uv.y);
}
