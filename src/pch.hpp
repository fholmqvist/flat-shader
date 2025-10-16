#pragma once

/* -------------------------------------------------------------------------- */
/*                                    GLEW                                    */
/* -------------------------------------------------------------------------- */
#include <GL/glew.h>

/* -------------------------------------------------------------------------- */
/*                                     GLM                                    */
/* -------------------------------------------------------------------------- */
#include <glm/ext/vector_float3.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

using namespace glm;

/* -------------------------------------------------------------------------- */
/*                                     SDL                                    */
/* -------------------------------------------------------------------------- */
#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_timer.h>

/* -------------------------------------------------------------------------- */
/*                                     STD                                    */
/* -------------------------------------------------------------------------- */
#include <algorithm>      // IWYU pragma: keep
#include <array>          // IWYU pragma: keep
#include <bitset>         // IWYU pragma: keep
#include <chrono>         // IWYU pragma: keep
#include <cstdarg>        // IWYU pragma: keep
#include <cstdint>        // IWYU pragma: keep
#include <cstdio>         // IWYU pragma: keep
#include <fstream>        // IWYU pragma: keep
#include <functional>     // IWYU pragma: keep
#include <iomanip>        // IWYU pragma: keep
#include <numeric>        // IWYU pragma: keep
#include <optional>       // IWYU pragma: keep
#include <queue>          // IWYU pragma: keep
#include <set>            // IWYU pragma: keep
#include <sstream>        // IWYU pragma: keep
#include <stack>          // IWYU pragma: keep
#include <stdexcept>      // IWYU pragma: keep
#include <stdlib.h>       // IWYU pragma: keep
#include <string>         // IWYU pragma: keep
#include <sys/resource.h> // IWYU pragma: keep
#include <thread>         // IWYU pragma: keep
#include <unordered_set>  // IWYU pragma: keep
#include <variant>        // IWYU pragma: keep
#include <vector>         // IWYU pragma: keep

/* -------------------------------------------------------------------------- */
/*                                   ASSIMP                                   */
/* -------------------------------------------------------------------------- */
#include <assimp/Importer.hpp>
#include <assimp/material.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
