#include "base.hpp"

std::string cyan_string(const std::string &str) {
    return "\033[36m" + str + "\033[0m";
}

std::string yellow_string(const std::string &str) {
    return "\033[33m" + str + "\033[0m";
}

std::string red_string(const std::string &str) {
    return "\033[31m" + str + "\033[0m";
}

void log_info(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    printf("%s %s\n", cyan_string("[INFO]").c_str(), buffer);
}

void log_warn(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    printf("%s %s\n", yellow_string("[WARN]").c_str(), buffer);
}

void log_dang(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    printf("%s %s\n", yellow_string("[DANG]").c_str(), buffer);
    abort();
}

std::string frmt(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    return std::string(buffer);
}

std::pair<int, int> aspect_ratio(int width, int height) {
    int gcd = std::gcd(width, height);
    return { width / gcd, height / gcd };
}

std::string time_to_string(u64 time, int precision) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision);

    if (time < 1000) {
        oss << time << "us";
    } else {
        oss << (time / 1000) << "ms";
    }

    return oss.str();
}

usize MB(usize b) {
    return b * 1024 * 1024;
}

usize KB(usize b) {
    return b * 1024;
}

usize B_AS_MB(usize b) {
    return b / 1024 / 1024;
}

usize B_AS_KB(usize b) {
    return b / 1024;
}

float angle_diff(float a, float b) {
    float diff = fmod(b - a + pi<float>(), two_pi<float>());
    if (diff < 0) {
        diff += two_pi<float>();
    }
    return diff - pi<float>();
}

bool float_eq(float a, float b) {
    constexpr float epsilon = std::numeric_limits<float>::epsilon();

    if (std::isinf(a) || std::isinf(b)) {
        return a == b;
    }

    if (std::isnan(a) || std::isnan(b)) {
        return false;
    }

    float diff = std::fabs(a - b);

    if (diff <= epsilon) {
        return true;
    }

    return diff <= epsilon * std::fmax(std::fabs(a), std::fabs(b));
}

float round_to_n_decimals(float v, int n) {
    auto scale = std::pow(10.0, n);
    return std::round(v * scale) / scale;
}

bool float_eq_n_decimals(float a, float b, u8 decimals) {
    a = round_to_n_decimals(a, decimals);
    b = round_to_n_decimals(b, decimals);
    return float_eq(a, b);
}

vec3 forward_vector(vec3 &rot) {
    float pitch = rot.x;
    float yaw = rot.y;

    vec3 forward;
    forward.x = cos(pitch) * sin(yaw);
    forward.y = sin(pitch);
    forward.z = cos(pitch) * cos(yaw);

    return normalize(forward);
}
