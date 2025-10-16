#pragma once

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef uintptr_t uptr;
typedef ptrdiff_t size;
typedef size_t usize;

typedef u64 ID;

#ifndef M_PI
#define M_PI (float)3.14159265358979323846
#endif

#define DEG2RAD(x) ((x) * (M_PI / 180.0f))

void log_info(const char* fmt, ...);
void log_warn(const char* fmt, ...);
void log_dang(const char* fmt, ...);
std::string frmt(const char* fmt, ...);
std::pair<int, int> aspect_ratio(int width, int height);
std::string time_to_string(u64 time, int precision = 4);
usize MB(usize b);
usize KB(usize b);
usize B_AS_MB(usize b);
usize B_AS_KB(usize b);
float angle_diff(float a, float b);
bool float_eq(float, float);
bool float_eq_n_decimals(float a, float b, u8 decimals);
vec3 forward_vector(vec3& rot);
