#include "camera.hpp"

#include "base.hpp"
#include "pch.hpp" // IWYU pragma: keep
#include "window.hpp"

#include "input.hpp"

const mat4 ORTHO = ortho(0.0f, (float)SCREEN_W, (float)SCREEN_H, 0.0f, -1.0f, 1.0f);

const float DEFAULT_YAW = 0;
const float DEFAULT_PITCH = -90;

void Camera::update() {
    pos += vel * DELTA_TIME;
    const float damping = powf(0.9, DELTA_TIME * MOVE_SPEED);
    vel *= damping;

    if (length(vel) < 0.001f) {
        vel = vec3(0);
    }

    rot += vec2(mrotv.x, mrotv.y);

    mrotv *= damping;

    update_vectors();
}

void Camera::movement(vec3 move) {
    vec3 flat_front = normalize(vec3(front.x, 0, front.z));
    vec3 flat_right = normalize(cross(flat_front, { 0, 1, 0 }));

    vec3 direction = move.x * flat_right + move.z * -flat_front;

    vel += direction * DELTA_TIME * MOVE_SPEED;
}

void Camera::mouse_movement(float x_offset, float y_offset) {
    x_offset *= mouse_sensitivity * DELTA_TIME;
    y_offset *= mouse_sensitivity * DELTA_TIME;

    mrotv += vec2(x_offset, y_offset);
}

void Camera::mouse_scroll(int8_t dir) {
    if (dir == 1) {
        vel.y += 1;
    } else if (dir == -1) {
        vel.y -= 1;
    }
}

void Camera::reset_view() {
    rot = vec2(DEFAULT_YAW, DEFAULT_PITCH);
    mrotv = vec2(0);
    update_vectors();
}

void Camera::update_vectors() {
    vec3 new_front;
    new_front.x = cos(radians(rot.x)) * cos(radians(rot.y));
    new_front.y = sin(radians(rot.y));
    new_front.z = sin(radians(rot.x)) * cos(radians(rot.y));
    front = normalize(new_front);
    right = normalize(cross(front, w_up));
    up = normalize(cross(right, front));
}

void Camera::print_debug_information() {
    log_info("Camera");
    printf("  pos: { %0.1f, %0.1f, %0.1f }\n", pos.x, pos.y, pos.z);
    printf("  rot: { %0.1f, %0.1f }\n", rot.x, rot.y);
}

mat4 Camera::view_matrix() {
    return lookAt(pos, pos + front, up);
}

mat4 Camera::perspective() {
    return glm::perspective(radians(FOV), (float)SCREEN_W / (float)SCREEN_H, 0.1f, 20.0f);
}
